#include "BatteryMonitor.h"

BatteryMonitor::BatteryMonitor(MainWindow& w, SerialPort& port)
        : m_Window(w), m_ArduinoPort(port), m_BatteryA(port), m_BatteryB(port) {

    m_LabelTimeElapsed = m_Window.findChild<QLabel*>("time_elapsed");
    m_LabelProgress = m_Window.findChild<QLabel*>("progress");

    // Sets the labels for each of the data points in the battery
    m_LabelAVoltage = m_Window.findChild<QLabel*>("cell_a_voltage");
    m_LabelACurrent = m_Window.findChild<QLabel*>("cell_a_current");
    m_LabelATemp = m_Window.findChild<QLabel*>("cell_a_temperature");
    m_LabelAProgress = m_Window.findChild<QLabel*>("cell_a_progress");
    m_LabelAStage = m_Window.findChild<QLabel*>("cell_a_stage");
    m_LabelAElapsed = m_Window.findChild<QLabel*>("cell_a_elapsed");

    m_LabelBVoltage = m_Window.findChild<QLabel*>("cell_b_voltage");
    m_LabelBCurrent = m_Window.findChild<QLabel*>("cell_b_current");
    m_LabelBTemp = m_Window.findChild<QLabel*>("cell_b_temperature");
    m_LabelBProgress = m_Window.findChild<QLabel*>("cell_b_progress");
    m_LabelBStage = m_Window.findChild<QLabel*>("cell_b_stage");
    m_LabelBElapsed = m_Window.findChild<QLabel*>("cell_b_elapsed");

    // Sets the arduino port for each battery so that they can communicate with the hardware
    m_BatteryA.SetArduinoPort(m_ArduinoPort);
    m_BatteryB.SetArduinoPort(m_ArduinoPort);

    // Sets the letters for each battery so that they can be identified.
    m_BatteryA.SetLetter("a");
    m_BatteryB.SetLetter("b");

    // Initializes the electronic load helper
    m_Vi.Init();
}

BatteryMonitor::~BatteryMonitor() {
    // Closes any services that were open.
    m_Logger.Close();
    m_Vi.Close();
}

void BatteryMonitor::OnReceive(SerialData data) {
    // Store results in m_BatteryA and m_BatteryB
    m_LastReceived = std::chrono::high_resolution_clock::now();

    m_LastMeasurements[m_MeasurementCounter] = data;

    if (m_MeasurementCounter == NUM_SAMPLES_TO_AVERAGE - 1) {
        SerialData averageData{};
        AverageSerialData(&averageData);

        if (m_BatteryA.GetState() == Battery::IDLE) {
            m_BatteryA.SetIdleCurrent(averageData.m_CellA_IM);
        }

        if (m_BatteryB.GetState() == Battery::IDLE) {
            m_BatteryB.SetIdleCurrent(averageData.m_CellB_IM);
        }

        m_BatteryA.SetTemp(averageData.m_CellA_TM);
        m_BatteryA.SetVoltage(averageData.m_CellA_VM);
        m_BatteryA.SetCurrent(averageData.m_CellA_IM);

        m_BatteryB.SetTemp(averageData.m_CellB_TM);
        m_BatteryB.SetVoltage(averageData.m_CellB_VM);
        m_BatteryB.SetCurrent(averageData.m_CellB_IM);

        m_JigTemperature = averageData.m_Jig_TM;
        m_VRef = averageData.m_VRef;

        static const int precision = 3;
        m_LabelAVoltage->setText(QString::number(m_BatteryA.GetVoltage(), 'g', precision) + " V");
        m_LabelATemp->setText(QString::number(m_BatteryA.GetTemp(), 'g', precision) + " °C");
        m_LabelACurrent->setText(QString::number(m_BatteryA.GetCurrent(), 'g', precision) + " A");
        m_LabelAProgress->setText(QString::number((m_BatteryA.GetCurrentSequenceState())) + "/10");
        m_LabelAStage->setText(QString::fromStdString(m_BatteryA.GetSequenceStateLabel()));

        m_LabelBVoltage->setText(QString::number(m_BatteryB.GetVoltage(), 'g', precision) + " V");
        m_LabelBTemp->setText(QString::number(m_BatteryB.GetTemp(), 'g', precision) + " °C");
        m_LabelBCurrent->setText(QString::number(m_BatteryB.GetCurrent(), 'g', precision) + " A");
        m_LabelBProgress->setText(QString::number((m_BatteryB.GetCurrentSequenceState())) + "/10");
        m_LabelBStage->setText(QString::fromStdString(m_BatteryB.GetSequenceStateLabel()));

        m_LabelProgress->setText(QString::number(((float)(m_BatteryA.GetCurrentSequenceState() + m_BatteryB.GetCurrentSequenceState()) / 20.0f * 100.0f), 'g', precision) + " %");
    }

    m_MeasurementCounter = (m_MeasurementCounter + 1) % NUM_SAMPLES_TO_AVERAGE;
}

void BatteryMonitor::AverageSerialData(SerialData* data) {
    *data = { 0, 0, 0, 0, 0, 0, 0, 0 };
    for (auto& m_LastMeasurement : m_LastMeasurements) {
        for (int j = 0; j < 8; j++) {
            data->m_Data[j] += m_LastMeasurement.m_Data[j];
        }
    }
    for (float& d : data->m_Data) {
        d /= static_cast<float>(NUM_SAMPLES_TO_AVERAGE);
    }
}

void BatteryMonitor::SetIdleDuration(int val) {
    this->m_IdleDuration = val;
}

void BatteryMonitor::Start() {
    if (m_Running)
        return;

    // Battery A should start with charging. By default they start in idle.
    m_BatteryA.SetCurrentSequenceStep(1);

    // Restarts Watchdog Timer, otherwise it will never listen to the following commands
    m_ArduinoPort.WriteSerialPort("ping\n");

    // Begins testing.
    m_BatteryA.StartStateTest();
    m_BatteryB.StartStateTest();

    // Initializes logger
    m_Logger.Init();

    m_Running = true;
    m_Thread = thread(&BatteryMonitor::Run, this);

    std::cout << "Starting Battery Monitor" << std::endl;
}

void BatteryMonitor::Stop() {
    if (!m_Running)
        return;

    // Stops electronic load and logger.
    m_Vi.StopLoad();
    m_Logger.Close();

    // Sets both batteries to idle.
    m_BatteryA.SetCurrentSequenceStep(0);
    m_BatteryB.SetCurrentSequenceStep(0);

    m_BatteryA.Idle();
    m_BatteryB.Idle();

    m_Running = false;
    m_Thread.join();

    std::cout << "Stopping Battery Monitor" << std::endl;
}

void BatteryMonitor::Run() {
    // start timer
    auto startTime = std::chrono::high_resolution_clock::now();

    // Start the timer for the batteries
    m_BatteryA.ResetStateStartTime();
    m_BatteryB.ResetStateStartTime();

    auto lastLog = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();

    m_LastReceived = std::chrono::high_resolution_clock::now();

    std::cout << "Running Battery Monitor" << std::endl;

    // pings to the arduino
    m_ArduinoPort.WriteSerialPort("ping\n");

    while (m_Running && (!m_BatteryA.IsCompleted() || !m_BatteryB.IsCompleted())) {

        now = std::chrono::high_resolution_clock::now();

        m_ArduinoPort.WriteSerialPort("ping\n");

        if (std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now() - m_LastReceived).count() >= 5) {
            m_BatteryA.Idle();
            m_BatteryB.Idle();

            m_Logger.Close();

            std::cout << "ERROR: Haven't Received Message in a while" << std::endl;

            Stop();
            return;
        }

        CheckBattery(m_BatteryA, m_BatteryB);
        CheckBattery(m_BatteryB, m_BatteryA);

        now = std::chrono::high_resolution_clock::now();

        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastLog).count() >= 10) {
            m_Logger.LogState(m_BatteryA, m_BatteryB, *this, m_Vi);
            lastLog = std::chrono::high_resolution_clock::now();
        }

        m_LabelAElapsed->setText(QString::fromStdString(StringifyDuration(m_BatteryA.GetStateDuration())));
        m_LabelBElapsed->setText(QString::fromStdString(StringifyDuration(m_BatteryB.GetStateDuration())));

        // Update time spent in the test
        auto testTimeElapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime);
        m_LabelTimeElapsed->setText(QString::fromStdString(StringifyDuration(testTimeElapsed)));

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    m_Running = false;
    m_Logger.Close();
}

void BatteryMonitor::CheckBattery(Battery& battery,
                                  Battery& secondaryBattery) {

    if (battery.GetState() == Battery::CHARGING &&
        (std::abs(battery.GetCurrent()) <= Battery::LOWEST_CURRENT ||
        battery.GetVoltage() >= Battery::HIGHEST_VOLTAGE) &&
        battery.GetStateDuration().count() >= 5) {

        battery.CompleteState();
        battery.ResetStateStartTime();

        std::cout << "UPDATE: Battery " << battery.GetLetter() << " is exiting charge and is now " << battery.GetSequenceStateLabel() << "." << std::endl;

    } else if ( battery.GetState() == Battery::DISCHARGING &&
              ( battery.GetVoltage() <= Battery::LOWEST_VOLTAGE || battery.GetCurrent() < 0 ) &&
                battery.GetStateDuration().count() >= 5) {

        m_Vi.StopLoad();
        battery.CompleteState();

        battery.ResetStateStartTime();

        std::cout << "UPDATE: Battery " << battery.GetLetter() << " is exiting discharge and is now " << battery.GetSequenceStateLabel() << "." << std::endl;

    } else if (battery.GetState() == Battery::IDLE || battery.IsCompleted()) {
        if (battery.GetStateDuration().count() >= m_IdleDuration || battery.IsCompleted()) {
            battery.SetReady(true);
        }

        if (battery.IsReady() && secondaryBattery.IsReady()) {
            battery.CompleteState();
            secondaryBattery.CompleteState();

            // restart timers
            battery.ResetStateStartTime();
            secondaryBattery.ResetStateStartTime();

            battery.SetReady(false);
            secondaryBattery.SetReady(false);

            std::cout << "UPDATE: Battery " << battery.GetLetter() << " is now " << battery.GetSequenceStateLabel() << "." << std::endl;

            if (battery.GetState() == Battery::DISCHARGING ||
                secondaryBattery.GetState() == Battery::DISCHARGING) {
                m_Vi.StartLoad();
                std::cout << "UPDATE: Starting Load." << std::endl;
            }

        }

    }
}

std::string BatteryMonitor::StringifyDuration(std::chrono::seconds input_seconds) {
    using namespace std::chrono;
    auto h = duration_cast<hours>(input_seconds);
    input_seconds -= h;
    auto m = duration_cast<minutes>(input_seconds);
    input_seconds -= m;
    auto s = duration_cast<seconds>(input_seconds);

    auto hc = h.count();
    auto mc = m.count();
    auto sc = s.count();

    std::stringstream ss;
    ss.fill('0');

    ss << std::setw(2) << hc << ":" << std::setw(2) << mc << ":" << std::setw(2) << sc;

    return ss.str();
}
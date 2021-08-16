#include "BatteryMonitor.h"

BatteryMonitor::BatteryMonitor(MainWindow& w, SerialPort& port)
        : m_Window(w), m_ArduinoPort(port), m_BatteryA(port), m_BatteryB(port) {

    // Sets the labels for each of the data points in the battery
    m_LabelAVoltage = m_Window.findChild<QLabel*>("cell_a_voltage");
    m_LabelACurrent = m_Window.findChild<QLabel*>("cell_a_current");
    m_LabelATemp = m_Window.findChild<QLabel*>("cell_a_temperature");
    m_LabelAStage = m_Window.findChild<QLabel*>("cell_a_stage");
    m_LabelAElapsed = m_Window.findChild<QLabel*>("cell_a_elapsed");

    m_LabelBVoltage = m_Window.findChild<QLabel*>("cell_b_voltage");
    m_LabelBCurrent = m_Window.findChild<QLabel*>("cell_b_current");
    m_LabelBTemp = m_Window.findChild<QLabel*>("cell_b_temperature");
    m_LabelBStage = m_Window.findChild<QLabel*>("cell_b_stage");
    m_LabelBElapsed = m_Window.findChild<QLabel*>("cell_b_elapsed");

    // Sets the arduino port for each battery so that they can communicate with the hardware
    m_BatteryA.SetArduinoPort(m_ArduinoPort);
    m_BatteryB.SetArduinoPort(m_ArduinoPort);

    // Sets the letters for each battery so that they can be identified.
    m_BatteryA.SetLetter("a");
    m_BatteryB.SetLetter("b");

    // Initializes the electronic load helper
    m_Vi.init();
}

BatteryMonitor::~BatteryMonitor() {
    m_Logger.Close();
    m_Vi.close();
}

void BatteryMonitor::OnReceive(SerialData data) {
    // Store results in m_BatteryA and m_BatteryB
    m_LastReceived = std::chrono::high_resolution_clock::now();

    m_LastMeasurements[m_MeasurementCounter] = data;
    if (m_MeasurementCounter == NUM_SAMPLES_TO_AVERAGE - 1) {
        SerialData averageData{};
        AverageSerialData(&averageData);

        UpdateBatteryData(m_BatteryA, averageData.m_CellA_TM, averageData.m_CellA_IM, averageData.m_CellA_VM);
        UpdateBatteryData(m_BatteryB, averageData.m_CellB_TM, averageData.m_CellB_IM, averageData.m_CellB_VM);

        m_JigTemperature = averageData.m_Jig_TM;
        m_VRef = averageData.m_VRef;

        static const int precision = 3;
        m_LabelAVoltage->setText(QString::number(m_BatteryA.GetVoltage(), 'g', precision) + " V");
        m_LabelATemp->setText(QString::number(m_BatteryA.GetTemp(), 'g', precision) + " °C");
        m_LabelACurrent->setText(QString::number(m_BatteryA.GetCurrent(), 'g', precision) + " A");
        m_LabelAStage->setText(QString::fromStdString(m_BatteryA.GetSequenceStateLabel()));

        m_LabelBVoltage->setText(QString::number(m_BatteryB.GetVoltage(), 'g', precision) + " V");
        m_LabelBTemp->setText(QString::number(m_BatteryB.GetTemp(), 'g', precision) + " °C");
        m_LabelBCurrent->setText(QString::number(m_BatteryB.GetCurrent(), 'g', precision) + " A");
        m_LabelBStage->setText(QString::fromStdString(m_BatteryB.GetSequenceStateLabel()));
    }

    m_MeasurementCounter = (m_MeasurementCounter + 1) % NUM_SAMPLES_TO_AVERAGE;
}

void BatteryMonitor::UpdateBatteryData(Battery& battery, float temp, float current, float voltage) {
    if (battery.GetState() == battery.IDLE) {
        battery.SetIdleCurrent(current);
    }

    battery.SetTemp(temp);
    battery.SetVoltage(voltage);
    battery.SetCurrent(current);
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

void BatteryMonitor::setIdleDuration(int val) {
    this->m_IdleDuration = val;
}

void BatteryMonitor::Start() {
    if (m_Running)
        return;

    // Battery A should start with charging. By default they start in idle.
    m_BatteryA.SetCurrentSequenceStep(1);

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

    // Stops electronic load.
    m_Vi.stopLoad();

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
    auto currentMillisA = std::chrono::high_resolution_clock::now();
    auto currentMillisB = std::chrono::high_resolution_clock::now();

    auto lastLog = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();

    m_LastReceived = std::chrono::high_resolution_clock::now();

    std::cout << "Running Battery Monitor" << std::endl;

    m_ArduinoPort.writeSerialPort("ping\n");

    while (m_Running && (!m_BatteryA.IsCompleted() || !m_BatteryB.IsCompleted())) {

        now = std::chrono::high_resolution_clock::now();

        m_ArduinoPort.writeSerialPort("ping\n");

        if (std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now() - m_LastReceived).count() >= 5) {
            m_BatteryA.Idle();
            m_BatteryB.Idle();

            m_Logger.Close();

            std::cout << "ERROR: Haven't Received Message in a while" << std::endl;

            Stop();
        }

        CheckBattery(m_BatteryA, m_BatteryB, currentMillisA, currentMillisB);
        CheckBattery(m_BatteryB, m_BatteryA, currentMillisB, currentMillisA);

        now = std::chrono::high_resolution_clock::now();

        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastLog).count() >= 10) {
            m_Logger.LogState(m_BatteryA, m_BatteryB, *this, m_Vi);
            lastLog = std::chrono::high_resolution_clock::now();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    m_Running = false;
    m_Logger.Close();
}

void BatteryMonitor::CheckBattery(Battery& battery,
                                  Battery& secondaryBattery,
                                  std::chrono::steady_clock::time_point& currentMillis,
                                  std::chrono::steady_clock::time_point& currentMillisSecondary) {

    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now() - currentMillis);

    if (battery.GetState() == Battery::CHARGING &&
        (std::abs(battery.GetCurrent()) <= Battery::LOWEST_CURRENT ||
        battery.GetVoltage() >= Battery::HIGHEST_VOLTAGE) &&
        elapsed.count() >= 5) {

        battery.CompleteState();
        // restart timer
        currentMillis = std::chrono::high_resolution_clock::now();

        std::cout << "UPDATE: Battery " << battery.GetLetter() << " is now " << battery.GetSequenceStateLabel() << "." << std::endl;

    } else if ( battery.GetState() == Battery::DISCHARGING &&
                battery.GetVoltage() <= Battery::LOWEST_VOLTAGE &&
                elapsed.count() >= 5) {

        m_Vi.stopLoad();
        battery.CompleteState();
        // restart timer
        currentMillis = std::chrono::high_resolution_clock::now();

        std::cout << "UPDATE: Battery " << battery.GetLetter() << " is now " << battery.GetSequenceStateLabel() << "." << std::endl;

    } else if (battery.GetState() == Battery::IDLE || battery.IsCompleted()) {

        if (battery.GetLetter() == "a") {
            m_LabelAElapsed->setText(QString::fromStdString(stringifyDuration(elapsed)));
        } else if (battery.GetLetter() == "b") {
            m_LabelBElapsed->setText(QString::fromStdString(stringifyDuration(elapsed)));
        }

        if (elapsed.count() >= m_IdleDuration || battery.IsCompleted()) {
            battery.SetReady(true);
        }

        if (battery.IsReady() && secondaryBattery.IsReady()) {
            battery.CompleteState();
            secondaryBattery.CompleteState();

            // restart timers
            currentMillis = std::chrono::high_resolution_clock::now();
            currentMillisSecondary = std::chrono::high_resolution_clock::now();

            battery.SetReady(false);
            secondaryBattery.SetReady(false);

            std::cout << "UPDATE: Battery " << battery.GetLetter() << " is now " << battery.GetSequenceStateLabel() << "." << std::endl;


            if (battery.GetState() == Battery::DISCHARGING ||
                secondaryBattery.GetState() == Battery::DISCHARGING) {
                m_Vi.startLoad();
                std::cout << "UPDATE: Starting Load." << std::endl;
            }

        }

    } else {

    }
}

std::string BatteryMonitor::stringifyDuration(std::chrono::seconds input_seconds) {
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
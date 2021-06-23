#include "BatteryMonitor.h"

BatteryMonitor::BatteryMonitor(MainWindow& w, SerialPort& port)
        : m_Window(w), m_ArduinoPort(port), m_BatteryA(port), m_BatteryB(port) {

    m_LabelAVoltage = m_Window.findChild<QLabel*>("cell_a_voltage");
    m_LabelACurrent = m_Window.findChild<QLabel*>("cell_a_current");
    m_LabelATemp = m_Window.findChild<QLabel*>("cell_a_temperature");
    m_LabelACharge = m_Window.findChild<QLabel*>("cell_a_charge");
    m_LabelAStage = m_Window.findChild<QLabel*>("cell_a_stage");
    m_LabelAElapsed = m_Window.findChild<QLabel*>("cell_a_elapsed");

    m_LabelBVoltage = m_Window.findChild<QLabel*>("cell_b_voltage");
    m_LabelBCurrent = m_Window.findChild<QLabel*>("cell_b_current");
    m_LabelBTemp = m_Window.findChild<QLabel*>("cell_b_temperature");
    m_LabelBCharge = m_Window.findChild<QLabel*>("cell_b_charge");
    m_LabelBStage = m_Window.findChild<QLabel*>("cell_b_stage");
    m_LabelBElapsed = m_Window.findChild<QLabel*>("cell_b_elapsed");

    m_BatteryA.setArduinoPort(m_ArduinoPort);
    m_BatteryB.setArduinoPort(m_ArduinoPort);

    m_BatteryA.setLetter("a");
    m_BatteryB.setLetter("b");

    m_Vi.init();
}

BatteryMonitor::~BatteryMonitor() {
    m_Logger.Close();
}

void BatteryMonitor::OnReceive(SerialData data) {
    // Store results in m_BatteryA and m_BatteryB
    m_LastReceived = std::chrono::high_resolution_clock::now();

    m_LastMeasurements[m_MeasurementCounter] = data;
    if (m_MeasurementCounter == NUM_SAMPLES_TO_AVERAGE - 1) {
        SerialData averageData{};
        averageSerialData(&averageData);

        if (m_BatteryA.isIdle()) {
            m_BatteryA.setIdleCurrent(averageData.m_CellA_IM);
        }

        if (m_BatteryB.isIdle()) {
            m_BatteryB.setIdleCurrent(averageData.m_CellB_IM);
        }

        m_BatteryA.setTemp(averageData.m_CellA_TM);
        m_BatteryA.setVoltage(averageData.m_CellA_VM);
        m_BatteryA.setCurrent(averageData.m_CellA_IM);

        m_BatteryB.setTemp(averageData.m_CellB_TM);
        m_BatteryB.setVoltage(averageData.m_CellB_VM);
        m_BatteryB.setCurrent(averageData.m_CellB_IM);

        m_JigTemperature = averageData.m_Jig_TM;
        m_VRef = averageData.m_VRef;

        static const int precision = 3;
        m_LabelAVoltage->setText(QString::number(m_BatteryA.getVolt(), 'g', precision) + " V");
        m_LabelATemp->setText(QString::number(m_BatteryA.getTemp(), 'g', precision) + " °C");
        m_LabelACurrent->setText(QString::number(m_BatteryA.getCurrent(), 'g', precision) + " A");
        m_LabelACharge->setText(QString::number(m_BatteryA.getCharge(), 'g', precision) + " %");
        m_LabelAStage->setText(QString::fromStdString(m_BatteryA.getStageText()));

        m_LabelBVoltage->setText(QString::number(m_BatteryB.getVolt(), 'g', precision) + " V");
        m_LabelBTemp->setText(QString::number(m_BatteryB.getTemp(), 'g', precision) + " °C");
        m_LabelBCurrent->setText(QString::number(m_BatteryB.getCurrent(), 'g', precision) + " A");
        m_LabelBCharge->setText(QString::number(m_BatteryB.getCharge(), 'g', precision) + " %");
        m_LabelBStage->setText(QString::fromStdString(m_BatteryB.getStageText()));
    }

    m_MeasurementCounter = (m_MeasurementCounter + 1) % NUM_SAMPLES_TO_AVERAGE;
}

void BatteryMonitor::averageSerialData(SerialData* data) {
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

    m_BatteryA.setStage(Battery::CHARGE1);
    m_BatteryB.setStage(Battery::IDLE0);

    m_BatteryA.charge();
    m_BatteryB.idle();

    m_BatteryA.setCompleted(false);
    m_BatteryB.setCompleted(false);

    m_Logger.Init();

    m_Running = true;
    m_Thread = thread(&BatteryMonitor::Run, this);
}

void BatteryMonitor::Stop() {
    if (!m_Running)
        return;

    m_Vi.close();

    m_BatteryA.setStage(Battery::IDLE0);
    m_BatteryB.setStage(Battery::IDLE0);

    m_BatteryA.setCompleted(true);
    m_BatteryB.setCompleted(true);

    m_BatteryA.idle();
    m_BatteryB.idle();

    m_Running = false;
    m_Thread.join();
}

void BatteryMonitor::Run() {
    auto currentMillisA = std::chrono::high_resolution_clock::now();
    auto currentMillisB = std::chrono::high_resolution_clock::now();

    auto lastLog = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();

    m_LastReceived = std::chrono::high_resolution_clock::now();

    while (m_Running && (!m_BatteryA.isCompleted() || !m_BatteryB.isCompleted())) {

        now = std::chrono::high_resolution_clock::now();

        m_ArduinoPort.writeSerialPort("ping\n");

        if (std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now() - m_LastReceived).count() >= 1) {
            m_BatteryA.idle();
            m_BatteryB.idle();

            m_Logger.Close();

            Stop();
        }

        checkBattery(m_BatteryA, m_BatteryB, currentMillisA, currentMillisB);
        checkBattery(m_BatteryB, m_BatteryA, currentMillisB, currentMillisA);

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

void BatteryMonitor::checkBattery(Battery& battery,
                                  Battery& secondaryBattery,
                                  std::chrono::steady_clock::time_point& currentMillis,
                                  std::chrono::steady_clock::time_point& currentMillisSecondary) {

    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::high_resolution_clock::now() - currentMillis);

    if (battery.getGeneralStage() == Battery::CHARGING &&
        std::abs(battery.getCurrent()) <= Battery::LOWEST_CURRENT &&
        elapsed.count() >= 10) {

        if (battery.getState() == Battery::CHARGE3) {
            battery.setCompleted(true);
        }
        battery.goNext();
        // restart timer
        currentMillis = std::chrono::high_resolution_clock::now();

    } else if ( battery.getGeneralStage() == Battery::DISCHARGING &&
                battery.getVolt() <= Battery::LOWEST_VOLTAGE &&
                elapsed.count() >= 10) {

        m_Vi.stopLoad();
        battery.goNext();
        // restart timer
        currentMillis = std::chrono::high_resolution_clock::now();

    } else if (battery.getGeneralStage() == Battery::IDLE) {

        if (battery.getLetter() == "a") {
            m_LabelAElapsed->setText(QString::fromStdString(stringifyDuration(elapsed)));
        } else if (battery.getLetter() == "b") {
            m_LabelBElapsed->setText(QString::fromStdString(stringifyDuration(elapsed)));
        }

        if (elapsed.count() >= m_IdleDuration) {
            battery.setReadyForNext(true);
        }

        if (battery.isReadyForNext() && secondaryBattery.isReadyForNext()) {
            battery.goNext();
            secondaryBattery.goNext();

            // restart timers
            currentMillis = std::chrono::high_resolution_clock::now();
            currentMillisSecondary = std::chrono::high_resolution_clock::now();

            battery.setReadyForNext(false);
            secondaryBattery.setReadyForNext(false);

            if (battery.getGeneralStage() == Battery::DISCHARGING ||
                secondaryBattery.getGeneralStage() == Battery::DISCHARGING)
                m_Vi.startLoad();
        }

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
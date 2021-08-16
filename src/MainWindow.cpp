#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "BatteryMonitor.h"

#include <iostream>

static const char * COM_PORTS[] = {
        R"(\\.\COM1)",
        R"(\\.\COM2)",
        R"(\\.\COM3)",
        R"(\\.\COM4)",
        R"(\\.\COM5)",
        R"(\\.\COM6)",
        R"(\\.\COM7)",
        R"(\\.\COM8)",
        R"(\\.\COM9)",
        R"(\\.\COM10)",
        R"(\\.\COM11)",
        R"(\\.\COM12)",
        R"(\\.\COM13)",
        R"(\\.\COM14)",
        R"(\\.\COM15)",
        R"(\\.\COM16)",
};

MainWindow::MainWindow(SerialReceiver* receiver, QWidget* parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        m_receiver(receiver)
{
    ui->setupUi(this);
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnChangeArduinoPort(int)));
    connect(ui->startButton, SIGNAL(clicked(bool)), this, SLOT(OnClickStart(bool)));
    connect(ui->stopButton, SIGNAL(clicked(bool)), this, SLOT(OnClickStop(bool)));
    connect(ui->duration, SIGNAL(valueChanged(int)), this, SLOT(OnSpinBoxChanged(int)));

}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setBatteryMonitor(BatteryMonitor * batteryMonitor) {
    m_BatteryMonitor = batteryMonitor;
}

void MainWindow::OnChangeArduinoPort(int index) {
    m_receiver->SetArduinoPort(COM_PORTS[index]);
}

void MainWindow::OnSpinBoxChanged(int val) {
    m_BatteryMonitor->setIdleDuration(val);
}

void MainWindow::OnClickStart(bool checked) {
    m_BatteryMonitor->Start();
}

void MainWindow::OnClickStop(bool checked) {
    m_BatteryMonitor->Stop();
}
#include "MainWindow.h"
#include "ui_MainWindow.h"

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
    connect(ui->startButton, SIGNAL(clicked(bool)), this, SLOT(OnclickStart(bool)));


}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::OnChangeArduinoPort(int index) {
    m_receiver->SetArduinoPort(COM_PORTS[index]);
}
void MainWindow::OnclickStart(bool checked) {
    if (m_StartCallBack)
        m_StartCallBack();
}
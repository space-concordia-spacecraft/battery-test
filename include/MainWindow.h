#pragma once

#include <QMainWindow>
#include <QWidget>

#include "SerialReceiver.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

    MainWindow(QWidget *parent);


public:
    explicit MainWindow(SerialReceiver* receiver, QWidget* parent = nullptr);

    ~MainWindow() override;
    void setStartCallBack(std::function<void(void)> startCallBack) { this->m_StartCallBack = startCallBack; }

private:
    Ui::MainWindow* ui;
    SerialReceiver* m_receiver;
    std::function<void(void)> m_StartCallBack;


private slots:
    void OnChangeArduinoPort(int index);
    void OnclickStart(bool checked);
};
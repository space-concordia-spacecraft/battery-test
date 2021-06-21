#pragma once

#include <QMainWindow>
#include <QWidget>

#include "SerialReceiver.h"

class BatteryMonitor;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(SerialReceiver* receiver, QWidget* parent = nullptr);
    void setBatteryMonitor(BatteryMonitor * batteryMonitor);
    ~MainWindow() override;

private:
    Ui::MainWindow* ui;
    SerialReceiver* m_receiver;
    BatteryMonitor* m_BatteryMonitor;

private slots:
    void OnChangeArduinoPort(int index);
    void OnClickStart(bool checked);
    void OnSpinBoxChanged(int val);
    void OnClickStop(bool checked);
};
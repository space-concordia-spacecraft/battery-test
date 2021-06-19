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

private:
    Ui::MainWindow* ui;
    SerialReceiver* m_receiver;




private slots:
    void OnChangeArduinoPort(int index);
    void OnclickStart(bool checked);
    void OnStart();
    void OnStop();
    void OnPause();
};
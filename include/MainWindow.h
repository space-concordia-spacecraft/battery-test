#pragma once

#include <QMainWindow>
#include <QWidget>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    ~MainWindow() override;

private:
    Ui::MainWindow* ui;
};
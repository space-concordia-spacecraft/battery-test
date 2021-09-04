#include <QApplication>

#include "MainWindow.h"
#include "SerialReceiver.h"
#include "BatteryMonitor.h"

int main(int argc, char** argv) {

    SerialReceiver receiver;

    QApplication a(argc, argv);
    MainWindow w(&receiver);
    w.show();

    BatteryMonitor batteryMonitor(w, receiver.GetArduinoPort());

    receiver.SetListener(&batteryMonitor);
    receiver.Start();

    w.SetBatteryMonitor(&batteryMonitor);

    QApplication::exec();

    batteryMonitor.Stop();

    receiver.Stop();

    return 0;
}
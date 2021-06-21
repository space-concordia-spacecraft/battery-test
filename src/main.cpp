#include <QApplication>

#include "MainWindow.h"
#include "SerialReceiver.h"
#include "BatteryMonitor.h"

int main(int argc, char** argv) {

    SerialReceiver receiver;

    QApplication a(argc, argv);
    MainWindow w(&receiver);
    w.show();

    BatteryMonitor batteryMonitor(w, receiver.getArduinoPort());

    receiver.SetListener(&batteryMonitor);
    receiver.Start();

    w.setStartCallBack([&batteryMonitor]() { batteryMonitor.Start(); });

    QApplication::exec();

    receiver.Stop();

    return 0;
}
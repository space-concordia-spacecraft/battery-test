#include <QApplication>

#include "MainWindow.h"
#include "SerialReceiver.h"
#include "BatteryMonitor.h"

int main(int argc, char** argv) {

    BatteryMonitor batteryMonitor;
    SerialReceiver receiver;

    receiver.SetListener(reinterpret_cast<SerialListener*>(&batteryMonitor));
    receiver.Start();

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QApplication::exec();

    receiver.Stop();

    return 0;
}
#include <QApplication>

#include "MainWindow.h"
#include "SerialReceiver.h"
#include "BatteryMonitor.h"
#include "visa.h"

void test() {
    ViSession defaultRM, vi;
    char buf[256] = { 0 };
    std::string strTemp;
    char* stringTemp;
    ViChar buffer[VI_FIND_BUFLEN];
    ViRsrc matches = buffer;
    ViUInt32 nmatches;
    ViFindList list;
    viOpenDefaultRM(&defaultRM);
    std::cout << defaultRM << std::endl;
    viFindRsrc(defaultRM, ViString("USB?*"), &list, &nmatches, matches);
    std::cout << list << std::endl;
    std::cout << nmatches << std::endl;
    std::cout << matches << std::endl;
    viOpen(defaultRM, matches, VI_NULL, VI_NULL, &vi);
    std::cout << vi << std::endl;
    viPrintf(vi, ViString("*RST\n"));
    strTemp = strTemp + "\n";
    stringTemp = (char*) strTemp.c_str();
    viPrintf(vi, stringTemp);
    viScanf(vi, ViString("%t\n"), &buf);
    viClose(vi);
    viClose(defaultRM);
}

int main(int argc, char** argv) {

    test();

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
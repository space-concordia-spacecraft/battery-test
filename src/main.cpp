#include <QApplication>

#include "MainWindow.h"
#include "SerialReceiver.h"
#include "BatteryMonitor.h"
#include "visa.h"

#include "visa.h"
#include <string>
#include <iostream>

//void printVi(const char* str, ViSession vi) {
//    char buf[256] = { 0 };
//
//    viPrintf(vi, ViString(str));
//    viScanf(vi, ViString("%t\n"), &buf);
//    std::cout << buf << std::endl;
//}
//
//int main() {
//    ViSession defaultRM, vi;
//    char buf[256] = { 0 };
//    std::string strTemp;
//    char* stringTemp;
//    ViChar buffer[VI_FIND_BUFLEN];
//    ViRsrc matches = buffer;
//    ViUInt32 nmatches;
//    ViFindList list;
//    viOpenDefaultRM(&defaultRM);
//    std::cout << defaultRM << std::endl;
//    viFindRsrc(defaultRM, ViString("USB?*"), &list, &nmatches, matches);
//    std::cout << list << std::endl;
//    std::cout << nmatches << std::endl;
//    std::cout << matches << std::endl;
//    viOpen(defaultRM, matches, VI_NULL, VI_NULL, &vi);
//    std::cout << vi << std::endl;
//
//    viPrintf(vi, ViString(":SOUR:SENS 1\n"));
//
//    printVi(":SOUR:SENS?\n", vi);
//
//    viPrintf(vi, ViString(":SOUR:FUNC:MODE BATT\n"));
//
//    printVi(":SOUR:FUNC:MODE?\n", vi);
//
//    viPrintf(vi, ViString(":SOUR:BATT:LEV:IMM 1.6\n"));
//
//    printVi(":SOUR:BATT:LEV:IMM?\n", vi);
//
//    viPrintf(vi, ViString(":SOUR:BATT:RANG 4\n"));
//
//    printVi(":SOUR:BATT:RANG?\n", vi);
//
//    viPrintf(vi, ViString(":SOUR:BATT:VON 3\n"));
//
//    printVi(":SOUR:BATT:VON?\n", vi);
//
//    viPrintf(vi, ViString(":SOUR:BATT:VEN 1\n"));
//
//    printVi(":SOUR:BATT:VEN?\n", vi);
//
//    viPrintf(vi, ViString(":SOUR:BATT:VST 3\n"));
//
//    printVi(":SOUR:BATT:VST?\n", vi);
//
//    viPrintf(vi, ViString(":SOUR:BATT:CEN 0\n"));
//
//    printVi(":SOUR:BATT:CEN?\n", vi);
//
//    viPrintf(vi, ViString(":SOUR:BATT:TEN 1\n"));
//
//    printVi(":SOUR:BATT:TEN?\n", vi);
//
//    viPrintf(vi, ViString(":SOUR:BATT:TIM 10800\n"));
//
//    printVi(":SOUR:BATT:TIM?\n", vi);
//
//    viPrintf(vi, ViString(":SOUR:INP:STAT 1\n"));
//
//    printVi(":SOUR:INP:STAT?\n", vi);
//
//    viClose(vi);
//    viClose(defaultRM);
//}

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
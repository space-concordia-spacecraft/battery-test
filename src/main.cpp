#include <iostream>

#include <QApplication>

#include "MainWindow.h"
#include "SerialPort.h"

const char * port = R"(\\.\COM4)";

int main(int argc, char** argv) {

//    SerialPort arduino(port);
//    if(arduino.isConnected()){
//        std::cout << "Connection made" << std::endl << std::endl;
//    }
//
//    while (arduino.isConnected()) {
//        char buffer[MAX_DATA_LENGTH];
//        arduino.readSerialPort(buffer, MAX_DATA_LENGTH);
//        std::cout << buffer << std::endl;
//    }

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return QApplication::exec();
}
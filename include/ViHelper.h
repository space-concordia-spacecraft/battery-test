#pragma once

#include <string>
#include "visa.h"
#include <iostream>

class ViHelper {
public:

    void startLoad();
    void stopLoad();

    void init();
    void close();

    void printParameter(char * param);

private:
    ViSession m_Vi, m_DefaultRM;
};
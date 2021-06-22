#pragma once

#include <string>
#include "visa.h"
#include <iostream>

class ViHelper {
public:

    void startLoad() const;
    void stopLoad() const;

    void init();
    void close();

    void printParameter(const char* param, std::ostream& out = std::cout) const;

    void printCapactity(std::ostream& out = std::cout) const;

private:
    ViSession m_Vi, m_DefaultRM;
};
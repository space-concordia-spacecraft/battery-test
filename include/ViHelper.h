#pragma once

#include <string>
#include "visa.h"
#include <iostream>

class ViHelper {
public:

    void StartLoad() const;
    void StopLoad() const;

    void Init();
    void Close();

    void PrintParameter(const char* param, std::ostream& out = std::cout) const;

    void PrintCapacity(std::ostream& out = std::cout) const;

private:
    ViSession m_Vi, m_DefaultRM;
};
#pragma once

#include <string>
#include "visa.h"
#include <iostream>

namespace zeus {
    class ViHelper {
    public:

        void StartLoad() const;
        void StopLoad() const;

        void Init();
        void Close();

        void PrintParameter(const char* param, std::ostream& out = std::cout) const;
        void PrintCapactity(std::ostream& out = std::cout) const;

    private:
        ViSession m_Vi, m_DefaultRM;
    };
}

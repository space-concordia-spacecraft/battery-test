#include "ViHelper.h"

void ViHelper::close() {
    stopLoad();

    viClose(m_Vi);
    viClose(m_DefaultRM);
}

void ViHelper::init() {
    ViChar buffer[VI_FIND_BUFLEN];
    ViRsrc matches = buffer;
    ViUInt32 nmatches;
    ViFindList list;
    viOpenDefaultRM(&m_DefaultRM);

    std::cout << m_DefaultRM << std::endl;

    viFindRsrc(m_DefaultRM, ViString("USB?*"), &list, &nmatches, matches);

    std::cout << list << std::endl;
    std::cout << nmatches << std::endl;
    std::cout << matches << std::endl;

    viOpen(m_DefaultRM, matches, VI_NULL, VI_NULL, &m_Vi);

    std::cout << m_Vi << std::endl;

    viPrintf(m_Vi, ViString(":SOUR:SENS 1\n"));
    printParameter(":SOUR:SENS?\n");

    viPrintf(m_Vi, ViString(":SOUR:FUNC:MODE BATT\n"));
    printParameter(":SOUR:FUNC:MODE?\n");

    viPrintf(m_Vi, ViString(":SOUR:BATT:LEV:IMM 1.6\n"));
    printParameter(":SOUR:BATT:LEV:IMM?\n");

    viPrintf(m_Vi, ViString(":SOUR:BATT:RANG 4\n"));
    printParameter(":SOUR:BATT:RANG?\n");

    viPrintf(m_Vi, ViString(":SOUR:BATT:VON 3\n"));
    printParameter(":SOUR:BATT:VON?\n");

    viPrintf(m_Vi, ViString(":SOUR:BATT:VEN 1\n"));
    printParameter(":SOUR:BATT:VEN?\n");

    viPrintf(m_Vi, ViString(":SOUR:BATT:VST 3\n"));
    printParameter(":SOUR:BATT:VST?\n");

    viPrintf(m_Vi, ViString(":SOUR:BATT:CEN 0\n"));
    printParameter(":SOUR:BATT:CEN?\n");

    viPrintf(m_Vi, ViString(":SOUR:BATT:TEN 1\n"));
    printParameter(":SOUR:BATT:TEN?\n");

    viPrintf(m_Vi, ViString(":SOUR:BATT:TIM 10800\n"));
    printParameter(":SOUR:BATT:TIM?\n");
}

void ViHelper::printParameter(const char* param) {
    char buf[256] = { 0 };
    viPrintf(m_Vi, ViString(param));
    viScanf(m_Vi, ViString("%t\n"), &buf);
    std::cout << buf << std::endl;
}

void ViHelper::startLoad() {
    viPrintf(m_Vi, ViString(":SOUR:INP:STAT 1\n"));
    printParameter(":SOUR:INP:STAT?\n");
}

void ViHelper::stopLoad() {
    viPrintf(m_Vi, ViString(":SOUR:INP:STAT 0\n"));
    printParameter(":SOUR:INP:STAT?\n");
}
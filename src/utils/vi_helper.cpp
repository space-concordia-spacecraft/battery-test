#include "utils/vi_helper.h"

namespace zeus {
    void ViHelper::Close() {
        StopLoad();

        viClose(m_Vi);
        viClose(m_DefaultRM);
    }

    void ViHelper::Init() {
        ViChar buffer[VI_FIND_BUFLEN];
        ViRsrc matches = buffer;
        ViUInt32 nmatches;
        ViFindList list;
        viOpenDefaultRM(&m_DefaultRM);
        viFindRsrc(m_DefaultRM, ViString("USB?*"), &list, &nmatches, matches);
        viOpen(m_DefaultRM, matches, VI_NULL, VI_NULL, &m_Vi);

        viPrintf(m_Vi, ViString(":SOUR:SENS 1\n"));
        PrintParameter(":SOUR:SENS?\n");

        viPrintf(m_Vi, ViString(":SOUR:FUNC:MODE BATT\n"));
        PrintParameter(":SOUR:FUNC:MODE?\n");

        viPrintf(m_Vi, ViString(":SOUR:BATT:LEV:IMM 1.6\n"));
        PrintParameter(":SOUR:BATT:LEV:IMM?\n");

        viPrintf(m_Vi, ViString(":SOUR:BATT:RANG 4\n"));
        PrintParameter(":SOUR:BATT:RANG?\n");

        viPrintf(m_Vi, ViString(":SOUR:BATT:VON 3\n"));
        PrintParameter(":SOUR:BATT:VON?\n");

        viPrintf(m_Vi, ViString(":SOUR:BATT:VEN 1\n"));
        PrintParameter(":SOUR:BATT:VEN?\n");

        viPrintf(m_Vi, ViString(":SOUR:BATT:VST 3\n"));
        PrintParameter(":SOUR:BATT:VST?\n");

        viPrintf(m_Vi, ViString(":SOUR:BATT:CEN 0\n"));
        PrintParameter(":SOUR:BATT:CEN?\n");

        viPrintf(m_Vi, ViString(":SOUR:BATT:TEN 1\n"));
        PrintParameter(":SOUR:BATT:TEN?\n");

        viPrintf(m_Vi, ViString(":SOUR:BATT:TIM 10800\n"));
        PrintParameter(":SOUR:BATT:TIM?\n");
    }

    void ViHelper::PrintParameter(const char* param, std::ostream& out) const {
        char buf[256] = { 0 };
        viPrintf(m_Vi, ViString(param));
        viScanf(m_Vi, ViString("%t"), &buf);
        out << buf << std::endl;
    }

    void ViHelper::StartLoad() const {
        int attempts = 0;
        bool loadStarted = false;
        static char buf[8] = {0};

        do {
            viPrintf(m_Vi, ViString(":SOUR:INP:STAT 1\n"));
            // TODO: SLEEP
            viPrintf(m_Vi, ViString(":SOUR:INP:STAT?\n"));
            viScanf(m_Vi, ViString("%t"), &buf);
            loadStarted = buf[0];
            attempts ++;
        } while(attempts <= 3 && !loadStarted);

        if(attempts >= 3 && !loadStarted)
            std::cout << "LOAD COULDN'T START" << std::endl;

        PrintParameter(":SOUR:INP:STAT?\n");
    }

    void ViHelper::StopLoad() const {
        viPrintf(m_Vi, ViString(":SOUR:INP:STAT 0\n"));
        PrintParameter(":SOUR:INP:STAT?\n");
    }

    void ViHelper::PrintCapacity(std::ostream& out) const {
        PrintParameter(":FETC:CAP?\n", out);
    }
}
/* 
* SystemStats.h
*
* Created: 11.05.2016 22:04:58
* Author: Kurt
*/


#ifndef __SYSTEMSTATS_H__
#define __SYSTEMSTATS_H__

#include <stdio.h>

class SystemStats
{
public:
    uint16_t m_DSPCyclesMin;
    uint16_t m_DSPCyclesMax;
    uint16_t m_DSPCyclesPrev;
    int32_t  m_CyclesPerFrameAvg;
    int32_t  m_CyclesPerFrameMax;
    uint32_t m_Uptime;
};

extern SystemStats g_SystemStats;

#endif //__SYSTEMSTATS_H__

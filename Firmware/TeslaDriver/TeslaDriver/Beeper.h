// This file is part of TeslaDriver.
//
// Copyright (C) 2016 Kurt Skauen <http://kavionic.com/>
//
// TeslaDriver is free software : you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// TeslaDriver is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TeslaDriver. If not, see < http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////////////


#ifndef __BEEPER_H__
#define __BEEPER_H__


#include "Hardware.h"
#include "Config.h"

#include "Misc/DigitalPort.h"

namespace SoundCategory
{
    enum Enum
    {
        e_Tactile,
        e_Progress,
        e_Alarm
    };
}

class Beeper
{
public:
    static const uint8_t TIME_PER_TICK = 5; // Number of milliseconds between each beep-duration step.
    static void     Initialize();
    static inline void Run();
    static void     Mute(SoundCategory::Enum category, bool mute);
    static bool     IsMuted(SoundCategory::Enum category);

    static void     SetMuteFlags(uint8_t flags);
    static uint8_t  GetMuteFlags();

    static void     Beep(BeepID::Enum beepID);
    static void     BeepTimed(uint8_t duration);
    static void     Alarm();
    static void     SetBuzzerVolume(uint8_t volume);
    static uint8_t  GetBuzzerVolume();
    static void     SetBuzzerFrequency(uint16_t frequency);
    static uint16_t GetBuzzerFrequency();


private:
    static inline void Enable(bool enable)
    {
        if (enable) {
            BUZZER_TIMER.CTRLB = TC1_CCAEN_bm | TC1_CCBEN_bm | TC_WGMODE_DSBOTH_gc;
            BUZZER_TIMER.CNT   = 0;
            BUZZER_TIMER.CTRLA = TC_CLKSEL_DIV1_gc;
        } else {
            BUZZER_TIMER.CTRLA = 0;
            BUZZER_TIMER.CTRLB = 0;
        }        
    }
    static uint8_t s_MuteFlags;
    static uint8_t s_TimerSkipCount;
    static uint8_t g_BeepEndTime;
    static uint8_t g_AlarmCycleCount;
    static uint8_t g_AlarmCycle;
    static uint8_t s_BuzzerFrequency;
};

void Beeper::Run()
{
    
    if ( --s_TimerSkipCount ) {
        return;
    } else {
        s_TimerSkipCount = TIME_PER_TICK;
    }

    if ( g_AlarmCycleCount )
    {
        if ( !(--g_BeepEndTime) )
        {
            --g_AlarmCycleCount;
            g_BeepEndTime = Config::GetAlarmCycleLength(g_AlarmCycle);
            if ( g_BeepEndTime == 0 ) {
                g_AlarmCycle = 1;
                g_BeepEndTime = Config::GetAlarmCycleLength(0);
            } else {
                ++g_AlarmCycle;
            }
//            g_AlarmCycle = (g_AlarmCycle + 1) & uint8_t(~(ARRAY_COUNT(g_EEPROM.sound.m_AlarmCycle) - 1));
            if ( g_AlarmCycleCount & 0x01 )
            {
                Enable(true);
            }
            else
            {
                Enable(false);
            }
        }
        
    }
    else if ( g_BeepEndTime )
    {
        if ( !(--g_BeepEndTime) )
        {
            Enable(false);
        }
    }    
}

#endif //__BEEPER_H__

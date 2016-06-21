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


#ifndef __DS18B20_H__
#define __DS18B20_H__

#include "Misc/Utils.h"

class DS18B20
{
public:
    void Initialize();
    static void Run();

    static const uint8_t TEMP_FRACTION_BITS = 4;
    static int16_t GetTemp1();
    static int16_t GetTemp2();
    
    static uint8_t GetReadingIndex() { return s_ReadingIndex; }

    static void    SetResolution(uint8_t resolution); // Set resolution in number of bits between 9 and 12 inclusive.
    static uint8_t GetResolution();
    
    enum LogicState_e
    {
        e_WriteConfig,
        e_StartConversion,
        e_WaitForConversion,
        e_RequestScratchpad,
        e_ReadScratchpad
    };
    
    enum State_e
    {
        e_Idle,
        e_Reset,
        e_ReadPresence,
        e_PresenceDone,
        e_StartReadSlot,
        e_Reading,
        e_Writing
    };

    static const uint8_t CFG_RES_UNUSED_BITS = 0x1f;
    static const uint8_t CFG_RES_POS         = 5;
    
    static const uint8_t CFG_RES_9BIT_gc  = 0 << CFG_RES_POS;
    static const uint8_t CFG_RES_10BIT_gc = 1 << CFG_RES_POS;
    static const uint8_t CFG_RES_11BIT_gc = 2 << CFG_RES_POS;
    static const uint8_t CFG_RES_12BIT_gc = 3 << CFG_RES_POS;
    
    static const uint8_t CFG_RES_MASK        = 3 << CFG_RES_POS;

    static const uint8_t CMD_SEARCH_ROM       = 0xf0;
    static const uint8_t CMD_READ_ROM         = 0x33;
    static const uint8_t CMD_MATCH_ROM        = 0x55;
    static const uint8_t CMD_SKIP_ROM         = 0xcc;
    static const uint8_t CMD_ALARM_SEARCH     = 0xec;
    static const uint8_t CMD_CONVERT_TEMP     = 0x44;
    static const uint8_t CMD_WRITE_SCRATCHPAD = 0x4e;
    static const uint8_t CMD_READ_SCRATCHPAD  = 0xbe;
    static const uint8_t CMD_COPY_SCRATCHPAD  = 0x48;
    static const uint8_t CMD_RECALL_EEPROM    = 0xb8;
    static const uint8_t CMD_READ_POWERSUPPLY = 0xb4;
        
    static volatile int16_t s_Temperature1;
    static volatile int16_t s_Temperature2;
    static volatile uint8_t s_ReadingIndex;
    
};

#endif //__DS18B20_H__

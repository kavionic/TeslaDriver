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


#ifndef __FANCONTROLLER_H__
#define __FANCONTROLLER_H__

#include "DS18B20.h"

struct Event;

class FanController
{
public:
    enum FanID_e { e_Fan1, e_Fan2, e_FanCount};
    void Initialize();
    void Run(const Event& event);
    void SetRPM(int8_t fanID, uint16_t speed);
    uint32_t GetRPM(int8_t fanID);
private:
    static const uint16_t TEMP_MIN = 30 << DS18B20::TEMP_FRACTION_BITS;
    static const uint16_t TEMP_MAX = 55 << DS18B20::TEMP_FRACTION_BITS;
    static const uint16_t TEMP_ERROR_LO = 55 << DS18B20::TEMP_FRACTION_BITS;
    static const uint16_t TEMP_ERROR_HI = 60 << DS18B20::TEMP_FRACTION_BITS;
    
    int16_t m_TargetFanSpeed[e_FanCount];
    int16_t m_CurrentFanSpeed[e_FanCount];
    uint32_t m_PrevUpdateTime;
    int16_t  m_PrevError[e_FanCount];
    int32_t  m_IntegralError[e_FanCount];
};

extern FanController g_FanController;

#endif //__FANCONTROLLER_H__

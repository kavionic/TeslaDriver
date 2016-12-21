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


#ifndef __ADCCONTROLLER_H__
#define __ADCCONTROLLER_H__


class ADCController
{
public:
    static ADCController Instance;
    void Initialize();
    void Run();
    
    void SetCurrentLimits(uint16_t limitLow, uint16_t limitHigh);
    void GetCurrentLimits(uint16_t* limitLow, uint16_t* limitHigh) const;

    int16_t GetVoltage3();
    int16_t GetVoltage5();
    int16_t GetVoltage12();
    int16_t GetVoltage48();
    
    int16_t GetCurrentSenseRefVoltage();
    
    int16_t GetCurrentLow();
    int16_t GetCurrentHigh();

private:
    void UpdateCurrentSenseReferenceVoltage();
    
    int16_t m_CurrentSenseOffset = 0;
};

#endif //__ADCCONTROLLER_H__

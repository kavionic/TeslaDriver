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
    
    void SampleCurrent();
    
    void SetCurrentLimits(uint16_t limitLow, uint16_t limitHigh);
    void GetCurrentLimits(uint16_t* limitLow, uint16_t* limitHigh) const;

    uint16_t GetVoltage3();
    uint16_t GetVoltage5();
    uint16_t GetVoltage12();
    uint16_t GetVoltage48();
    
    int16_t GetCurrentSenseRefVoltage();
    
    int16_t GetCurrentLow();
    int16_t GetCurrentHigh();

private:
    void UpdateCurrentSenseReferenceVoltage();
    
    uint8_t BinarySearch(int16_t a[], uint8_t length, int16_t key);
    void    InsertSample(int16_t* buffer, uint8_t len, int16_t value);

    static const uint8_t CURRENT_MEASURE_COUNT = 9;
    int16_t m_CurrentSenseOffset = 0;
    int16_t m_CurrentMeasurementsLow[CURRENT_MEASURE_COUNT];
    int16_t m_CurrentMeasurementsHigh[CURRENT_MEASURE_COUNT];
    int8_t m_CurrentMeasurementsCount = 0;
    int16_t m_CurrentMeasureLow;
    int16_t m_CurrentMeasureHigh;
};

#endif //__ADCCONTROLLER_H__

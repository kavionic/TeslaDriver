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


#ifndef __PWMCONTROLLER_H__
#define __PWMCONTROLLER_H__

extern "C" void DMA_CH0_vect(void); // __attribute__ ((signal,__INTR_ATTRS));
extern "C" void DMA_CH1_vect(void); // __attribute__ ((signal,__INTR_ATTRS));

struct PWMModulationPair;

class PWMController
{
public:
    static const uint8_t MODULATION_DMA_BUFFER_SIZE = 32;

    void Initialize();
    void Run();
    
    void SetPeriod(uint16_t period);
    uint16_t GetPeriod() const;
    
    void     SetDutyCycle(uint16_t dutyCycle);
    uint16_t GetDutyCycle() const;
    
    void SetDeadTime(uint8_t deadTimeLS, uint8_t deadTimeHS);
    void GetDeadTime(uint8_t* deadTimeLS, uint8_t* deadTimeHS) const;
    
    int16_t  GetAvaialbleModulationDataSpace() const;
    int16_t  WriteModulationData(const uint8_t* data, int16_t size);
    void     SetModulationSampleRate(uint16_t sampleRate);
    uint16_t GetModulationSampleRate() const;
    void     SetModulationScale(uint8_t multiplier, uint8_t rightShifter);
    void     GetModulationScale(uint8_t& multiplier, uint8_t& rightShifter) const;
    
private:
    friend void DMA_CH2_vect(void);
    friend void DMA_CH3_vect(void);
    
    void SetupModulationDMAChannel(DMA_CH_t& channel, volatile PWMModulationPair* srcAddr, volatile uint16_t* pwmRegister);

//    void ScaleModulationData(uint16_t* dstLS, uint16_t* dstHS, const uint8_t* data, int16_t size);
    
    uint16_t m_ModulationSampleRate = 22050;
    
};

extern PWMController g_PWMController;

#endif //__PWMCONTROLLER_H__

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
    enum FaultFlags_e
    {
        e_FaultActive          = 0x01, // Flag set if any of the other flag is set, but must be explicitly cleared.
        e_FaultOvertemp        = 0x02,
        e_FaultTempSensorError = 0x04,
        e_FaultOverCurrent     = 0x08,
        e_FaultUnderVoltage    = 0x10,
    };
    static const uint8_t MODULATION_DMA_BUFFER_SIZE = 32;

    void Initialize();
    void Run();
    
    void SetPeriod(uint16_t period);
    uint16_t GetPeriod() const;
    
    void     SetDutyCycle(uint16_t dutyCycle);
    uint16_t GetDutyCycle() const;
    
    bool IsFaulty() const { return m_FaultFlags != 0; }
    void SetFaultFlags(uint8_t faults);
    void ClearFaultFlags(uint8_t faults);
    bool HasFault(uint8_t flags) { return (m_FaultFlags & flags) != 0; }
    
    void SetDeadTime(uint8_t deadTimeLS, uint8_t deadTimeHS);
    void GetDeadTime(uint8_t* deadTimeLS, uint8_t* deadTimeHS) const;
    
    int16_t  GetAvaialbleModulationDataSpace() const;
    int16_t  WriteModulationData(const uint8_t* data, int16_t size);
    void     SetModulationSampleRate(uint16_t sampleRate);
    uint16_t GetModulationSampleRate() const;
    void     SetModulationScale(uint8_t multiplier, uint8_t rightShifter);
    void     GetModulationScale(uint8_t& multiplier, uint8_t& rightShifter) const;
    static bool IsModulating();
private:
    friend void DMA_CH2_vect(void);
    friend void DMA_CH3_vect(void);
    
    void SetupModulationDMAChannel(DMA_CH_t& channel, volatile PWMModulationPair* srcAddr, volatile uint16_t* pwmRegister);

//    void ScaleModulationData(uint16_t* dstLS, uint16_t* dstHS, const uint8_t* data, int16_t size);
    
    
    uint8_t m_FaultFlags = 0;
    
    uint16_t m_ModulationSampleRate = 22050;
    
};

extern PWMController g_PWMController;

#endif //__PWMCONTROLLER_H__

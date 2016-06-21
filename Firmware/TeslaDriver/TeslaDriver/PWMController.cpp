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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <stdio.h>

#include "PWMController.h"
#include "Hardware.h"
#include "SystemStats.h"

#include "Misc/DigitalPort.h"
#include "Misc/Utils.h"
#include "Misc/Clock.h"

static const int16_t PWM_MODULATION_BUFFER_SIZE = 8192;

static uint8_t g_ModulationBuffer[PWM_MODULATION_BUFFER_SIZE];
static volatile int16_t g_ModulationBufferBytesAvailable;
static volatile int16_t g_ModulationBufferInPos;
static volatile int16_t g_ModulationBufferOutPos;

struct PWMModulationPair
{
    uint16_t m_Low;
    uint16_t m_High;
};

static PWMModulationPair g_ModulationDMABuffer[2][PWMController::MODULATION_DMA_BUFFER_SIZE];
static volatile bool g_ValidDMABuffers[2];

static uint8_t  g_ModulationMultiplier = 157;
static uint8_t  g_ModulationShifter = 7;
static uint16_t g_DutyCycle;
static uint16_t g_PWMThreshold;
static int16_t g_PWMModulationCenter;

#define DMA_CHANNEL_CTRL (DMA_CH_REPEAT_bm | DMA_CH_SINGLE_bm | DMA_CH_BURSTLEN_4BYTE_gc)
///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

static inline void ScaleModulationData(PWMModulationPair* dst, const uint8_t* data, int16_t size)
{
    int16_t maxVal = PWM_TIMER.PERBUF>>1;
    for (int16_t i = 0 ; i < size ; ++i)
    {
        int16_t sample = (uint16_t(data[i] * g_ModulationMultiplier) >> g_ModulationShifter) + g_PWMModulationCenter;
        if (sample < 0 ) {
            sample = 0;
        } else if ( sample > maxVal ) {
             sample = maxVal;
        }
        dst[i].m_Low = sample;
        dst[i].m_High = PWM_TIMER.PERBUF - sample;
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

static inline void FillDMABuffers(uint8_t channel)
{
    int16_t bytesWritten = 0;
    while(g_ModulationBufferBytesAvailable && bytesWritten < PWMController::MODULATION_DMA_BUFFER_SIZE)
    {
        int16_t sizeToEnd = min(PWM_MODULATION_BUFFER_SIZE - g_ModulationBufferOutPos, g_ModulationBufferBytesAvailable);
        if ( sizeToEnd > PWMController::MODULATION_DMA_BUFFER_SIZE - bytesWritten) sizeToEnd = PWMController::MODULATION_DMA_BUFFER_SIZE - bytesWritten;
        sei();
        ScaleModulationData(g_ModulationDMABuffer[channel] + bytesWritten, g_ModulationBuffer + g_ModulationBufferOutPos, sizeToEnd);
        cli();
        g_ModulationBufferOutPos += sizeToEnd;
        g_ModulationBufferOutPos &= PWM_MODULATION_BUFFER_SIZE - 1;
        g_ModulationBufferBytesAvailable -= sizeToEnd;
        bytesWritten += sizeToEnd;
    }    

    uint16_t transferCount;
    if (bytesWritten != 0)
    {
        g_ValidDMABuffers[channel] = true;
        transferCount = bytesWritten<<2;
    }
    else
    {
        g_ModulationDMABuffer[channel][0].m_Low = g_PWMThreshold;
        g_ModulationDMABuffer[channel][0].m_High = PWM_TIMER.PERBUF - g_PWMThreshold;
        g_ValidDMABuffers[channel] = false;
        transferCount = 4;
    }
    if (channel == 0) {
        DMA.CH0.TRFCNT = transferCount;
    } else {
        DMA.CH1.TRFCNT = transferCount;
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

static inline void HandleDMAInterrupt(uint8_t channel)
{
    FillDMABuffers(channel);
    if (!g_ValidDMABuffers[0] && !g_ValidDMABuffers[1]) // Both DMA buffers empty. Stop the DMA timer and reset the PWM duty cycle.
    {
        PWM_MODULATION_TIMER.CTRLA = TC_CLKSEL_OFF_gc;
        PWM_TIMER.PWM_CMP_LO_PWM = g_PWMThreshold;
        PWM_TIMER.PWM_CMP_HI_PWM = PWM_TIMER.PER - g_PWMThreshold;

        g_SystemStats.m_DSPCyclesMax  = 0;
        g_SystemStats.m_DSPCyclesMin  = 0;
        g_SystemStats.m_DSPCyclesPrev = 0;

        printf_P(PSTR("S%d\n"), channel);
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

ISR(DMA_CH0_vect)
{
    if ( DMA.INTFLAGS & DMA_CH0TRNIF_bm )
    {
        uint16_t cycles = SYSTEM_TIMER.CNT;
        DMA.INTFLAGS = DMA_CH0TRNIF_bm;
        HandleDMAInterrupt(0);
        cycles = SYSTEM_TIMER.CNT - cycles;
        if (cycles > g_SystemStats.m_DSPCyclesMax) g_SystemStats.m_DSPCyclesMax = cycles;
        if (cycles < g_SystemStats.m_DSPCyclesMin) g_SystemStats.m_DSPCyclesMin = cycles;
        g_SystemStats.m_DSPCyclesPrev = cycles;
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

ISR(DMA_CH1_vect)
{
    if ( DMA.INTFLAGS & DMA_CH1TRNIF_bm )
    {
        uint16_t cycles = SYSTEM_TIMER.CNT;
        DMA.INTFLAGS = DMA_CH1TRNIF_bm;
        HandleDMAInterrupt(1);
        cycles = SYSTEM_TIMER.CNT - cycles;
        if (cycles > g_SystemStats.m_DSPCyclesMax) g_SystemStats.m_DSPCyclesMax = cycles;
        if (cycles < g_SystemStats.m_DSPCyclesMin) g_SystemStats.m_DSPCyclesMin = cycles;
        g_SystemStats.m_DSPCyclesPrev = cycles;
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void PWMController::Initialize()
{
    // Setup timer
    PWM_TIMER.CTRLB = TC0_CCAEN_bm | TC0_CCBEN_bm | TC0_CCCEN_bm | TC0_CCDEN_bm | TC_WGMODE_DSBOTH_gc;

    // Setup waveform generator:

    //EVSYS.CH2MUX = EVSYS_CHMUX_PORTD_PIN1_gc;
    //    EVSYS.CH2CTRL = EVSYS_DIGFILT_8SAMPLES_gc;
	
    AWEXC.CTRL = PWM_AWEX_CHANNELS;
    AWEXC.OUTOVEN = PWM_OUT_PINS;
    AWEXC.DTLSBUF = 1;
    AWEXC.DTHSBUF = 1;
    //AWEXC.FDEMASK = PWM_FAULT_EVENTS; // Event 2 = fault
    //AWEXC.FDCTRL = AWEX_FDACT_CLEAROE_gc | AWEX_FDMODE_bm;
	
    HIRESC.CTRLA = HIRES_HREN0_bm | BIT(2,1);
	
        
    ADCA.CTRLA = ADC_ENABLE_bm;
    ADCA.CTRLB = ADC_CONMODE_bm;
    ADCA.REFCTRL = ADC_REFSEL_INT1V_gc | ADC_BANDGAP_bm | ADC_TEMPREF_bm;
    ADCA.EVCTRL  = ADC_EVSEL_4567_gc | ADC_EVACT_CH01_gc;
    ADCA.PRESCALER = ADC_PRESCALER_DIV16_gc;
    
    ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
    ADCA.CH0.MUXCTRL = /*ADC_CH_MUXNEG_GND_MODE3_gc |*/ ADC_CH_MUXPOS_PIN5_gc;

    ADCA.CH1.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
    ADCA.CH1.MUXCTRL = /*ADC_CH_MUXNEG_GND_MODE3_gc |*/ ADC_CH_MUXPOS_PIN5_gc;
        
    EVSYS.CH4CTRL = EVSYS_DIGFILT_1SAMPLE_gc;
    EVSYS.CH4MUX  = PWM_SAMPLE_LO_TRIG;
    
    EVSYS.CH5CTRL = EVSYS_DIGFILT_1SAMPLE_gc;
    EVSYS.CH5MUX  = PWM_SAMPLE_HI_TRIG;
    
    SetPeriod(CPU_FREQ * 4 / 103000);
    SetDutyCycle(32768);
    PWM_TIMER.PER = PWM_TIMER.PERBUF;
    PWM_TIMER.CCA = PWM_TIMER.CCABUF;
    PWM_TIMER.CCB = PWM_TIMER.CCBBUF;
    PWM_TIMER.CCC = PWM_TIMER.CCCBUF;
    PWM_TIMER.CCD = PWM_TIMER.CCDBUF;
            
    // Setup pins:
    PORTD.PIN1CTRL = PORT_ISC_LEVEL_gc | PORT_OPC_PULLUP_gc | PORT_INVEN_bm;
    //    AWEXC.STATUS |= AWEX_FDF_bm;
    DigitalPort::SetAsOutput(PWM_OUT_PORT, PWM_OUT_PINS);
    PWM_TIMER.CTRLA = TC_CLKSEL_DIV1_gc;
    
    DMA.CTRL = DMA_ENABLE_bm | DMA_DBUFMODE_CH01_gc | DMA_PRIMODE_RR0123_gc;
    SetupModulationDMAChannel(DMA.CH0, g_ModulationDMABuffer[0], &PWM_TIMER.PWM_CMP_LO_PWM);
    SetupModulationDMAChannel(DMA.CH1, g_ModulationDMABuffer[1], &PWM_TIMER.PWM_CMP_LO_PWM);

    DMA.CH0.CTRLB = DMA_CH_TRNINTLVL_MED_gc;
    DMA.CH1.CTRLB = DMA_CH_TRNINTLVL_MED_gc;
    
    PWM_MODULATION_EVCH_MUX = PWM_MODULATION_EVENT_TRIGGER_SRC;
    PWM_MODULATION_EVCH_CTRL = EVSYS_DIGFILT_1SAMPLE_gc;
    
    PWM_MODULATION_TIMER.PERBUF = CPU_FREQ / m_ModulationSampleRate;
        
/*    for ( int16_t i = 0 ; i < 512 ; ++i ) {
        m_ModulationBufferLS[0][i] = i>>1;
        m_ModulationBufferHS[0][i] = 800 - m_ModulationBufferHS[0][i];
        m_ModulationBufferLS[1][i] = (512-i)>>1;
        m_ModulationBufferHS[1][i] = 800 - m_ModulationBufferHS[0][i];
    }*/
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void PWMController::Run()
{
    static int32_t prevUpdateTime = 0;
    static bool state = false;
    uint32_t time = Clock::GetTime();
    
    if ((time - prevUpdateTime) > 1)
    {
        if (state)
        {
            SetDutyCycle(0);
            state = false;
        }
        else
        {
            SetDutyCycle(65535);
            state = true;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void PWMController::SetupModulationDMAChannel(DMA_CH_t& channel, volatile PWMModulationPair* srcAddr, volatile uint16* pwmRegister)
{
    channel.CTRLA = DMA_CH_REPEAT_bm | DMA_CH_SINGLE_bm | DMA_CH_BURSTLEN_4BYTE_gc;
    channel.TRFCNT = 4; // MODULATION_BUFFER_SIZE * 2;
    channel.ADDRCTRL = DMA_CH_SRCDIR_INC_gc | DMA_CH_SRCRELOAD_TRANSACTION_gc | DMA_CH_DESTDIR_INC_gc | DMA_CH_DESTRELOAD_BURST_gc;
    channel.SRCADDR0 = ((intptr_t)srcAddr) & 0xff;
    channel.SRCADDR1 = (((intptr_t)srcAddr) >> 8) & 0xff;
    //	channel.SRCADDR2 = (((intptr_t)pwmVariable) >> 16) & 0xff;
    channel.DESTADDR0 = ((intptr_t)pwmRegister) & 0xff;
    channel.DESTADDR1 = (((intptr_t)pwmRegister) >> 8) & 0xff;
    //	channel.DESTADDR2 = (((intptr_t)pwmRegister) >> 16) & 0xff;
    channel.TRIGSRC = PWM_MODULATION_DMA_TRIGGER;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void PWMController::SetPeriod(uint16_t period)
{
    g_PWMThreshold = (U32(g_DutyCycle) * period + 65536) >> 17; // / (65535*2);
    g_PWMModulationCenter = g_PWMThreshold - ((127 * g_ModulationMultiplier) >> g_ModulationShifter);
    
    PWM_TIMER.PERBUF            = period;
    PWM_TIMER.PWM_CMP_HI_SAMPLE = period;    // We use CCA to detect underflow and CCB to detect overflow for current measurements.
    
    PWM_TIMER.PWM_CMP_LO_PWM = g_PWMThreshold;
    PWM_TIMER.PWM_CMP_HI_PWM = PWM_TIMER.PER - g_PWMThreshold;
    
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

uint16_t PWMController::GetPeriod() const
{
    return PWM_TIMER.PERBUF;
}    

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////
    
void PWMController::SetDutyCycle(uint16_t dutyCycle)
{
    g_DutyCycle = dutyCycle;
    
//    uint16_t pwm = U32(dutyCycle) * PWM_TIMER.PERBUF / (65535*2);
    g_PWMThreshold = (U32(dutyCycle) * PWM_TIMER.PERBUF + 65536) >> 17; // / (65535*2);
    g_PWMModulationCenter = g_PWMThreshold - ((127 * g_ModulationMultiplier) >> g_ModulationShifter);
    
//    printf_P(PSTR("Set duty cycle: %u / %u / %u\n"), dutyCycle, pwm, PWM_TIMER.PERBUF);
    PWM_TIMER.PWM_CMP_LO_PWM = g_PWMThreshold;
    PWM_TIMER.PWM_CMP_HI_PWM = PWM_TIMER.PERBUF - g_PWMThreshold;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

uint16_t PWMController::GetDutyCycle() const
{
    return g_DutyCycle;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void PWMController::SetDeadTime(uint8_t deadTimeLS, uint8_t deadTimeHS)
{
    AWEXC.DTLSBUF = deadTimeLS;
    AWEXC.DTHSBUF = deadTimeHS;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void PWMController::GetDeadTime(uint8_t* deadTimeLS, uint8_t* deadTimeHS) const
{
    *deadTimeLS = AWEXC.DTLSBUF;
    *deadTimeHS = AWEXC.DTHSBUF;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

int16_t PWMController::GetAvaialbleModulationDataSpace() const
{
    cli();
    int16_t sizeAvail = g_ModulationBufferBytesAvailable;
    sei();
    return PWM_MODULATION_BUFFER_SIZE - sizeAvail;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

int16_t PWMController::WriteModulationData(const uint8_t* data, int16_t size)
{
    cli();
    int16_t availableSize = g_ModulationBufferBytesAvailable;
    sei();
    availableSize = PWM_MODULATION_BUFFER_SIZE - availableSize;
    if (size > availableSize) {
        printf_P(PSTR("ERROR: Modulation buffer overflow! %d > %d\n"), size, availableSize);
        size = availableSize;
    }
    //printf_P(PSTR("WriteModulationData: %d/%d\n"), size, origSize);
    for(int16_t i = 0 ; i < size ; ++i)
    {
        g_ModulationBuffer[g_ModulationBufferInPos++] = *(data++);
        g_ModulationBufferInPos &= PWM_MODULATION_BUFFER_SIZE - 1;
    }
    cli();
    g_ModulationBufferBytesAvailable += size;
    bool restart = g_ModulationBufferBytesAvailable && PWM_MODULATION_TIMER.CTRLA == 0;
    sei();
    if (restart)
    {
        FillDMABuffers(0);
        FillDMABuffers(1);
        DMA.CH0.CTRLA = DMA_CH_ENABLE_bm | DMA_CHANNEL_CTRL;
        PWM_MODULATION_TIMER.CTRLA = TC_CLKSEL_DIV1_gc;
    }
    return size;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////
    
void PWMController::SetModulationSampleRate(uint16_t sampleRate)
{
    m_ModulationSampleRate = sampleRate;    
    PWM_MODULATION_TIMER.PERBUF = CPU_FREQ / m_ModulationSampleRate;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

uint16_t PWMController::GetModulationSampleRate() const
{
    return m_ModulationSampleRate;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void PWMController::SetModulationScale(uint8_t multiplier, uint8_t rightShifter)
{
    uint16_t center = g_PWMThreshold - ((127 * multiplier) >> rightShifter);
    cli();
    g_ModulationMultiplier = multiplier;
    g_ModulationShifter = rightShifter;
    g_PWMModulationCenter = center;
    sei();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void PWMController::GetModulationScale( uint8_t& multiplier, uint8_t& rightShifter ) const
{
    multiplier   = g_ModulationMultiplier;
    rightShifter = g_ModulationShifter;    
}

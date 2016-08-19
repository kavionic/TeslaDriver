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


#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "FanController.h"
#include "PWMController.h"
#include "Hardware.h"
#include "EventSys.h"

#include "Misc/DigitalPort.h"
#include "Misc/Clock.h"

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////


static volatile uint16_t g_PrevFan1Time = 0;
static volatile uint16_t g_PrevFan1Capture = 0;
static volatile uint32_t g_PrevFan1Periode = 0;

static volatile uint16_t g_PrevFan2Time = 0;
static volatile uint32_t g_PrevFan2Capture = 0;
static volatile uint32_t g_PrevFan2Periode = 0;

ISR(TCE0_CCA_vect)
{
    uint16_t capture = FAN_RPM_TIMER.CCA;
    uint16_t time = Clock::GetUpdateCycle();
    sei();
    uint16_t numUpdates = time - g_PrevFan1Time;
    g_PrevFan1Time = time;

    uint32_t periode = U32(numUpdates) * CLOCK_CPUCYCLES_PER_UPDATE + capture - g_PrevFan1Capture;
    g_PrevFan1Capture = capture;
    
//    g_PrevFan1Periode = periode;
    g_PrevFan1Periode += int32_t(periode - g_PrevFan1Periode) / 2;
}

ISR(TCE0_CCB_vect)
{
    uint16_t capture = FAN_RPM_TIMER.CCB;
    uint16_t time = Clock::GetUpdateCycle();
    sei();
    uint16_t numUpdates = time - g_PrevFan2Time;
    g_PrevFan2Time = time;

    uint32_t periode = U32(numUpdates) * CLOCK_CPUCYCLES_PER_UPDATE + capture - g_PrevFan2Capture;
    g_PrevFan2Capture = capture;
    
    //    g_PrevFan2Periode = periode;
    g_PrevFan2Periode += int32_t(periode - g_PrevFan2Periode) / 2;
}

void FanController::Initialize()
{
    // WARNING: RPM Timer shared with system clock!
    
    FAN_RPM_TIMER.CTRLB = TC0_CCAEN_bm | TC0_CCBEN_bm;
    FAN_RPM_TIMER.CTRLD = TC_EVACT_CAPT_gc | TC_EVSEL_CH6_gc;
 
    FAN_RPM_PORT->PIN0CTRL = PORT_ISC_RISING_gc;
    FAN_RPM_PORT->PIN1CTRL = PORT_ISC_RISING_gc;
    
    EVSYS.CH6MUX = EVSYS_CHMUX_PORTE_PIN0_gc;
    EVSYS.CH6CTRL = EVSYS_DIGFILT_8SAMPLES_gc;
    
    EVSYS.CH7MUX = EVSYS_CHMUX_PORTE_PIN1_gc;
    EVSYS.CH7CTRL = EVSYS_DIGFILT_8SAMPLES_gc;

    FAN_RPM_TIMER.INTCTRLB = TC_CCAINTLVL_MED_gc | TC_CCBINTLVL_MED_gc;
    
    FAN_PWM_TIMER.CTRLB = TC0_CCAEN_bm | TC0_CCBEN_bm | TC_WGMODE_SINGLESLOPE_gc;
    FAN_PWM_TIMER.PERBUF = FAN_PWM_PERIOD;
    FAN_PWM_TIMER.CCABUF = 0;
    FAN_PWM_TIMER.CCBBUF = 0;
    
    FAN_PWM_TIMER.CTRLA = TC_CLKSEL_DIV1_gc;
    
    DigitalPort::InvertPins(FAN_PWM_PORT, FAN_PWM1_PIN | FAN_PWM2_PIN);
    DigitalPort::SetAsOutput(FAN_PWM_PORT, FAN_PWM1_PIN | FAN_PWM2_PIN);
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

class PerfTimer
{
public:
    PerfTimer() {
        m_StartTimer = Clock::GetHighresTimer();
    }
    ~PerfTimer() {
        uint32_t currentTimer = Clock::GetHighresTimer();
        uint32_t lapsed = currentTimer - m_StartTimer;
        uint32_t microSeconds = lapsed * 1000 / (CPU_FREQ / 1000000);
        printf_P(PSTR("Time lapsed: %06lu (%lu.%luuS)\n"), lapsed, microSeconds / 1000, microSeconds % 1000);
    }
    
private:    
    uint32_t m_StartTimer;
};

void FanController::Run(const Event& event)
{
    
    uint8_t updateCycles = Clock::GetUpdateCycle();
    uint8_t numUpdates = updateCycles - g_PrevFan1Time;
    
    if (numUpdates > 40) {
        g_PrevFan1Time = updateCycles;
        g_PrevFan1Capture = 0;
    }        
    
    
    uint32_t time = Clock::GetTime();
    uint8_t deltaTime = time - m_PrevUpdateTime;
    //static const uint16_t deltaTime = 2;
    static int8_t currentFan = 0;
    
    if ( deltaTime > 4)
    {
        uint16_t temp1 = DS18B20::GetTemp1();
        uint16_t temp2 = DS18B20::GetTemp2();
        uint16_t temp = max(temp1, temp2);
        
        uint16_t targetRPM = 4000;
        if (temp < TEMP_MIN) {
            targetRPM = 0;
        } else if (temp > TEMP_MAX) {
            targetRPM = 4000;
        } else {
            targetRPM = 1200 + U32(4000 - 1200) * (temp - TEMP_MIN) / (TEMP_MAX - TEMP_MIN);
        }
        if (g_PWMController.HasFault(PWMController::e_FaultOvertemp)) {
            if (temp < TEMP_ERROR_LO) {
                g_PWMController.ClearFaultFlags(PWMController::e_FaultOvertemp);
            }            
        } else {
            if (temp >= TEMP_ERROR_HI) {
                g_PWMController.SetFaultFlags(PWMController::e_FaultOvertemp);
            }                
        }            
        
//        volatile PerfTimer perfTimer;
        m_PrevUpdateTime = time;
        int8_t i = currentFan & 0x1;
        currentFan++;
//        for (int i = 0 ; i < e_FanCount; ++i)
        {
            const int16_t KP = 2;
            const int16_t KI = 20;
            const int16_t KD = 8;
            int16_t rpm = GetRPM(i);
            int16_t error = targetRPM - rpm;
//            int16_t absError = (error >= 0) ? error : -error;
            m_IntegralError[i] = clamp<int32_t>(-50000, 50000, m_IntegralError[i] + error);
//            int16_t derivative = (absError < 100) ? (error - m_PrevError) : 0;
            int16_t derivative = error - m_PrevError[i];

            int32_t output = clamp<int32_t>(0, FAN_PWM_PERIOD, error * KP + m_IntegralError[i] / KI  + derivative * KD + FAN_PWM_PERIOD / 2);
            m_PrevError[i] = error;
            
/*            printf_P(PSTR("%04d, %04d, %ld, %03d, %04ld  "), rpm, error, m_IntegralError[i], derivative, output);
            
            int16 pError = error / 8;
            for (int8_t i = -75; i <= 75; ++i) {
                if ((pError < 0 && i <= 0 && i > pError) || (pError >= 0 && i >= 0 && i < pError)) {
                    printf_P(PSTR("*"));
                } else {
                    printf_P(PSTR("."));                    
                }
            }
            printf_P(PSTR(" "));
            int16_t power = output * 20 / FAN_PWM_PERIOD;
            for (int8_t i = 0 ; i < 20; ++i) {
                if (i < power) {
                    printf_P(PSTR("*"));
                } else {
                    printf_P(PSTR("."));
                }
            }
            printf_P(PSTR("\n"));*/
            if (i == 0) {
                FAN_PWM_TIMER.CCABUF = output;
            } else {
                FAN_PWM_TIMER.CCBBUF = output;                
            }                
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void FanController::SetRPM( int8_t fanID, uint16_t speed )
{

}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

uint32_t FanController::GetRPM( int8_t fanID )
{
    cli();
    uint32_t period = (fanID == 0) ? g_PrevFan1Periode : g_PrevFan2Periode;
    sei();
    
    return CPU_FREQ * 60 / 2 / period;
}

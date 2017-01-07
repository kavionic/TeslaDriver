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
#include <string.h>

#include "ADCController.h"
#include "Hardware.h"

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ADCController::Initialize()
{
    // Disable digital input on pins used for analog IO:
    PORTA.PIN5CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN6CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTA.PIN7CTRL = PORT_ISC_INPUT_DISABLE_gc;
    
    PORTB.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTB.PIN3CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTB.PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc;
    PORTB.PIN5CTRL = PORT_ISC_INPUT_DISABLE_gc;

    // Setup ADC A for measuring the various voltage rails:
    ADCA.CTRLA = ADC_ENABLE_bm;
    ADCA.CTRLB = /*ADC_IMPMODE_bm |*/ ADC_FREERUN_bm;
    ADCA.REFCTRL = ADC_REFSEL_INT1V_gc | ADC_BANDGAP_bm | ADC_TEMPREF_bm;
    ADCA.EVCTRL  = ADC_EVACT_SWEEP_gc | ADC_SWEEP_0123_gc;
    ADCA.PRESCALER = ADC_PRESCALER_DIV512_gc; // Max 125kHz ADC clock when measuring internal signals.
    

    ADC_VSENSE3_3.CTRL = ADC_CH_INPUTMODE_INTERNAL_gc;
    ADC_VSENSE3_3.MUXCTRL = ADC_CH_MUXINT_SCALEDVCC_gc;
    
    ADC_VSENSE5.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
    ADC_VSENSE5.MUXCTRL = /*ADC_CH_MUXNEG_GND_MODE3_gc |*/ ADC_CH_MUXPOS_PIN5_gc;

    ADC_VSENSE12.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
    ADC_VSENSE12.MUXCTRL = /*ADC_CH_MUXNEG_GND_MODE3_gc |*/ ADC_CH_MUXPOS_PIN6_gc;

    ADC_VSENSE48.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
    ADC_VSENSE48.MUXCTRL = /*ADC_CH_MUXNEG_GND_MODE3_gc |*/ ADC_CH_MUXPOS_PIN7_gc;

    // Setup ADC B for current sensing:

    ADCB.CTRLA = ADC_ENABLE_bm;
    ADCB.CTRLB = ADC_IMPMODE_bm | ADC_CONMODE_bm;
    ADCB.REFCTRL = ADC_REFSEL_INT1V_gc | ADC_BANDGAP_bm;
    ADCB.EVCTRL  = ADC_EVSEL_4567_gc | ADC_EVACT_CH01_gc;
    ADCB.PRESCALER = ADC_PRESCALER_DIV16_gc;
    
    ADC_CURSENSE_LOW.CTRL = ADC_CH_INPUTMODE_DIFFWGAIN_gc | ADC_CH_GAIN_1X_gc;
    ADC_CURSENSE_LOW.MUXCTRL = ADC_CH_MUXNEG_PIN5_gc | ADC_CH_MUXPOS_PIN10_gc;

    ADC_CURSENSE_HIGH.CTRL = ADC_CH_INPUTMODE_DIFFWGAIN_gc | ADC_CH_GAIN_1X_gc;
    ADC_CURSENSE_HIGH.MUXCTRL = ADC_CH_MUXNEG_PIN5_gc | ADC_CH_MUXPOS_PIN10_gc;

    ADC_CURSENSE_REF.CTRL = ADC_CH_INPUTMODE_DIFFWGAIN_gc | ADC_CH_GAIN_1X_gc;
    ADC_CURSENSE_REF.MUXCTRL = ADC_CH_MUXNEG_PIN4_gc | ADC_CH_MUXPOS_PIN5_gc;

    
    // Setup DACs for generating current limit levels for the analog comparator:
    DACB.CTRLA = DAC_IDOEN_bm | DAC_CH1EN_bm | DAC_ENABLE_bm;
    DACB.CTRLB = DAC_CHSEL_DUAL_gc;
    DACB.CTRLC = DAC_REFSEL_AVCC_gc;

    UpdateCurrentSenseReferenceVoltage();

    DACB.CH0DATA = m_CurrentSenseOffset + int16_t(3.3/4095.0 * (0.5 - 50e-3) + 0.5);
    DACB.CH1DATA = m_CurrentSenseOffset + int16_t(3.3/4095.0 * (0.5 + 50e-3) + 0.5);
    
    // Setup analog comparator B in window mode to detect over/under current events:
    ACB.AC0MUXCTRL = AC_MUXNEG_PIN3_gc | AC_MUXPOS_PIN2_gc; // DAC CH1 / CURSENS_L
    ACB.AC1MUXCTRL = AC_MUXNEG_DAC_gc | AC_MUXPOS_PIN2_gc;  // DAC CH0 / CURSENS_L
    ACB.WINCTRL = AC_WEN_bm | AC_WINTMODE_OUTSIDE_gc;

    ACB.AC0CTRL = AC_ENABLE_bm | AC_HSMODE_bm | AC_HYSMODE_LARGE_gc | AC_INTMODE_BOTHEDGES_gc; // | AC_INTMODE1_bm; // High speed, Large hysteresis, Falling edge
    ACB.AC1CTRL = AC_ENABLE_bm | AC_HSMODE_bm | AC_HYSMODE_LARGE_gc | AC_INTMODE_BOTHEDGES_gc; // | AC_INTMODE1_bm | AC_INTMODE0_bm; // High speed, Large hysteresis, Rising edge

    // Setup events for trigging current sense samples at end of each half-cycle.
    EVSYS.CH4CTRL = EVSYS_DIGFILT_1SAMPLE_gc;
    EVSYS.CH4MUX  = PWM_SAMPLE_LO_TRIG;
    
    EVSYS.CH5CTRL = EVSYS_DIGFILT_1SAMPLE_gc;
    EVSYS.CH5MUX  = PWM_SAMPLE_HI_TRIG;
    
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ADCController::Run()
{
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

uint8_t ADCController::BinarySearch( int16_t a[], uint8_t length, int16_t key )
{
    uint8_t low = 0;
    uint8_t high = length;

    for (;;)
    {
        if ( low == high ) return low;
        uint8_t mid = low + ((high - low) >> 1);

        if (key > a[mid])
        {
            low = mid + 1;
            continue;
        }
        else if (key < a[mid])
        {
            high = mid;
            continue;
        }
        return mid;
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ADCController::InsertSample( int16_t* buffer, uint8_t len, int16_t value )
{
    uint8_t i = BinarySearch(buffer, len, value);
    memmove(buffer + i + 1, buffer + i, (len - i) * sizeof(*buffer));
    buffer[i] = value;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ADCController::SampleCurrent()
{
    InsertSample(m_CurrentMeasurementsLow, m_CurrentMeasurementsCount, ADC_CURSENSE_LOW.RES);
    InsertSample(m_CurrentMeasurementsHigh, m_CurrentMeasurementsCount, ADC_CURSENSE_HIGH.RES);
    if (m_CurrentMeasurementsCount != CURRENT_MEASURE_COUNT - 1)
    {
        m_CurrentMeasurementsCount++;
    }
    else
    {
        m_CurrentMeasureLow  = (m_CurrentMeasurementsLow[CURRENT_MEASURE_COUNT/2] * 8 - m_CurrentMeasureLow) / 8;
        m_CurrentMeasureHigh = (m_CurrentMeasurementsHigh[CURRENT_MEASURE_COUNT/2] * 8 - m_CurrentMeasureHigh) / 8;
        m_CurrentMeasurementsCount = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ADCController::SetCurrentLimits(uint16_t limitLow, uint16_t limitHigh)
{
    DACB.CH0DATA = m_CurrentSenseOffset + limitLow;
    DACB.CH1DATA = m_CurrentSenseOffset + limitHigh;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ADCController::GetCurrentLimits(uint16_t* limitLow, uint16_t* limitHigh) const
{
    *limitLow  = DACB.CH0DATA - m_CurrentSenseOffset;
    *limitHigh = DACB.CH1DATA - m_CurrentSenseOffset;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

uint16_t ADCController::GetVoltage3()
{
    int32_t voltage = ADC_VSENSE3_3.RES - 200;
    return (voltage * 10000) / 4095;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

uint16_t ADCController::GetVoltage5()
{
    int32_t voltage = ADC_VSENSE5.RES - 200;
    return (voltage * 7800) / 4095; // voltage * 1000mS * 7800ohm / 1000ohm / 4095
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

uint16_t ADCController::GetVoltage12()
{
    int32_t voltage = ADC_VSENSE12.RES - 200;
    return (voltage * 21000) / 4095; // voltage * 1000mS * 21000ohm / 1000ohm / 4095
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

uint16_t ADCController::GetVoltage48()
{
    int32_t voltage = ADC_VSENSE48.RES - 200;
    return (voltage * 69000) / 4095; // voltage * 1000mS * 69000ohm / 1000ohm / 4095
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

int16_t ADCController::GetCurrentSenseRefVoltage()
{
    return m_CurrentSenseOffset;    
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////
 
int16_t ADCController::GetCurrentLow()
{
    return m_CurrentMeasureLow / 8; //ADC_CURSENSE_LOW.RES;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

int16_t ADCController::GetCurrentHigh()
{
    return m_CurrentMeasureHigh / 8; // ADC_CURSENSE_HIGH.RES;    
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ADCController::UpdateCurrentSenseReferenceVoltage()
{
    uint16_t oldDAC = DACB.CH1DATA;
    DACB.CH1DATA = 2048;
    
    // Discard first sample. Mostly to give the DAC time to propagate it's output.
    ADC_CURSENSE_REF.INTFLAGS = ADC_CH_CHIF_bm; // Clear interrupt flag;
    ADC_CURSENSE_REF.CTRL |= ADC_CH_START_bm;        
    while(!(ADC_CURSENSE_REF.INTFLAGS & ADC_CH_CHIF_bm)); // Wait for conversion
    
    int32_t sampleSum = 0;
    for (int16_t i = 512; i; --i)
    {
        ADC_CURSENSE_REF.INTFLAGS = ADC_CH_CHIF_bm; // Clear interrupt flag;
        ADC_CURSENSE_REF.CTRL |= ADC_CH_START_bm;
    
        while(!(ADC_CURSENSE_REF.INTFLAGS & ADC_CH_CHIF_bm)); // Wait for conversion
        sampleSum += int16_t(ADC_CURSENSE_REF.RES);
    }
    int16_t offset = (sampleSum + 256) / 512;
    m_CurrentSenseOffset = (sampleSum * 100) / (int32_t(512)*165); // Divide by 1.65 to get from +/-1V reference to 3.3V reference.
    
    DB_LOG(DP_INFO, PSTRL("Current sense offset: "), offset, PSTRL(", "), m_CurrentSenseOffset);
    
    DACB.CH1DATA = oldDAC;
}
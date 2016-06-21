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
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Hardware.h"

#include "DS18B20.h"

#include "Misc/DigitalPort.h"

static const uint16_t TIME_RESET          = U64(480) * CPU_FREQ / 1000000;
static const uint16_t TIME_PRESENCE_TOTAL = U64(480) * CPU_FREQ / 1000000;
static const uint16_t TIME_PRESENCE_TEST  = U64(65) * CPU_FREQ / 1000000;
static const uint16_t TIME_DATA_SLOT      = U64(61) * CPU_FREQ / 1000000;
static const uint16_t TIME_WRITE_0        = U64(60) * CPU_FREQ / 1000000;
static const uint16_t TIME_WRITE_1        = U64(2) * CPU_FREQ / 1000000;
static const uint16_t TIME_READ_START     = U64(2) * CPU_FREQ / 1000000;
static const uint16_t TIME_READ_TEST      = U64(10) * CPU_FREQ / 1000000;

static const uint8_t SCRATCHPAD_READ_BYTE_TEMP1 = 0;
static const uint8_t SCRATCHPAD_READ_BYTE_TEMP2 = 1;
static const uint8_t SCRATCHPAD_READ_BYTE_CFG1  = 2;
static const uint8_t SCRATCHPAD_READ_BYTE_CFG2  = 3;
static const uint8_t SCRATCHPAD_READ_CRC1       = 4;
static const uint8_t SCRATCHPAD_READ_CRC2       = 5;

volatile int16_t DS18B20::s_Temperature1;
volatile int16_t DS18B20::s_Temperature2;
volatile uint8_t DS18B20::s_ReadingIndex;

static int16_t g_Temperature1Buf;
static int16_t g_Temperature2Buf;

static DS18B20::LogicState_e g_LogicState;
static DS18B20::State_e      g_IOState;
static uint8_t               g_SensorConfig = DS18B20::CFG_RES_UNUSED_BITS | DS18B20::CFG_RES_9BIT_gc;
static uint8_t               g_Data[10];
static uint8_t               g_CurrentBit;
static uint8_t               g_BytesToTransfer;
static volatile bool         g_CycleDone;


///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

static inline void StartPulse(uint16_t pulseWidth, uint16_t periode)
{
    cli();
    TEMP_SENS_TIMER.CNT = 1;
    TEMP_SENS_TIMER.CCC = periode;
    TEMP_SENS_TIMER.CCA = pulseWidth;
    TEMP_SENS_TIMER.CCB = pulseWidth;
    sei();
    TEMP_SENS_TIMER.CTRLC = TC0_CMPA_bm | TC0_CMPB_bm;
    DigitalPort::SetAsOutput(TEMP_SENS_PORT, TEMP_SENS_PIN1 | TEMP_SENS_PIN2);
    TEMP_SENS_TIMER.CTRLA = TC_CLKSEL_DIV1_gc;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

static inline void StartReset()
{
    g_IOState = DS18B20::e_Reset;
    StartPulse(TIME_RESET, TIME_RESET + TIME_PRESENCE_TEST);
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void DS18B20::Initialize()
{
    TEMP_SENS_TIMER.INTCTRLB = TC_CCCINTLVL_HI_gc;
    
    TEMP_SENS_TIMER.CTRLB = TC0_CCAEN_bm | TC0_CCBEN_bm | TC_WGMODE_SINGLESLOPE_gc;
    
    DigitalPort::InvertPins(TEMP_SENS_PORT, TEMP_SENS_PIN1 | TEMP_SENS_PIN2);
    DigitalPort::SetPinsOutputAndPullConfig(TEMP_SENS_PORT, TEMP_SENS_PIN1 | TEMP_SENS_PIN2, PORT_OPC_WIREDAND_gc);
    
    DigitalPort::SetHigh(TEMP_SENS_PORT, TEMP_SENS_PIN1 | TEMP_SENS_PIN2);
    DigitalPort::SetAsOutput(TEMP_SENS_PORT, PIN4_bm);
    
    g_Data[0] = CMD_SKIP_ROM;
    g_Data[1] = CMD_WRITE_SCRATCHPAD; // Write config.
    g_Data[2] = 0;
    g_Data[3] = 0;
    g_Data[4] = g_SensorConfig;
    g_BytesToTransfer = 5;
    
    g_LogicState = e_WriteConfig;
    StartReset();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

int16_t DS18B20::GetTemp1()
{
        cli();
        int16_t temp = s_Temperature1;
        sei();
        return temp;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

int16_t DS18B20::GetTemp2()
{
        cli();
        int16_t temp = s_Temperature2;
        sei();
        return temp;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void DS18B20::SetResolution(uint8_t resolution)
{
    g_SensorConfig = CFG_RES_UNUSED_BITS | ((resolution - 9) << CFG_RES_POS);
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

uint8_t DS18B20::GetResolution()
{
    return ((g_SensorConfig & CFG_RES_MASK) >> CFG_RES_POS) + 9;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

ISR(TCF0_CCC_vect)
{
    DigitalPort::SetAsInput(TEMP_SENS_PORT, TEMP_SENS_PIN1 | TEMP_SENS_PIN2);
    uint8_t portData = ~DigitalPort::Get(TEMP_SENS_PORT);
    TEMP_SENS_TIMER.CTRLA = 0; // Stop the timer.
    TEMP_SENS_TIMER.CNT = 0;
    sei();
    
    if (g_IOState == DS18B20::e_Reset)
    {
        if ((portData & (TEMP_SENS_PIN1 | TEMP_SENS_PIN2)) == 0)
        {
            cli();
            TEMP_SENS_TIMER.CCC = TIME_PRESENCE_TOTAL - TIME_PRESENCE_TEST;
            sei();
            
            if (g_LogicState == DS18B20::e_WriteConfig || g_LogicState == DS18B20::e_StartConversion || g_LogicState == DS18B20::e_RequestScratchpad)
            {
                g_IOState = DS18B20::e_Writing;
            }
            else if (g_LogicState == DS18B20::e_ReadScratchpad || g_LogicState == DS18B20::e_WaitForConversion)
            {
                g_IOState = DS18B20::e_StartReadSlot;
            }
            TEMP_SENS_TIMER.CTRLA = TC_CLKSEL_DIV1_gc;
            //printf_P(PSTR("S"));
        }
        else
        {
//            printf_P(PSTR("F"));
            StartReset();
        }
    }
    else if (g_IOState == DS18B20::e_Writing)
    {
        uint8_t byte = g_CurrentBit >> 3;;
        uint8_t bit  = g_CurrentBit & 0x7;
        if (byte == g_BytesToTransfer)
        {
            g_CurrentBit = 0;
            if (g_LogicState == DS18B20::e_WriteConfig)
            {
                g_Data[0] = DS18B20::CMD_SKIP_ROM;
                g_Data[1] = DS18B20::CMD_CONVERT_TEMP;
                g_BytesToTransfer = 2;
                g_LogicState = DS18B20::e_StartConversion;
                StartReset();
            }
            else if (g_LogicState == DS18B20::e_StartConversion)
            {
                g_LogicState = DS18B20::e_WaitForConversion;
                g_IOState = DS18B20::e_StartReadSlot;
                StartPulse(TIME_READ_START, TIME_READ_TEST);
            }                
            else if (g_LogicState == DS18B20::e_RequestScratchpad)
            {
//                g_Data[0] = 0;
//                g_Data[1] = 0;
                g_LogicState = DS18B20::e_ReadScratchpad;
                g_IOState = DS18B20::e_StartReadSlot;
                StartPulse(TIME_READ_START, TIME_READ_TEST);
            }                
        }
        else
        {
            if (g_Data[byte] & BIT8(bit,1)) {
                StartPulse(TIME_WRITE_1, TIME_DATA_SLOT);
            } else {
                StartPulse(TIME_WRITE_0, TIME_DATA_SLOT);
            }
            g_CurrentBit++;
        }            
    }
    else if (g_IOState == DS18B20::e_StartReadSlot)
    {
        StartPulse(TIME_READ_START, TIME_READ_TEST);
        g_IOState = DS18B20::e_Reading;
    }
    else if (g_IOState == DS18B20::e_Reading)
    {
        //bool inputState = !(portData & TEMP_SENS_PIN1);
        
        if (g_LogicState != DS18B20::e_WaitForConversion)
        {
            g_IOState = DS18B20::e_StartReadSlot;
            uint8_t curTemp1Bit = (portData >> TEMP_SENS_PIN1_bp) & 0x01;
            uint8_t curTemp2Bit = (portData >> TEMP_SENS_PIN2_bp) & 0x01;
            
            uint8_t byte = g_CurrentBit >> 3;
            uint8_t bit  = g_CurrentBit & 0x7;
            g_CurrentBit++;
            
            if (bit == 0)
            {
                g_Data[SCRATCHPAD_READ_BYTE_TEMP1] = 0;
                g_Data[SCRATCHPAD_READ_BYTE_TEMP2] = 0;
                if (byte == 0)
                {
                    g_Data[SCRATCHPAD_READ_CRC1] = 0;
                    g_Data[SCRATCHPAD_READ_CRC2] = 0;
                }                    
            }

            // XOR LSB of current CRC with the new bit.
            uint8_t feedback1 = (g_Data[SCRATCHPAD_READ_CRC1] & 0x01) ^ curTemp1Bit;  
            uint8_t feedback2 = (g_Data[SCRATCHPAD_READ_CRC2] & 0x01) ^ curTemp2Bit;

            g_Data[SCRATCHPAD_READ_CRC1] >>= 1;
            g_Data[SCRATCHPAD_READ_CRC2] >>= 1;
        

            if (feedback1) g_Data[SCRATCHPAD_READ_CRC1] ^= 0x8C; // CRC ^ b10001100
            if (feedback2) g_Data[SCRATCHPAD_READ_CRC2] ^= 0x8C; // CRC ^ b10001100
            
            g_Data[SCRATCHPAD_READ_BYTE_TEMP1] |= BIT8(bit, curTemp1Bit);
            g_Data[SCRATCHPAD_READ_BYTE_TEMP2] |= BIT8(bit, curTemp2Bit);

            if (bit == 7)
            {
                if (byte == 8)
                {
                    g_CurrentBit = 0;
                    if (g_Data[SCRATCHPAD_READ_CRC1] == 0 && g_Data[SCRATCHPAD_READ_CRC2] == 0)
                    {
                        cli();
                        DS18B20::s_Temperature1 = g_Temperature1Buf;
                        DS18B20::s_Temperature2 = g_Temperature2Buf;
                        sei();
                        DS18B20::s_ReadingIndex++;
                    
                
                        if ( ((g_Data[SCRATCHPAD_READ_BYTE_CFG1] ^ g_SensorConfig) & DS18B20::CFG_RES_MASK) == 0 && ((g_Data[SCRATCHPAD_READ_BYTE_CFG2] ^ g_SensorConfig) & DS18B20::CFG_RES_MASK) == 0)
                        {
                            g_Data[0] = DS18B20::CMD_SKIP_ROM;
                            g_Data[1] = DS18B20::CMD_CONVERT_TEMP;
                            g_BytesToTransfer = 2;
                            g_LogicState = DS18B20::e_StartConversion;
                        }
                        else
                        {
                            // Config has changed, or was corrupted on last update. Write it again.
                            g_Data[0] = DS18B20::CMD_SKIP_ROM;
                            g_Data[1] = DS18B20::CMD_WRITE_SCRATCHPAD; // Write config.
                            g_Data[2] = 0;
                            g_Data[3] = 0;
                            g_Data[4] = g_SensorConfig;
                            g_BytesToTransfer = 5;

                            g_LogicState = DS18B20::e_WriteConfig;
                        }                            
                    }
                    else
                    {
                        // CRC check failed. Re-read scratchpad.
                        //printf_P(PSTR("%d/%d\n"), g_Data[SCRATCHPAD_READ_CRC1], g_Data[SCRATCHPAD_READ_CRC1]);
                        g_Data[0] = DS18B20::CMD_SKIP_ROM;
                        g_Data[1] = DS18B20::CMD_READ_SCRATCHPAD;
                        g_BytesToTransfer = 2;
                        g_LogicState = DS18B20::e_RequestScratchpad;
                    }
                    StartReset();
                }
                else
                {
                    if (byte == 0) {
                        g_Temperature1Buf = g_Data[SCRATCHPAD_READ_BYTE_TEMP1];
                        g_Temperature2Buf = g_Data[SCRATCHPAD_READ_BYTE_TEMP2];
                    } else if (byte == 1) {
                        g_Temperature1Buf |= U16(g_Data[SCRATCHPAD_READ_BYTE_TEMP1]) << 8;
                        g_Temperature2Buf |= U16(g_Data[SCRATCHPAD_READ_BYTE_TEMP2]) << 8;
                    } else if (byte == 4) {
                        g_Data[SCRATCHPAD_READ_BYTE_CFG1] = g_Data[SCRATCHPAD_READ_BYTE_TEMP1];
                        g_Data[SCRATCHPAD_READ_BYTE_CFG2] = g_Data[SCRATCHPAD_READ_BYTE_TEMP2];
                    }                    
                    cli();
                    TEMP_SENS_TIMER.CCC = TIME_DATA_SLOT - TIME_READ_TEST;
                    sei();
                    TEMP_SENS_TIMER.CTRLA = TC_CLKSEL_DIV1_gc;
                }
            }
            else
            {
                cli();
                TEMP_SENS_TIMER.CCC = TIME_DATA_SLOT - TIME_READ_TEST;
                sei();
                TEMP_SENS_TIMER.CTRLA = TC_CLKSEL_DIV1_gc;                
            }
        }
        else
        {
            if ((portData & (TEMP_SENS_PIN1 | TEMP_SENS_PIN2)) == (TEMP_SENS_PIN1 | TEMP_SENS_PIN2))
            {
                g_Data[0] = DS18B20::CMD_SKIP_ROM;
                g_Data[1] = DS18B20::CMD_READ_SCRATCHPAD;
                g_BytesToTransfer = 2;
                g_LogicState = DS18B20::e_RequestScratchpad;
                StartReset();
                //printf_P(PSTR("+"));
            }
            else
            {
                cli();
                TEMP_SENS_TIMER.CCC = TIME_DATA_SLOT - TIME_READ_TEST;
                sei();
                TEMP_SENS_TIMER.CTRLA = TC_CLKSEL_DIV1_gc;
                //printf_P(PSTR("-"));
            }
        }
    }        
}

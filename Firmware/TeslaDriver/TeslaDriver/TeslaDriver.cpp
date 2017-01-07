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
#include <avr/wdt.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#include "Hardware.h"

/*
void * operator new(std::size_t n)
{
    void * const p = std::malloc(n);
    // handle p == 0
    return p;
}*/

/*void operator delete(void * p) // or delete(void *, std::size_t)
{
    free(p);
}*/

#include "Misc/SpinTimer.h"
#include "Misc/Clock.h"
#include "Misc/DigitalPort.h"
#include "Misc/Utils.h"
#include "Misc/Display.h"

#include "ESP8266/ESP8266.h"

#include "NetIF.h"
#include "ADCController.h"
#include "PWMController.h"
#include "DS18B20.h"
#include "UserIF.h"
#include "Keyboard.h"
#include "Beeper.h"
#include "FanController.h"
#include "EventSys.h"
#include "SystemStats.h"
#include "Config.h"

#include "Misc/SpinTimer.cpp"
#include "Misc/Clock.cpp"
#include "Misc/Display.cpp"
#include "Misc/Utils.cpp"
#include "ESP8266/ESP8266.cpp"


ESP8266       g_WifiDevice;
ADCController ADCController::Instance;
PWMController g_PWMController;
DS18B20       g_TempSensor;
Keyboard      g_Keyboard;
DisplayLCD    g_Display;
NetIF         g_NetIF;
UserIF        g_UI;
FanController g_FanController;

/*static union
{
    struct 
    {
        uint16_t Lo;
        uint16_t Hi;
    };
    uint32_t Value = 0;
} g_HighresClock;*/

static uint32_t g_HighresClock;
static uint16_t g_TimerTick10  = 20;
static uint16_t g_TimerTick100 = 5;
static uint8_t  g_TimerTick500 = 5;
static uint8_t  g_TimerTick1000 = 2;

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void SetupClock()
{
    // Enable crystall oscillator in 12-16MHz low-power mode:
    OSC.XOSCCTRL = OSC_FRQRANGE_12TO16_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
    OSC.CTRL |= OSC_XOSCEN_bm;


    // Wait for oscillator to startup:
    while( (OSC.STATUS & OSC_XOSCRDY_bm) == 0 );

    // Enable PLL in x2 mode:
    OSC.PLLCTRL = OSC_PLLSRC_XOSC_gc | OSC_PLLFAC3_bm;
    OSC.CTRL |= OSC_PLLEN_bm;

    // Wait for PLL to lock:
    while( (OSC.STATUS & OSC_PLLRDY_bm) == 0 );

    // Select PLL as system clock source:
    CCP = CCP_IOREG_gc;
	CLK.PSCTRL = CLK_PSBCDIV_4_1_gc;
    CCP = CCP_IOREG_gc;
    CLK.CTRL = CLK_SCLKSEL_PLL_gc;
    
    // Disable the unused oscillators:
    OSC.CTRL = OSC_XOSCEN_bm | OSC_PLLEN_bm;

    // Peripheral Clock output: Disabled
    PORTCFG.CLKEVOUT=(PORTCFG.CLKEVOUT & (~PORTCFG_CLKOUT_gm)) | PORTCFG_CLKOUT_OFF_gc;
    //    PORTCFG.CLKEVOUT = PORTCFG_CLKOUTSEL_CLK1X_gc | PORTCFG_CLKOUT_PC7_gc;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

static int uart_putchar(char c, FILE *stream)
{
    if (c == '\n')
    uart_putchar('\r', stream);
    
    // Wait for the transmit buffer to be empty
    while ( !( DEBUG_USART.STATUS & USART_DREIF_bm) );
    
    // Put our character into the transmit buffer
    DEBUG_USART.DATA = c;
    
    return 0;
}

void DebugLogWriter::Write( char c )
{
    // Wait for the transmit buffer to be empty
    while ( !( DEBUG_USART.STATUS & USART_DREIF_bm) );
    
    // Put our character into the transmit buffer
    DEBUG_USART.DATA = c;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

/*ISR(TCE0_OVF_vect)
{
    g_HighresClock.Hi++;
}*/

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////
    
ISR(TCE0_OVF_vect)
{
    static uint8_t faultDelay = 0;
    
    if (faultDelay == 0)
    {
        if (AWEXC.STATUS & AWEX_FDF_bm)
        {
            faultDelay = 2;
        }
    }
    else
    {
        if (!--faultDelay)
        {
            AWEXC.STATUS |= AWEX_FDF_bm;
        }
    }        

//    SYSTEM_TIMER.CCC += CPU_FREQ / 1000;
    g_HighresClock += CPU_FREQ / 1000;
//    Clock::IncrementTime();
    Clock::AddCycles();
    sei();

    ADCController::Instance.SampleCurrent();

    if ( !(--g_TimerTick10) )
    {
        g_TimerTick10 = 20;
        if ( !(--g_TimerTick100) )
        {
            g_TimerTick100 = 5;
            EventSys::AddEvent(Event(EventID::e_TimeTick100, 0));
        
            if ( !(--g_TimerTick500) )
            {
                g_TimerTick500 = 5;
                EventSys::AddEvent(Event(EventID::e_TimeTick500, 0));
                if ( !(--g_TimerTick1000) )
                {
                    g_TimerTick1000 = 2;
                    EventSys::AddEvent(Event(EventID::e_TimeTick1000, 0));
                }
            }
        }
        //g_Keyboard.Run();
    }
    Beeper::Run();
//    g_Timer += 0x10000;
}

/*
static uint16_t ReadCalibrationWord(volatile void* address)
{
    uint16_t value;

    NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
    value = pgm_read_word(address);
    NVM_CMD = NVM_CMD_NO_OPERATION_gc;
    return value;
}*/

static uint8_t ReadCalibrationByte(volatile void* address)
{
    uint8_t value;

    NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
    value = pgm_read_byte(address);
    NVM_CMD = NVM_CMD_NO_OPERATION_gc;
    return value;
}

void panic()
{
    PWM_OUT_PORT->OUTCLR = PWM_OUT_PINS;
    AWEXC.OUTOVEN = PWM_OUT_PINS;
    PWM_OUT_PORT->DIRSET = PWM_OUT_PINS;
    g_Display.SetCursor(0,0);
    g_Display.Printf_P(PSTR("PANIC!"));
    cli();
    for(;;);
}

int main(void)
{
    PWM_OUT_PORT->OUTCLR = PWM_OUT_PINS;
    PWM_OUT_PORT->DIRSET = PWM_OUT_PINS;
    
    ADCA.CALL = ReadCalibrationByte(&((NVM_PROD_SIGNATURES_t*)PROD_SIGNATURES_START)->ADCACAL0);
    ADCA.CALH = ReadCalibrationByte(&((NVM_PROD_SIGNATURES_t*)PROD_SIGNATURES_START)->ADCACAL1);
    
    ADCB.CALL = ReadCalibrationByte(&((NVM_PROD_SIGNATURES_t*)PROD_SIGNATURES_START)->ADCBCAL0);
    ADCB.CALH = ReadCalibrationByte(&((NVM_PROD_SIGNATURES_t*)PROD_SIGNATURES_START)->ADCBCAL1);
    
    DACB.CH0GAINCAL = ReadCalibrationByte(&((NVM_PROD_SIGNATURES_t*)PROD_SIGNATURES_START)->DACB0GAINCAL);
    DACB.CH0OFFSETCAL = ReadCalibrationByte(&((NVM_PROD_SIGNATURES_t*)PROD_SIGNATURES_START)->DACB0OFFCAL);
    
    DACB.CH1GAINCAL = ReadCalibrationByte(&((NVM_PROD_SIGNATURES_t*)PROD_SIGNATURES_START)->DACB1GAINCAL);
    DACB.CH1OFFSETCAL = ReadCalibrationByte(&((NVM_PROD_SIGNATURES_t*)PROD_SIGNATURES_START)->DACB1OFFCAL);
    
    CCP = CCP_IOREG_gc;
    WDT.CTRL = (WDT.CTRL & ~WDT_ENABLE_bm) | WDT_CEN_bm;
    SetupClock();
 
    NVM.CTRLB = NVM_EEMAPEN_bm; // Map EEPROM into MAPPED_EEPROM_START
    
    
    SYSTEM_TIMER.INTCTRLA = TC_OVFINTLVL_MED_gc;
//    SYSTEM_TIMER.INTCTRLB = TC_CCCINTLVL_MED_gc;
//    SYSTEM_TIMER.CCC = CPU_FREQ / 1000;
    SYSTEM_TIMER.PERBUF = CLOCK_CPUCYCLES_PER_UPDATE - 1; //  CPU_FREQ / 1000;
    SYSTEM_TIMER.CTRLA = TC_CLKSEL_DIV1_gc;
    
    PMIC.CTRL |= PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;

//    CLOCK_TIMER.CTRLA = TC_CLKSEL_DIV1_gc;
//    CLOCK_TIMER.INTCTRLA |= TC_OVFINTLVL_MED_gc;
//    CLOCK_TIMER.PER = CPU_FREQ / 1000;
    

    // Set the TxD pin as an output - set PORTC OUT register bit 3 to 1
    //PORTE.DIRSET = PIN3_bm;
    DigitalPort::SetAsOutput(DEBUG_USART_PORT, DEBUG_USART_OUT_PIN);
    // Set baud rate & frame format
    DEBUG_USART.BAUDCTRLB = ((-3) << USART_BSCALE_gp) & USART_BSCALE_gm; ///*USART_BSCALE3_bm | USART_BSCALE2_bm | */ USART_BSCALE1_bm | USART_BSCALE0_bm; // BSCALE = 3
    DEBUG_USART.BAUDCTRLA = 131 << USART_BSEL_gp;
    
    // Set mode of operation
    DEBUG_USART.CTRLA = 0;				// Disable interrupts
    DEBUG_USART.CTRLC = USART_CMODE_ASYNCHRONOUS_gc | USART_PMODE_DISABLED_gc | USART_CHSIZE_8BIT_gc;			// async, no parity, 8 bit data, 1 stop bit

    // Enable receiver and transmitter.
    DEBUG_USART.CTRLB = USART_RXEN_bm | USART_TXEN_bm;

    static FILE mystdout;
    fdev_setup_stream(&mystdout, uart_putchar, NULL, _FDEV_SETUP_WRITE);
    stdout = &mystdout;
    
    printf_P(PSTR("Old Config version %d.\n"), g_EEPROM.global.m_Version);
    if (g_EEPROM.global.m_Version != EEPROMContent::VERSION)
    {
        printf_P(PSTR("****** Wrong config version %d! Resetting config\n"), g_EEPROM.global.m_Version);
        Config::ResetConfig();
    }
   printf_P(PSTR("Config version %d.\n"), g_EEPROM.global.m_Version);
   printf_P(PSTR("Kbd speed %d.\n"), g_EEPROM.keyboard.m_RepeatSpeed);

    g_Keyboard.Initialize();

    bool softwareReset  = false;
    printf_P(PSTR("Reset flags: %x\n"), RST.STATUS);
    if (RST.STATUS & RST_PORF_bm) {
        printf_P(PSTR("Power on reset.\n"));
    }
    if (RST.STATUS & RST_EXTRF_bm) {
        printf_P(PSTR("External reset.\n"));
    }
    if (RST.STATUS & RST_BORF_bm) {
        printf_P(PSTR("Reset by brownout!\n"));
    }
    if (RST.STATUS & RST_WDRF_bm) {
        printf_P(PSTR("Reset by watchdog!\n"));
    }
    if (RST.STATUS & RST_PDIRF_bm) {
        printf_P(PSTR("Programmer/debugger reset.\n"));
    }
    if (RST.STATUS & RST_SRF_bm) {
        printf_P(PSTR("Software reset.\n"));
        softwareReset = true;
    }
    if (RST.STATUS & RST_SDRF_bm) {
        printf_P(PSTR("Reset by spike-detect!\n"));
    }
    
    RST.STATUS |= RST_PORF_bm | RST_EXTRF_bm | RST_BORF_bm | RST_WDRF_bm | RST_PDIRF_bm | RST_SRF_bm | RST_SDRF_bm;

    g_FanController.Initialize();
    g_Display.InitializeDisplay();
    g_Display.EnableDisplay(true);
    g_UI.Initialize();
        
    g_TempSensor.Initialize();
    
    ADCController::Instance.Initialize();
    g_PWMController.Initialize();
 
    Beeper::Initialize();   
    
    QUAD_DECODER_TIMER.CNT = PWM_TIMER.CCCBUF * 2;
    
    printf_P(PSTR("CPU Initialized\n"));
    
    printf_P(PSTR("Initializing radio...\n"));
    
    PMIC.CTRL = PMIC_RREN_bm | PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
    
    sei();

    g_WifiDevice.Initialize(softwareReset, false);
    Beeper::Beep(BeepID::e_KeyPress);
    g_NetIF.Initialize();
    
    printf_P(PSTR("...radio initialized.\n"));
 
    //uint8_t tempReadingIndex = 0;
    
    uint32_t prevFrameTime = 0;
    int32_t averageFrameTime = 0;
   while(1)
    {
        Event event;
        
        if ( !EventSys::GetEvent(&event) )
        {
            event.type = EventID::e_None;
        }
        if (event.type == EventID::e_KeyDown) {
            printf_P(PSTR("Key down: %S\n"), Keyboard::GetKeyName(KbdKeyCodes::Enum(event.value)));
        }            
        else if (event.type == EventID::e_KeyRepeat) {
            printf_P(PSTR("Key repeat: %S\n"), Keyboard::GetKeyName(KbdKeyCodes::Enum(event.value)));
        } else if (event.type == EventID::e_KeyUp) {
            printf_P(PSTR("Key up: %S\n"), Keyboard::GetKeyName(KbdKeyCodes::Enum(event.value)));
        }
        g_FanController.Run(event);

        if ( !g_WifiDevice.Run()){
            printf_P(PSTR("ERROR: ESP8266::Run() failed! Restarting radio.\n"));
            g_NetIF.ReconfigureRadio();
        }
        if (event.type == EventID::e_TimeTick1000 && (g_WifiDevice.GetStatusFlags() & (ESP8266::e_StatusConnectedToAP | ESP8266::e_StatusHotspotEnabled)) && !g_WifiDevice.PokeRadio())
        {
            Beeper::Beep(BeepID::e_KeyPress);
            g_NetIF.ReconfigureRadio();
        }
        int32_t v1 = ADC_VSENSE5.RES;
        int32_t v2 = ADC_VSENSE12.RES;
        
        static const int32_t PREC = 1000;
        v1 = I32(v1) * PREC * 7800 / 1000 / 2047;
        v2 = I32(v2) * PREC * 21000 / 1000 / 2047;
        
//        printf_P(PSTR("%d.%03d/%d.%03d\n"), I16(v1 / PREC), I16(v1 % PREC), I16(v2 / PREC), I16(v2 % PREC));
        //DS18B20::Run();
/*        uint8_t curTempReadingIndex = DS18B20::GetReadingIndex();
        if (curTempReadingIndex != tempReadingIndex)
        {
            tempReadingIndex = curTempReadingIndex;
            
            uint16_t temp1 = DS18B20::GetTemp1();
            uint16_t temp2 = DS18B20::GetTemp2();
            printf_P(PSTR("TEMP: %d.%d / %d.%d\n"), temp1 / 16, (temp1 % 16) * 100 / 16, temp2 / 16, (temp2 % 16) * 100 / 16 );
        }*/
//        printf_P(PSTR("Run keyboard\n"));
//        g_Keyboard.Run();
//        printf_P(PSTR("Run net\n"));
        g_NetIF.Run(event);
//        printf_P(PSTR("Run ui\n"));
        g_UI.Run(event);
        
//        g_PWMController.Run();
//        printf_P(PSTR("done\n"));
        cli();
        uint32_t time = g_HighresClock + CPU_FREQ / 1000 - I16(SYSTEM_TIMER.CCC - SYSTEM_TIMER.CNT);
        sei();
        int32_t deltaTime = time - prevFrameTime;
        prevFrameTime = time;

//        averageFrameTime += I32((deltaTime << 5) - averageFrameTime) / 32;
        averageFrameTime += I32((deltaTime - averageFrameTime)) / 8192;
//        g_SystemStats.m_CyclesPerFrame = averageFrameTime >> 5;
        g_SystemStats.m_CyclesPerFrameAvg = averageFrameTime;
        //g_SystemStats.m_CyclesPerFrame -= 10;
        if (event.type == EventID::e_TimeTick500) {
            g_SystemStats.m_CyclesPerFrameMax = deltaTime;
            g_SystemStats.m_DSPCyclesMin = g_SystemStats.m_DSPCyclesMax = g_SystemStats.m_DSPCyclesPrev;
        } else if (deltaTime > g_SystemStats.m_CyclesPerFrameMax) {
            g_SystemStats.m_CyclesPerFrameMax = deltaTime;
        }            
    }
}
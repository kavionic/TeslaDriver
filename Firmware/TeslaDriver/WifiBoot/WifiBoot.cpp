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
#include <alloca.h>

#include "Hardware.h"

#include "Misc/SpinTimer.h"
#include "Misc/Clock.h"
#include "Misc/DigitalPort.h"
#include "Misc/Utils.h"
#include "Misc/Display.h"

#include "ESP8266/ESP8266.h"

#include "Misc/SpinTimer.cpp"
#include "Misc/Clock.cpp"
//#include "Misc/Display.cpp"
#include "Misc/Utils.cpp"
#include "Misc/PString.cpp"
#include "Misc/PString.h"
//#include "Misc/Display.cpp"
#include "ESP8266/ESP8266.cpp"
#include "Config.cpp"
#include "BootNetIF.h"

//DisplayLCD    g_Display;
ESP8266       g_WifiDevice;
BootNetIF     g_BootNetIF;

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

ISR(TCE0_OVF_vect)
{
    Clock::AddCycles();
    sei();
/*    
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
    Beeper::Run();*/
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void DebugLogWriter::Write( char c )
{
    // Wait for the transmit buffer to be empty
    while ( !( DEBUG_USART.STATUS & USART_DREIF_bm) );
    
    // Put our character into the transmit buffer
    DEBUG_USART.DATA = c;
}


int main(void)
{
    PWM_OUT_PORT->OUTCLR = PWM_OUT_PINS;
    PWM_OUT_PORT->DIRSET = PWM_OUT_PINS;
    
    bool softwareReset  = (RST.STATUS & RST_SRF_bm) != 0;

    if (WifiBootMode_e(eeprom_read_byte(&g_EEPROMunmapped.global.m_PreferredBootMode)) == WifiBootMode_e::e_Application && pgm_read_word_near(0) != 0xffff) {
        asm("jmp 0");
    }
    RST.STATUS |= RST_PORF_bm | RST_EXTRF_bm | RST_BORF_bm | RST_WDRF_bm | RST_PDIRF_bm | RST_SRF_bm | RST_SDRF_bm;

    SetupClock();

    DigitalPort::InvertPins(FAN_PWM_PORT, FAN_PWM1_PIN | FAN_PWM2_PIN);
    DigitalPort::SetAsOutput(FAN_PWM_PORT, FAN_PWM1_PIN | FAN_PWM2_PIN);

//    NVM.CTRLB = NVM_EEMAPEN_bm; // Map EEPROM into MAPPED_EEPROM_START
    
       
    SYSTEM_TIMER.INTCTRLA = TC_OVFINTLVL_MED_gc;
    //    SYSTEM_TIMER.INTCTRLB = TC_CCCINTLVL_MED_gc;
    //    SYSTEM_TIMER.CCC = CPU_FREQ / 1000;
    SYSTEM_TIMER.PERBUF = CLOCK_CPUCYCLES_PER_UPDATE - 1; //  CPU_FREQ / 1000;
    SYSTEM_TIMER.CTRLA = TC_CLKSEL_DIV1_gc;
       

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

    CCP = CCP_IOREG_gc;
    PMIC.CTRL = PMIC_RREN_bm | PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm | PMIC_IVSEL_bm;
    sei();


//    g_Display.InitializeDisplay();
//    g_Display.EnableDisplay(true);

    g_WifiDevice.Initialize(softwareReset, false);
    g_BootNetIF.Initialize();

    while(1)
    {
        if ( !g_WifiDevice.Run()){
            DB_LOG(DP_ERROR, PSTR("ERROR: ESP8266::Run() failed! Restarting radio.\n"));
            g_BootNetIF.ReconfigureRadio();
       }
        g_BootNetIF.Run();
    }
}

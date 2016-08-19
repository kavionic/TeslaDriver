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

#include "Misc/SpinTimer.h"
#include "Misc/Clock.h"
#include "Misc/DigitalPort.h"
#include "Misc/Utils.h"
#include "Misc/Display.h"

#include "ESP8266/ESP8266.h"

#include "Misc/SpinTimer.cpp"
#include "Misc/Clock.cpp"
#include "Misc/Display.cpp"
#include "Misc/Utils.cpp"
#include "ESP8266/ESP8266.cpp"


ESP8266       g_WifiDevice;

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


int main(void)
{
    SetupClock();
       
    NVM.CTRLB = NVM_EEMAPEN_bm; // Map EEPROM into MAPPED_EEPROM_START
    
       
    SYSTEM_TIMER.INTCTRLA = TC_OVFINTLVL_MED_gc;
    //    SYSTEM_TIMER.INTCTRLB = TC_CCCINTLVL_MED_gc;
    //    SYSTEM_TIMER.CCC = CPU_FREQ / 1000;
    SYSTEM_TIMER.PERBUF = CLOCK_CPUCYCLES_PER_UPDATE - 1; //  CPU_FREQ / 1000;
    SYSTEM_TIMER.CTRLA = TC_CLKSEL_DIV1_gc;
       
    PMIC.CTRL |= PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;

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

    CPU_RAMPZ = 0x04;
    printf("Hello world: (%c)\n", pgm_read_byte(PSTR("Hello world!\n")));
//    printf("Hello world: (%c)\n", pgm_read_byte_far(int32(PSTR("Hello world!\n")) | 0x20000));
    //printf_P((const char*) uint16_t(PSTR("Hello world!\n")));
    while(1)
    {
    }
}
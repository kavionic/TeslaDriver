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


#ifndef HARDWARE_H_
#define HARDWARE_H_

///////////////////////////////////////
// Ports:
//  PORTA
//   -PIN3(1)   : WIFI Reset
//  PORTB
//   -PIN6(12)  : Quad decoder(1) phase 1
//   -PIN7(13)  : Quad decoder(3) phase 2
//  PORTC
//   -PIN4(20)  : PWM1H(2)
//   -PIN5(21)  : PWM1L(1)
//   -PIN6(22)  : PWM2L(3)
//   -PIN7(23)  : PWM2H(4)
//  PORTD
//   -PIN0(26)  : FAN1 PWM
//   -PIN1(27)  : FAN2 PWM
//   -PIN2(28)  : Debug RXD
//   -PIN3(29)  : Debug TXD
//  PORTE
//   -PIN0(36)  : FAN1 RPM
//   -PIN1(37)  : FAN2 RPM
//  PORTF
//   -PIN0(46)  : Temp sensor1
//   -PIN1(47)  : Temp sensor2
//   -PIN2(48)  : WIFI UART(TX/22) WIFI->MCU
//   -PIN3(49)  : WIFI UART(RX/21) MCU->WIFI
//   -PIN6(54)  : WIFI UART(TX/22) WIFI->MCU
//   -PIN7(55)  : WIFI UART(RX/21) MCU->WIFI
//  PORTR

///////////////////////////////////////
// Event channels:
//  CH0 : Quad decoder.
//  CH1 : Modulation DMA trigger.
//  CH2 : PWM fault.
//  CH3
//  CH4
//  CH5
//  CH6 : FAN1 RPM
//  CH7 : FAN2 RPM

///////////////////////////////////////
// Timers:
//  TCC0    : Main PWM.
//   -TCC2
//  TCC1    : PWM modulation DMA trigger.
//
// TCD0     : PWM for FAN1/FAN2/LCD_CONTRAST/LCD_LED
//  -TCD2
// TCD1     : Buzzer
//
// TCE0     : System clock timer. / System timer (spin timer). / FAN1/FAN2 RPM capture.
//  -TCE2
// TCE1     : Quad decoder.
//
// TCF0     : Temperature sensor IO timer.
//  -TCF2

///////////////////////////////////////
// USART:
//  USARTC0
//  USARTC1
//  USARTD0 : Debugging.
//  USARTD1
//  USARTE0
//  USARTE1
//  USARTF0 : WIFI

///////////////////////////////////////
// AD Converters:
//  ADCA
//   -CH0
//   -CH1
//   -CH2
//   -CH3
//
//  ADCB
//   -CH0
//   -CH1
//   -CH2
//   -CH3

///////////////////////////////////////
// DA Converters:
//  DACB
//   -CH0
//   -CH1

///////////////////////////////////////
// Analog Comparators:
//  ACA
//   -CH0
//   -CH1
//  ACB
//   -CH0
//   -CH1


#define CPU_FREQ 32000000
#define CLOCK_CPUCYCLES_PER_UPDATE    32768
#define CLOCK_PERIOD_BIT_COUNT      15


/// System timer //////////////////////////////////////////////////////////////

#define SYSTEM_TIMER TCE0

#define SPINTIMER_TIMERL SYSTEM_TIMER.CNTL
#define SPINTIMER_FREQ CPU_FREQ

/// Debug serial interface ////////////////////////////////////////////////////

#define DEBUG_USART USARTD0
#define DEBUG_USART_PORT e_DigitalPortID_D
#define DEBUG_USART_IN_PIN   PIN2_bm
#define DEBUG_USART_OUT_PIN  PIN3_bm

/// Quadrature decoder ////////////////////////////////////////////////////////

#define QUAD_DECODER_EVCH_MUX  EVSYS.CH0MUX
#define QUAD_DECODER_EVCH_CTRL EVSYS.CH0CTRL
#define QUAD_DECODER_TIMER_EVENT_SRC TC_EVSEL_CH0_gc

#define QUAD_DECODER_TIMER TCE1

#define QUAD_DECODER_PORT   e_DigitalPortID_B
#define QUAD_DECODER_PIN0   PIN6_bm
#define QUAD_DECODER_PIN1   PIN7_bm


/// Keyboard //////////////////////////////////////////////////////////////////

#define KEYBOARD_P0_PORT    e_DigitalPortID_E
#define KEYBOARD_P1_PORT    e_DigitalPortID_D
#define KEYBOARD_P2_PORT    e_DigitalPortID_D

#define KEYBOARD_P0_PIN     PIN4_bm
#define KEYBOARD_P1_PIN     PIN6_bm
#define KEYBOARD_P2_PIN     PIN7_bm

// Buzzer:
#define BUZZER_TIMER TCD1
#define BUZZER_PORT e_DigitalPortID_D
#define BUZZER_PIN1 PIN4_bm
#define BUZZER_PIN2 PIN5_bm

/// Temperature sensor ////////////////////////////////////////////////////////

#define TEMP_SENS_TIMER TCF0
#define TEMP_SENS_PORT e_DigitalPortID_F

#define TEMP_SENS_PIN1    PIN0_bm
#define TEMP_SENS_PIN1_bp PIN0_bp

#define TEMP_SENS_PIN2    PIN1_bm
#define TEMP_SENS_PIN2_bp PIN1_bp

/// PWM peripherals ///////////////////////////////////////////////////////////

#define PWM_TIMER TCC0
#define PWM_OUT_PORT      e_DigitalPortID_C
#define PWM_OUT_PINS      (PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm)
#define PWM_AWEX_CHANNELS (AWEX_DTICCAEN_bm | AWEX_DTICCBEN_bm)

#define PWM_CMP_LO_PWM    CCABUF
#define PWM_CMP_HI_PWM    CCBBUF
#define PWM_CMP_LO_SAMPLE CCCBUF
#define PWM_CMP_HI_SAMPLE CCDBUF

#define PWM_SAMPLE_LO_TRIG  EVSYS_CHMUX_TCC0_CCC_gc
#define PWM_SAMPLE_HI_TRIG  EVSYS_CHMUX_TCC0_CCD_gc

#define PWM_MODULATION_TIMER TCC1

#define PWM_FAULT_EVENTS    BIT(2,1)

#define PWM_MODULATION_EVCH_MUX          EVSYS.CH1MUX
#define PWM_MODULATION_EVCH_CTRL         EVSYS.CH1CTRL
#define PWM_MODULATION_DMA_TRIGGER       DMA_CH_TRIGSRC_EVSYS_CH1_gc
#define PWM_MODULATION_EVENT_TRIGGER_SRC EVSYS_CHMUX_TCC1_OVF_gc


/// WIFI INTERFACE ////////////////////////////////////////////////////////////

#define WIFI_RADIO_RESET_PORT e_DigitalPortID_F
#define WIFI_RADIO_RESET_PIN  PIN5_bm

#define WIFI_USART USARTF0
#define WIFI_USART_RX_INT USARTF0_RXC_vect
#define WIFI_USART_OUT_PORT e_DigitalPortID_F
#define WIFI_USART_IN_PIN   PIN6_bm
#define WIFI_USART_OUT_PIN  PIN7_bm

/// DISPLAY ///////////////////////////////////////////////////////////////////

#define DISPLAY_DATA_PORT            e_DigitalPortID_A

#define DISPLAY_REGISTER_SELECT_PORT e_DigitalPortID_B
#define DISPLAY_RW_PORT              e_DigitalPortID_B
#define DISPLAY_ENABLE_PORT          e_DigitalPortID_F

#define DISPLAY_REGISTER_SELECT PIN1_bm
#define DISPLAY_RW              PIN0_bm
#define DISPLAY_ENABLE          PIN4_bm

/// FAN CONTROL ///////////////////////////////////////////////////////////////
#define FAN_PWM_TIMER  TCD0
#define FAN_RPM_TIMER  TCE0

#define FAN_PWM_PORT    e_DigitalPortID_D
#define FAN_PWM1_PIN    PIN0_bm
#define FAN_PWM2_PIN    PIN1_bm
#define FAN_PWM_PERIOD (CPU_FREQ / 25000)

#define FAN_RPM_PORT    e_DigitalPortID_E
#define FAN_RPM1_PIN    PIN0_bm
#define FAN_RPM2_PIN    PIN1_bm

#include "ESP8266/ESP8266.h"
#include "Misc/Display.h"

void panic();

extern ESP8266    g_WifiDevice;
extern DisplayLCD g_Display;

#endif /* HARDWARE_H_ */
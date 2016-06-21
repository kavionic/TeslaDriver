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

#include "Keyboard.h"
#include "Beeper.h"
#include "Hardware.h"
#include "Config.h"
#include "EventSys.h"

#include "Misc/DigitalPort.h"
#include "Misc/Clock.h"
#include "Misc/SpinTimer.h"

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void Keyboard::Initialize()
{
    DigitalPort::EnablePullup(KEYBOARD_P0_PORT, KEYBOARD_P0_PIN);
    DigitalPort::EnablePullup(KEYBOARD_P1_PORT, KEYBOARD_P1_PIN);
    DigitalPort::EnablePullup(KEYBOARD_P2_PORT, KEYBOARD_P2_PIN);
    
    // Setup input pins.
    QUAD_DECODER_PORT->PIN6CTRL = PORT_ISC_LEVEL_gc | PORT_OPC_PULLUP_gc;
    QUAD_DECODER_PORT->PIN7CTRL = PORT_ISC_LEVEL_gc | PORT_OPC_PULLUP_gc;

    // Select the QDPH0 pin as a multiplexer input for an event channel.
    QUAD_DECODER_EVCH_MUX = EVSYS_CHMUX_PORTB_PIN6_gc;
    // Enable quadrature decoding and digital filtering in the event channel.
    QUAD_DECODER_EVCH_CTRL = EVSYS_QDEN_bm | EVSYS_DIGFILT_2SAMPLES_gc;

    // Set quadrature decoding on channel 0 as the event action for the timer.
    QUAD_DECODER_TIMER.CTRLD = TC_EVACT_QDEC_gc | QUAD_DECODER_TIMER_EVENT_SRC;
    // Set the period register of the timer. 8-bit resolution after accounting for 4 state changes / indent.
    QUAD_DECODER_TIMER.PER = 256 * 4 - 1;
    // Enable the timer/counter without clock prescaling.
    QUAD_DECODER_TIMER.CTRLA=TC_CLKSEL_DIV1_gc;
    QUAD_DECODER_TIMER.CNT = 0;    
    
    m_ButtonStates = 0;
    m_CurrentRow = 0;
    m_CurrentButton = 0;
    m_LastPressedKey = -1;
    DigitalPort::SetAsOutput(KEYBOARD_P0_PORT, KEYBOARD_P0_PIN);
}    

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void Keyboard::Run()
{
    uint8_t curButtonState = m_ButtonStates;

    static PORT_t* const ports[] = {KEYBOARD_P0_PORT, KEYBOARD_P1_PORT, KEYBOARD_P2_PORT};
    static const uint8_t pins[] = {KEYBOARD_P0_PIN, KEYBOARD_P1_PIN, KEYBOARD_P2_PIN};
        
    for (uint8_t i = 0 ; i < 3 ; ++i)
    {
        if (i != m_CurrentRow)
        {
            if ((DigitalPort::Get(ports[i]) & pins[i]) == 0) {
                curButtonState |= BIT8(m_CurrentButton, 1);
//                printf_P(PSTR("%d (%d, %d)\n"), m_CurrentButton, m_CurrentRow, i);
            } else {
                curButtonState &= U8(~BIT8(m_CurrentButton, 1));
            }
            m_CurrentButton++;
        }
    }
    DigitalPort::SetAsInput(ports[m_CurrentRow], pins[m_CurrentRow]);
    
    m_CurrentRow++;
    if (m_CurrentRow >= 3) {
        m_CurrentRow = 0;
        m_CurrentButton = 0;
    }
    DigitalPort::SetAsOutput(ports[m_CurrentRow], pins[m_CurrentRow]);

    uint8_t changedButtons = curButtonState ^ m_ButtonStates;
    for (uint8_t i = 0 ; i < 6 ; ++i)
    {
        if (changedButtons & BIT8(i,1))
        {
            if (curButtonState & BIT8(i,1))
            {
                m_LastPressedKey = i;
                m_NextRepeatTime = Clock::GetTime() + (uint16_t(Config::GetKeyRepeatDelay()) << 2);
                EventSys::AddEvent(Event(EventID::e_KeyDown, i));
                Beeper::Beep(BeepID::e_KeyPress);
//                printf_P(PSTR("%d pressed\n"), i);
            }
            else
            {
                if (m_LastPressedKey == i) m_LastPressedKey = -1;
                EventSys::AddEvent(Event(EventID::e_KeyUp, i));
//                printf_P(PSTR("%d released\n"), i);
            }
        }
    }
    m_ButtonStates = curButtonState;
        
    if ( m_LastPressedKey != -1 && I32(m_NextRepeatTime - Clock::GetTime()) <= 0 )
    {
        m_NextRepeatTime = Clock::GetTime() + (uint16_t(Config::GetKeyRepeatSpeed()) << 2);
        EventSys::AddEvent(Event(EventID::e_KeyRepeat, m_LastPressedKey));
    }

    
    // Read rotary encoder:
    uint8_t curCount = U8(QUAD_DECODER_TIMER.CNT >> 2); // The rotary encoder have 4 state changes per indentation. So divide by 4.
    
    int8_t rotaryDelta = curCount - m_PrevQuadDecoderValue;
    m_PrevQuadDecoderValue = curCount;
    
    if (rotaryDelta != 0)
    {
        Event* peekEvent = EventSys::PeekEvent();
        if ( peekEvent != nullptr && peekEvent->type == EventID::e_KnobTurned )
        {
            peekEvent->value += rotaryDelta;
        } else {
            EventSys::AddEvent(Event(EventID::e_KnobTurned, rotaryDelta));
        }
        Beeper::Beep(BeepID::e_Knob);
    }            
}

const char* Keyboard::GetKeyName(KbdKeyCodes::Enum key)
{
        switch(key)
        {
            case KbdKeyCodes::e_KnobButton: return PSTR("Knob");
            case KbdKeyCodes::e_Up:         return PSTR("Up");
            case KbdKeyCodes::e_Down:       return PSTR("Down");
            case KbdKeyCodes::e_Left:       return PSTR("Left");
            case KbdKeyCodes::e_Right:      return PSTR("Right");
            case KbdKeyCodes::e_Menu:       return PSTR("Menu");
            default: return nullptr;
        }
}

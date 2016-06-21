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
#include <string.h>

#include "TextInput.h"
#include "EventSys.h"
#include "Beeper.h"
#include "Keyboard.h"

#include "Misc/Utils.h"
#include "Misc/Clock.h"

//static char g_SymbolMapping[] PROGMEM = { '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', ':', ';', '<', '=', '>', '?', '@', '[', '\\', ']', '^', '_', , '`', '{', '|', '}', '\x7E', '\x7F' };
static const char g_SymbolMapping[] PROGMEM = { ':', ';', ',', '.', '+', '-', '!', '?', '(', ')', '[', ']', '{', '}', '<', '>', '\'', '"', '\\', '/', '_', '#', '$', '%', '&', '*', '=', '@', '|', '`', '^', '\x7E', '\x7F' }; // 33
static const uint8_t g_CharGroups[] PROGMEM = {28, 66, 106, 146, 164};

static const uint8_t CHAR_GROUP_COUNT = 5;
static uint8_t GetGroupStart(int8_t group) { return (group == 0) ? 0 : pgm_read_byte_near(&g_CharGroups[group - 1]); }
static uint8_t GetGroupSize(int8_t group ) { return (group == 0) ? pgm_read_byte_near(&g_CharGroups[0]) : (pgm_read_byte_near(&g_CharGroups[group]) - pgm_read_byte_near(&g_CharGroups[group - 1])); }

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

char CharIndexToChar(uint8_t index, bool upperCase)
{
    if ( index < 28 ) {
        if (index == 0) {
            return '\x7f'; // BACKSPACE
        } else if (index == 14) {
            return ' ';
        }
        index -= (index < 14) ? 1 : 2;
        return ((upperCase) ? 'A' : 'a') + index;   // Alpha
    } else if ( index < 38 ) {
        return '0' + index - 28;                    // Numeric
    } else if ( index < 71 ) {
        return pgm_read_byte_near(&g_SymbolMapping[index - 38]); // Symbol1
    } else if ( index < 164 ) {
        return index - 71 + 163;                    // Symbol2
    } else {
        return ' ';
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TextInput::Initialize()
{
    m_MaxLength = 0;
    m_KnobDownTime = 0;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TextInput::BeginEdit(const char* initialString, int8_t maxLength, bool passwordMode)
{
    m_KnobDownTime = 0;
    m_CurrentRange = 0;
    m_Uppercase = true;
    m_CurrentChar = 0;

    m_MaxLength = min(MAX_TEXT_LENGTH, maxLength);

    
    if (initialString != nullptr && initialString[0] != '\0') {
        m_PasswordMode = passwordMode;
        strncpy(m_Buffer, initialString, m_MaxLength);
        m_CurrentLength = strnlen(m_Buffer, m_MaxLength);
        SetCursorPos(m_CurrentLength);
    } else {
        m_PasswordMode = false;
        m_CurrentLength = 0;
        SetCursorPos(0);
    }
    

    g_Display.WriteCustomCharBegin(6);
    g_Display.WriteCustomCharRow(CB_00100);
    g_Display.WriteCustomCharRow(CB_01010);
    g_Display.WriteCustomCharRow(CB_10001);
    g_Display.WriteCustomCharRow(CB_00000);
    g_Display.WriteCustomCharRow(CB_00000);
    g_Display.WriteCustomCharRow(CB_00000);
    g_Display.WriteCustomCharRow(CB_00000);
    g_Display.WriteCustomCharRow(CB_00000);

    g_Display.WriteCustomCharBegin(7);
    g_Display.WriteCustomCharRow(CB_00000);
    g_Display.WriteCustomCharRow(CB_00000);
    g_Display.WriteCustomCharRow(CB_00000);
    g_Display.WriteCustomCharRow(CB_00000);
    g_Display.WriteCustomCharRow(CB_00000);
    g_Display.WriteCustomCharRow(CB_10001);
    g_Display.WriteCustomCharRow(CB_01010);
    g_Display.WriteCustomCharRow(CB_00100);
    
    g_Display.ClearDisplay();
/*    for ( int8_t i = 0 ; i < 20 && i < m_MaxLength && m_Buffer[i] != 0 ; ++i )
    {
        g_Display.WriteCharacter(m_Buffer[i]);
        m_CurrentLength++;
    }*/
    UpdateStringDisplay();
    PrintCharSet();
    PrintCharSelectCursor(0, false);
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TextInput::EndEdit()
{
    CursorOff();
    if ( m_CurrentLength < m_MaxLength ) m_Buffer[m_CurrentLength] = '\0';
    m_MaxLength = 0;
    m_CursorPos = 0;
    m_ScrollOffset = 0;
    m_CurrentChar = 0;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

bool TextInput::IsEditing() const
{
    return m_Buffer !=  nullptr;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

bool TextInput::HandleEvent(const Event& event)
{
    if ( m_KnobDownTime != 0 && Clock::GetTime() > m_KnobDownTime + 300 )
    {
        Beeper::Beep(BeepID::e_LongPress);
        m_KnobDownTime = 0;

        int8_t lineLength = (GetGroupSize(m_CurrentRange) + 1) >> 1;
        if ( m_CurrentChar < GetGroupStart(m_CurrentRange) + lineLength )
        {
            SetCurrentChar(m_CurrentChar + lineLength);
        }
        else if ( m_CurrentChar >= GetGroupStart(m_CurrentRange) + lineLength )
        {
            SetCurrentChar(m_CurrentChar - lineLength);
        }
    }
    bool keyRepeat = false;
    switch(event.type)
    {
        case EventID::e_KeyRepeat:
            keyRepeat = true;
            // FALL THROUGH //
        case EventID::e_KeyDown:
/*            if ( !keyRepeat && event.value >= KbdKeyCodes::e_Preset1 && event.value <= KbdKeyCodes::e_Preset5 )
            {
                int8_t range = event.value - KbdKeyCodes::e_Preset1;
                if ( range != m_CurrentRange )
                {
                    SetRange(range);
                }
                else
                {
                    if ( m_CurrentRange == 0 ) {
                        SetUpperCase(!m_Uppercase);
                    }
                }
                return true;
            }*/

            switch( event.value )
            {
/*                case KbdKeyCodes::e_Preset8:
                    if ( !keyRepeat )
                    {
                        int8_t lineLength = (GetGroupSize(m_CurrentRange) + 1) >> 1;
                        if ( m_CurrentChar < GetGroupStart(m_CurrentRange) + lineLength ) {
                            SetCurrentChar(m_CurrentChar + lineLength);
                        } else if ( m_CurrentChar >= GetGroupStart(m_CurrentRange) + lineLength ) {
                            SetCurrentChar(m_CurrentChar - lineLength);
                        }
                    }
                    return true;*/
                case KbdKeyCodes::e_Up:
                    if (m_CurrentRange > 0) {
                        SetRange(m_CurrentRange - 1);
                    } else if (!m_Uppercase) {
                        SetUpperCase(true);
                    }
                    return true;
                case KbdKeyCodes::e_Down:
                    if (m_CurrentRange == 0 && m_Uppercase) {
                        SetUpperCase(false);                        
                    } else if (m_CurrentRange < CHAR_GROUP_COUNT - 1) {
                        SetRange(m_CurrentRange + 1);
                    }
                    return true;
                case KbdKeyCodes::e_KnobButton:
                    if ( !keyRepeat ) {
                        printf_P(PSTR("Knob down\n"));
                        m_KnobDownTime = Clock::GetTime();
                    }                        
                    return true;
/*                case KbdKeyCodes::e_Start:
                    InsertChar(' ');
                    if ( keyRepeat ) Beeper::Beep(BeepID::e_KeyRepeat);
                    return true;
                case KbdKeyCodes::e_Stop:
                    if ( m_CursorPos == m_CurrentLength - 1 || MoveCursor(-1) ) {
                        DeleteChar();
                    }
                    if ( keyRepeat ) Beeper::Beep(BeepID::e_KeyRepeat);
                    return true;*/
                case KbdKeyCodes::e_Right:
                    MoveCursor(1);
                    if ( keyRepeat ) Beeper::Beep(BeepID::e_KeyRepeat);
                    return true;
                case KbdKeyCodes::e_Left:
                    MoveCursor(-1);
                    if ( keyRepeat ) Beeper::Beep(BeepID::e_KeyRepeat);
                    return true;
/*                case KbdKeyCodes::e_Back:
                    if ( !keyRepeat ) {
                        EventSys::AddEvent(Event(EventID::e_TextEditCanceled, 0));
                    }                        
                    return true;*/
                case KbdKeyCodes::e_Menu:
                    if ( !keyRepeat ) {
                        if (m_CurrentLength < m_MaxLength) m_Buffer[m_CurrentLength] = '\0';
                        CursorOff();
                        EventSys::AddEvent(Event(EventID::e_TextEditDone, m_CursorPos));
                    }                        
                    return true;
            }
            return false;
        case EventID::e_KeyUp:
            switch( event.value )
            {
                case KbdKeyCodes::e_KnobButton:
                    if ( m_KnobDownTime != 0 )
                    {
                        printf_P(PSTR("Knob up: %d (%ld)\n"), m_CurrentChar, m_KnobDownTime);
                        if (m_CurrentChar != 0) {
                            InsertChar(CharIndexToChar(m_CurrentChar, m_Uppercase));
                        } else {
                            if ( m_CursorPos == m_CurrentLength - 1 || MoveCursor(-1) ) {
                                DeleteChar();
                            }
                            if ( keyRepeat ) Beeper::Beep(BeepID::e_KeyRepeat);                            
                        }                            
                    }
                    m_KnobDownTime = 0;
                    return true;
            }
            return false;                    
        case EventID::e_KnobTurned:
        {
            if ( -event.value > m_CurrentChar ) {
                SetCurrentChar(0);
            } else {
                SetCurrentChar(m_CurrentChar + event.value);
            }                
            
            return true;
        }            
        default:
            return false;
    }    
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

bool TextInput::MoveCursor(int8_t delta)
{
    return SetCursorPos(m_CursorPos + delta);
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

bool TextInput::SetCursorPos(int8_t pos)
{
    if ( pos < 0 ) {
        pos = 0;
    } else if ( pos > m_CurrentLength ) {
        pos = m_CurrentLength;
    }
    
    if ( pos != m_CursorPos )
    {
        m_CursorPos = pos;
        
        if (m_CursorPos - m_ScrollOffset < 0) {
            m_ScrollOffset = m_CursorPos;
            UpdateStringDisplay();
        } else if (m_CursorPos - m_ScrollOffset >= 19) {
            m_ScrollOffset = m_CursorPos - 19;
            UpdateStringDisplay();
        } else {
            g_Display.SetCursor(m_CursorPos - m_ScrollOffset, 0);
        }            
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

int8_t TextInput::GetCharCursorPos(bool* pointDown) const
{
    int8_t lineLength = (GetGroupSize(m_CurrentRange) + 1) >> 1;
    int8_t pos = m_CurrentChar - GetGroupStart(m_CurrentRange);
    if ( pos < lineLength ) {
        if ( pointDown != nullptr ) {
            *pointDown = false;
        }            
    } else {
        if ( pointDown != nullptr ) {
            *pointDown = true;
        }            
        pos -= lineLength;
    }
    return pos;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TextInput::SetRange(int8_t range)
{
    if ( range != m_CurrentRange )
    {
        bool prevPointDown;
        int8_t prevCursorPos = GetCharCursorPos(&prevPointDown);
        
        m_CurrentRange = range;
        
        // Place char select cursor at the same location within the new range or at the end if out of range.
        int8_t lineLength = (GetGroupSize(m_CurrentRange) + 1) >> 1;
        m_CurrentChar = GetGroupStart(m_CurrentRange) + min<int8_t>(lineLength - 1, prevCursorPos);
        if ( prevPointDown ) m_CurrentChar += lineLength;
        
        bool pointDown;
        int8_t cursorPos = GetCharCursorPos(&pointDown);
        
        CursorOff();
        if (prevCursorPos != cursorPos) {
            g_Display.SetCursor(prevCursorPos, 2);
            g_Display.WriteCharacter(' ');
        }
        PrintCharSelectCursor(cursorPos, pointDown);
        PrintCharSet(); // Will turn cursor on.
    }    
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TextInput::SetUpperCase(bool upperCase)
{
    m_Uppercase = !m_Uppercase;
    PrintCharSet();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TextInput::UpdateStringDisplay()
{
    CursorOff();
    g_Display.SetCursor(0, 0);
    if (!m_PasswordMode)
    {
        for (int8_t i = m_ScrollOffset ; i - m_ScrollOffset < 20 && i < m_CurrentLength ; ++i) {
            g_Display.WriteCharacter(m_Buffer[i]);
        }
    }
    else
    {
        g_Display.Printf_P(PSTR("*******"));
    }
    g_Display.ClearToEndOfLine();
    g_Display.SetCursor(m_CursorPos - m_ScrollOffset, 0);
    CursorOn();
    
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TextInput::PrintCharSelectCursor(int8_t pos, bool down)
{
    g_Display.SetCursor(pos, 2);
    g_Display.WriteCharacter((down) ? 7 : 6);
    g_Display.SetCursor(m_CursorPos - m_ScrollOffset, 0);
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TextInput::SetCurrentChar(uint8_t curChar)
{
    if ( curChar >= GetGroupStart(ARRAY_COUNT(g_CharGroups)) ) {
        curChar = GetGroupStart(ARRAY_COUNT(g_CharGroups)) - 1;
    }
    if ( curChar != m_CurrentChar )
    {
        int8_t prevCursorPos = GetCharCursorPos(nullptr);
        int8_t prevRange = m_CurrentRange;
        m_CurrentChar = curChar;

        while(m_CurrentChar < GetGroupStart(m_CurrentRange) ) m_CurrentRange--;
        while(m_CurrentChar >= GetGroupStart(m_CurrentRange) + GetGroupSize(m_CurrentRange) ) m_CurrentRange++;
        
        if ( m_CurrentRange != prevRange ) {
            PrintCharSet();
        }            

        bool pointDown;
        int8_t cursorPos = GetCharCursorPos(&pointDown);

        CursorOff();        
        if (prevCursorPos != cursorPos) {
            g_Display.SetCursor(prevCursorPos, 2);
            g_Display.WriteCharacter(' ');
        }
        PrintCharSelectCursor(cursorPos, pointDown);
        CursorOn();
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TextInput::InsertChar(char character)
{
    if (m_PasswordMode) {
        printf_P(PSTR("Insert clear initial\n"));
        m_Buffer[0] = '\0';
        m_CurrentLength = 0;
        m_PasswordMode = false;
        m_CursorPos = 0;
    }        
    if ( m_CurrentLength < m_MaxLength )
    {
        memmove(&m_Buffer[m_CursorPos + 1], &m_Buffer[m_CursorPos], m_CurrentLength - m_CursorPos);
        m_Buffer[m_CursorPos] = character;
        m_CurrentLength++;
        m_CursorPos++;

        if (m_CursorPos - m_ScrollOffset >= 19)
        {
            m_ScrollOffset = m_CursorPos - 19;
            UpdateStringDisplay();
        }
        else
        {
            CursorOff();
            for (int8_t i = m_CursorPos - 1; i - m_ScrollOffset < 20 && i < m_CurrentLength ; ++i)
            {
                g_Display.WriteCharacter(m_Buffer[i]);
            }
            g_Display.SetCursor(m_CursorPos - m_ScrollOffset, 0);
            CursorOn();
        }            
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TextInput::DeleteChar()
{
    if (m_PasswordMode) {
        return;
/*        printf_P(PSTR("Delete clear initial\n"));
        m_Buffer[0] = '\0';
        m_CurrentLength = 0;
        m_PasswordMode = false;*/
    }
    if ( m_CursorPos < m_CurrentLength )
    {
        memmove(&m_Buffer[m_CursorPos], &m_Buffer[m_CursorPos + 1], m_CurrentLength - m_CursorPos - 1);
        m_CurrentLength--;
        
        CursorOff();
        for ( int8_t i = m_CursorPos ; i - m_ScrollOffset < 20 && i < m_CurrentLength ; ++i ) {
            g_Display.WriteCharacter(m_Buffer[i]);
        }
        g_Display.ClearToEndOfLine();
        g_Display.SetCursor(m_CursorPos - m_ScrollOffset, 0);
        CursorOn();
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TextInput::PrintCharSet()
{
    uint8_t rangeStart = GetGroupStart(m_CurrentRange);
    int8_t  rangeSize  = GetGroupSize(m_CurrentRange);
    int8_t  lineLength = (rangeSize + 1) >> 1;

    CursorOff();
    g_Display.SetCursor(0, 1);    
    for (int8_t i = 0 ; i < rangeSize ; ++i )
    {
        if ( i == lineLength ) {
            g_Display.ClearToEndOfLine();
            g_Display.SetCursor(0, 3);
        }
        g_Display.WriteCharacter(CharIndexToChar(rangeStart + i, m_Uppercase));
    }
    g_Display.ClearToEndOfLine();

    g_Display.SetCursor(m_CursorPos - m_ScrollOffset, 0);
    CursorOn();
}


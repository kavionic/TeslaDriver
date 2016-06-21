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


#ifndef __TEXTINPUT_H__
#define __TEXTINPUT_H__


#include <avr/io.h>

#include "Hardware.h"

#include "Misc/Display.h"

struct Event;

class TextInput
{
public:
    static const uint8_t MAX_TEXT_LENGTH = 128;
    void Initialize();
    const char* GetBuffer() const { return m_Buffer; }
    void BeginEdit(const char* initialString, int8_t maxLength, bool passwordMode);
    void EndEdit();
    bool IsEditing() const;

    
    bool HandleEvent(const Event& event);
    
private:
    void PrintCharSet();
    
    bool MoveCursor(int8_t delta);
    bool SetCursorPos(int8_t pos);
    void InsertChar(char character);
    void DeleteChar();
    
    void   CursorOff() { g_Display.EnableDisplay(true, false, false); }
    void   CursorOn()  { g_Display.EnableDisplay(true, true, false); }

    void   UpdateStringDisplay();
    void   PrintCharSelectCursor(int8_t pos, bool down);
    int8_t GetCharCursorPos(bool* pointDown) const;
    void   SetRange(int8_t range);
    void   SetUpperCase(bool upperCase);
    void   SetCurrentChar(uint8_t curChar);
    
    
    char     m_Buffer[MAX_TEXT_LENGTH];
    int8_t   m_MaxLength;
    bool     m_PasswordMode;
    int8_t   m_CurrentLength;
    int8_t   m_CursorPos;
    int8_t   m_ScrollOffset;
    uint8_t  m_CurrentChar;
    bool     m_Uppercase;
    uint32_t m_KnobDownTime;
    uint8_t  m_CurrentRange;
};

#endif //__TEXTINPUT_H__

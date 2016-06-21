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


#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <stdio.h>

namespace KbdKeyCodes
{
    enum Enum
    {
        e_KnobButton = 0,
        e_Up         = 4,
        e_Down       = 1,
        e_Left       = 2,
        e_Right      = 3,
        e_Menu       = 5
    };
}


class Keyboard
{
public:
    void Initialize();    
    void Run();
    
    static const char* GetKeyName(KbdKeyCodes::Enum key);
    
private:
    uint32_t m_NextRepeatTime;
    uint8_t  m_ButtonStates;
    int8_t   m_LastPressedKey;
    uint8_t  m_PrevQuadDecoderValue;
    int8_t   m_CurrentRow;
    int8_t   m_CurrentButton;
};

#endif //__KEYBOARD_H__

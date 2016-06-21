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


#ifndef __HOTSPOTMENU_H__
#define __HOTSPOTMENU_H__

#include "ScrollMenu.h"
#include "TextInput.h"

class HotspotMenu
{
public:
    void Initialize();
    void Shutdown() {}
    void HandleSelect(int8_t entry, bool isEditing);
    bool Run(const Event& event);

private:
    enum State_e
    {
        e_StateMain,
        e_StateEditSSID,
        e_StateEditPassword
    } m_State;
    
    static int8_t PrintMenuValue(void* userData, int8_t item, bool isSelected);
    ScrollMenu m_Menu;
    TextInput  m_TextInput;
    int8_t     m_CurrentChannel;
};

#endif //__HOTSPOTMENU_H__

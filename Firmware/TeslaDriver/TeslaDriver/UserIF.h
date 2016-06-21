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


#ifndef __USERIF_H__
#define __USERIF_H__

#include "InfoViewer.h"
#include "MainMenu.h"
#include "AccessPointSelector.h"
#include "HotspotMenu.h"
#include "TextInput.h"

class Event;

namespace UIState
{
    enum Enum
    {
        e_StateInfo,
        e_StateMainMenu,
        e_StateAccesPointSelector,
        e_StateHotspotMenu,
        e_StatePasswordInput,
        e_StateCount
    };
}

class UserIF
{
public:
    void Initialize();
    void Run(const Event& event);
    
private:
    union
    {
        InfoViewer          m_InfoViewer;
        MainMenu            m_MainMenu;
        AccessPointSelector m_AccessPointSelector;
        HotspotMenu         m_HotspotMenu;
        TextInput           m_TextInput;
    } m_Menus;
    
    UIState::Enum m_State = UIState::e_StateInfo;
    UIState::Enum m_PrevState = UIState::e_StateCount;
    int8_t        m_NewAccessPointIndex = -1;
};

extern UserIF g_UI;

#endif //__USERIF_H__

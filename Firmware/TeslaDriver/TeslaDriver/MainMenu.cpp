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

#include "MainMenu.h"
#include "UserIF.h"
#include "EventSys.h"

#include "Misc/Utils.h"

enum
{
    e_MenuConnectToAP,
    e_MenuSetupHotspot,
    e_MenuSetupInverter,
    e_MenuBack
};

///////////////////////////////////////////">1234567890123456789"
static const char g_MainMenu0[] PROGMEM = "Connect to network";
static const char g_MainMenu1[] PROGMEM = "Setup hotspot";
static const char g_MainMenu2[] PROGMEM = "Setup inverter";
static const char g_MainMenu3[] PROGMEM = "Back";

static const char* const g_MainMenuEntries[] PROGMEM = {g_MainMenu0, g_MainMenu1, g_MainMenu2, g_MainMenu3};

int8_t MainMenu::s_Selection;

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void MainMenu::Initialize()
{
    m_Menu.SetMenu(g_MainMenuEntries, ARRAY_COUNT(g_MainMenuEntries), nullptr, nullptr, 0, 3);
    m_Menu.SelectItem(s_Selection);
}

bool MainMenu::Run(const Event& event)
{
    switch(event.type)
    {
        case EventID::e_MenuSelected:
            if ( event.value != -1 ) s_Selection = event.value;
            switch(event.value)
            {
                case e_MenuConnectToAP:
                    printf_P(PSTR("MainMenu::Run() start AP selector\n"));
                    EventSys::AddEvent(Event(EventID::e_SetUIState, UIState::e_StateAccesPointSelector));
                    break;
                case e_MenuSetupHotspot:
                    EventSys::AddEvent(Event(EventID::e_SetUIState, UIState::e_StateHotspotMenu));
                    break;
                case e_MenuSetupInverter:
                    //EventSys::AddEvent(Event(EventID::SetUIState, UIState::e_StateSetupInverter));
                    break;
                case e_MenuBack:
                case -1:
                    EventSys::AddEvent(Event(EventID::e_SetUIState, UIState::e_StateInfo));
                break;
            }
            return true;
        default:
            return m_Menu.HandleEvent(event);
    }
    return false;
}



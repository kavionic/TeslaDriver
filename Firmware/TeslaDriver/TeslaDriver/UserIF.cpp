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

#include <stdio.h>
#include <string.h>

#include <avr/pgmspace.h>

#include "UserIF.h"
#include "NetIF.h"
#include "Hardware.h"
#include "EventSys.h"
#include "Keyboard.h"
#include "Config.h"

#include "Misc/Clock.h"
#include "Misc/Utils.h"


///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void UserIF::Initialize()
{
    m_Menus.m_InfoViewer.Initialize();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void UserIF::Run(const Event& event)
{
    bool eventHandled = false;
 
    bool stateChanged = m_State != m_PrevState;
    m_PrevState = m_State;
 
    if (stateChanged) {
        g_Display.ClearDisplay();
    }
 
    switch(m_State)
    {
        case UIState::e_StateInfo:
            if (stateChanged) m_Menus.m_InfoViewer.Initialize();
            eventHandled = m_Menus.m_InfoViewer.Run(event);
            break;
        case UIState::e_StateMainMenu:
            if (stateChanged) m_Menus.m_MainMenu.Initialize();
            eventHandled = m_Menus.m_MainMenu.Run(event);
            break;
        case UIState::e_StateAccesPointSelector:
            if (stateChanged) {
                printf_P(PSTR("UserIF::Run() start AP selector\n"));
                g_NetIF.Stop();
                m_Menus.m_AccessPointSelector.Initialize();
            }
            eventHandled = m_Menus.m_AccessPointSelector.Run(event);
            break;
        case UIState::e_StateHotspotMenu:
            if (stateChanged) m_Menus.m_HotspotMenu.Initialize();
            eventHandled = m_Menus.m_HotspotMenu.Run(event);
            break;
        case UIState::e_StatePasswordInput:
            if (stateChanged) {
                const WifiStoredAccessPoint* accessPoint = Config::GetAccessPoint(m_NewAccessPointIndex);
                m_Menus.m_TextInput.BeginEdit(accessPoint->m_Password, WIFI_MAX_PASSWD_LEN, true);
            }                
            eventHandled = m_Menus.m_TextInput.HandleEvent(event);
            break;
        case UIState::e_StateCount:
            break;
    }
    
    if (!eventHandled)
    {
        switch(event.type)
        {
            case EventID::e_KeyDown:
                switch(event.value)
                {
                    case KbdKeyCodes::e_Menu:
                    case KbdKeyCodes::e_KnobButton:
                        m_State = UIState::e_StateMainMenu;
                        break;
                    case KbdKeyCodes::e_Up:
                        m_State = UIState::e_StateAccesPointSelector;
                        break;
                    case KbdKeyCodes::e_Down:
                        m_State = UIState::e_StateHotspotMenu;
                        break;
                }                    
                break;
            case EventID::e_KeyRepeat:
                break;
            case EventID::e_KeyUp:
                break;
            case EventID::e_KnobTurned:
                break;
            case EventID::e_SetUIState:
                m_State = UIState::Enum(event.value);
                break;
            case EventID::e_AccessPointSelected:
                if (m_State == UIState::e_StateAccesPointSelector)
                {
                    const WifiAccessPoint* selectedAP = m_Menus.m_AccessPointSelector.GetSelectedAccessPoint();
                    if (selectedAP != nullptr)
                    {
                        printf_P(PSTR("Selected ap: '%s'\n"), selectedAP->m_SSID);
                        m_NewAccessPointIndex = Config::FindAccessPoint(selectedAP->m_SSID);
                        if (m_NewAccessPointIndex != -1)
                        {
                            printf_P(PSTR("Stored PW: '%s'\n"), Config::GetAccessPoint(m_NewAccessPointIndex)->m_Password);
                            m_State = UIState::e_StatePasswordInput;
                        }
                        else
                        {
                            m_NewAccessPointIndex = Config::NewAccessPoint(selectedAP->m_SSID);
                            m_State = UIState::e_StatePasswordInput;
                        }
                    } else {
                        g_NetIF.ReconfigureRadio();
                        m_State = UIState::e_StateInfo;
                    }
                }
                break;
            case EventID::e_TextEditDone:
                printf_P(PSTR("PW: '%s'\n"), m_Menus.m_TextInput.GetBuffer());
                if (m_State == UIState::e_StatePasswordInput)
                {
                    if (m_NewAccessPointIndex != -1)
                    {
                        Config::SetAccessPointPassword(m_NewAccessPointIndex, m_Menus.m_TextInput.GetBuffer());
                        Config::SetSelectedAccessPoint(m_NewAccessPointIndex);
                        Config::SetHotspotEnabled(false);
                        g_NetIF.ReconfigureRadio();
                    }
                }
                m_State = UIState::e_StateInfo;
                break;
            default:
                break;
        }        
    }
 }



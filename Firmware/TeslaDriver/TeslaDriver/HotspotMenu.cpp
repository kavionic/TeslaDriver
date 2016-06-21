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

#include "HotspotMenu.h"
#include "UserIF.h"
#include "NetIF.h"
#include "EventSys.h"
#include "Config.h"

#include "Misc/Utils.h"

enum MenuEntries_e
{
    e_MenuEnabled,
    e_MenuSSID,
    e_MenuChannel,
    e_MenuSecurity,
    e_MenuPassword,
    e_MenuHidden,
    e_MenuBack
};

static const char g_EncryptionNames[5][13] PROGMEM = { "Open", "WEP", "WPA-PSK", "WPA2-PSK", "WPA/WPA2-PSK"};

////////////////////////////////////////////">1234567890123456789"
static const char g_HotspotMenu0[] PROGMEM = "Enabled";
static const char g_HotspotMenu1[] PROGMEM = "Name";
static const char g_HotspotMenu2[] PROGMEM = "Channel";
static const char g_HotspotMenu3[] PROGMEM = "Secure";
static const char g_HotspotMenu4[] PROGMEM = "Password";
static const char g_HotspotMenu5[] PROGMEM = "Hidden";
static const char g_HotspotMenu6[] PROGMEM = "Back";

static const char* const g_HotspotMenuEntries[] PROGMEM = {g_HotspotMenu0, g_HotspotMenu1, g_HotspotMenu2, g_HotspotMenu3, g_HotspotMenu4, g_HotspotMenu5, g_HotspotMenu6};

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void HotspotMenu::Initialize()
{
    m_State = e_StateMain;
    m_CurrentChannel = Config::GetHotspotChannel();
    m_Menu.SetMenu(g_HotspotMenuEntries, ARRAY_COUNT(g_HotspotMenuEntries), PrintMenuValue, this, 0, 3);
}

void HotspotMenu::HandleSelect(int8_t entry, bool isEditing)
{
    switch(entry)
    {
        case e_MenuEnabled:
            Config::SetHotspotEnabled(!Config::IsHotspotEnabled());
            m_Menu.UpdateItem(entry, -1);
            g_NetIF.ReconfigureRadio();
            break;
        case e_MenuSSID:
            m_State = e_StateEditSSID;
            m_TextInput.BeginEdit(Config::GetHotspotSSID(), WIFI_MAX_SSID_LEN, false);
            break;
        case e_MenuChannel:
            if (!isEditing) {
                m_Menu.EditItem(true, -1);
            } else {
                m_Menu.EditItem(false, -1);
                Config::SetHotspotChannel(m_CurrentChannel);
            }                        
            break;
        case e_MenuSecurity:
            if (Config::GetHotspotEncryption() >= WifiSecurityMode_e::e_OPEN && Config::GetHotspotEncryption() < WifiSecurityMode_e::e_Count - 1) {
                Config::SetHotspotEncryption(WifiSecurityMode_e::Enum(Config::GetHotspotEncryption() + 1));
            } else {
                Config::SetHotspotEncryption(WifiSecurityMode_e::e_OPEN);
            }
            m_Menu.UpdateItem(entry, -1);
            break;
        case e_MenuPassword:
            m_State = e_StateEditPassword;
            m_TextInput.BeginEdit(Config::GetHotspotPassword(), WIFI_MAX_PASSWD_LEN, false);
            break;
        case e_MenuHidden:
            Config::SetHotspotHidden(!Config::IsHotspotHidden());
            m_Menu.UpdateItem(entry, -1);
            break;
        case e_MenuBack:
        case -1:
            g_NetIF.ReconfigureRadio();
            EventSys::AddEvent(Event(EventID::e_SetUIState, UIState::e_StateInfo));
            break;
    }
    
}
///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

bool HotspotMenu::Run(const Event& event)
{
    if (m_State != e_StateMain)
    {
        if (m_TextInput.HandleEvent(event)) {
            return true;
        }
    }
    switch(event.type)
    {
        case EventID::e_MenuSelected:
            HandleSelect(event.value, m_Menu.IsEditing());
            return true;
        case EventID::e_MenuItemEdited:
            if (m_Menu.GetSelectedItem() == e_MenuChannel) {
                m_CurrentChannel = clamp(1, 13, m_CurrentChannel + event.value);
                m_Menu.UpdateItem(e_MenuChannel, -1);
            }                
            return true;
        case EventID::e_TextEditDone:
            if(m_State == e_StateEditSSID) {
                Config::SetHotspotSSID(m_TextInput.GetBuffer());
            } else if(m_State == e_StateEditPassword) {
                Config::SetHotspotPassword(m_TextInput.GetBuffer());
            }
            m_State = e_StateMain;
            m_Menu.Print();
            return true;
            
        default:
            return m_Menu.HandleEvent(event);
    }
    return false;
}

int8_t HotspotMenu::PrintMenuValue(void* userData, int8_t item, bool isSelected)
{
    HotspotMenu* self = static_cast<HotspotMenu*>(userData);
    
//    g_Display.ClearTo(11);
    switch(item)
    {
        case e_MenuEnabled:
        {
            const char* str = (Config::IsHotspotEnabled()) ? (PSTR("YES")) : (PSTR("NO"));
            g_Display.ClearTo(20 - strlen_P(str));
            return g_Display.Printf_P(str);
        }
        case e_MenuSSID:        
            return g_Display.Printf_P(PSTR("%14s"), Config::GetHotspotSSID());
        case e_MenuChannel:
        {
            int8_t strLen = 1;
            if (isSelected) strLen += 2;
            if (self->m_CurrentChannel > 9) strLen++;
            g_Display.ClearTo(20 - strLen);
            return g_Display.Printf_P(((isSelected) ? PSTR("[%d]") : PSTR("%d")), self->m_CurrentChannel);
        }
        case e_MenuSecurity:
            if (Config::GetHotspotEncryption() >= WifiSecurityMode_e::e_OPEN && Config::GetHotspotEncryption() < WifiSecurityMode_e::e_Count) {
                g_Display.ClearTo(20 - strlen_P(g_EncryptionNames[Config::GetHotspotEncryption()]));
                return g_Display.Printf_P(g_EncryptionNames[Config::GetHotspotEncryption()]);
            } else {
                return 0;
            }                
        case e_MenuPassword:
            g_Display.ClearTo(20 - strlen("****"));
            return g_Display.Printf_P(PSTR("****"));
        case e_MenuHidden:
        {
            const char* str = (Config::IsHotspotHidden()) ? (PSTR("YES")) : (PSTR("NO"));
            g_Display.ClearTo(20 - strlen_P(str));
            return g_Display.Printf_P(str);
        }            
    }
    return 0;
}


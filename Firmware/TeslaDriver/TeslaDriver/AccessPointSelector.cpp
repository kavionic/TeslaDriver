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
#include <stdlib.h>
#include <string.h>

#include "AccessPointSelector.h"
#include "EventSys.h"
#include "Hardware.h"
#include "ESP8266/ESP8266.h"
#include "Misc/Utils.h"
#include "Keyboard.h"

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void AccessPointSelector::Initialize()
{
    m_ScrollOffset = 0;
    
    g_Display.WriteCustomCharBegin(4);
    g_Display.WriteCustomCharRow(CB_01110);
    g_Display.WriteCustomCharRow(CB_00010);
    g_Display.WriteCustomCharRow(CB_00010);
    g_Display.WriteCustomCharRow(CB_11111);
    g_Display.WriteCustomCharRow(CB_11011);
    g_Display.WriteCustomCharRow(CB_11011);
    g_Display.WriteCustomCharRow(CB_11111);
    g_Display.WriteCustomCharRow(CB_00000);
    
    g_Display.WriteCustomCharBegin(5);
    g_Display.WriteCustomCharRow(CB_00000);
    g_Display.WriteCustomCharRow(CB_01110);
    g_Display.WriteCustomCharRow(CB_01010);
    g_Display.WriteCustomCharRow(CB_11111);
    g_Display.WriteCustomCharRow(CB_11011);
    g_Display.WriteCustomCharRow(CB_11011);
    g_Display.WriteCustomCharRow(CB_11111);
    g_Display.WriteCustomCharRow(CB_00000);
    
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
    
    g_Display.SetCursor(4, 0);
    g_Display.Printf_P(PSTR("Discovering"));
    g_Display.SetCursor(3, 1);
    g_Display.Printf_P(PSTR("Access Points"));
    printf_P(PSTR("Requesting AP\n"));
    g_WifiDevice.SetWifiMode(WifiMode_e::e_Station);
    m_AccessPointCount = g_WifiDevice.DiscoverAccessPoints(m_AccessPoints, MAX_ACCESS_POINTS);
    //m_AccessPoints[0].m_Security = WifiSecurityMode_e::e_OPEN;
    if (m_AccessPointCount >0)
    {
        qsort(m_AccessPoints, m_AccessPointCount, sizeof(WifiAccessPoint), [](const void* lhs, const void* rhs) {return int(((WifiAccessPoint*)rhs)->m_Strength - ((WifiAccessPoint*)lhs)->m_Strength);});
        printf_P(PSTR("Received %d APs\n"), m_AccessPointCount);
        m_FirstVisibleItem = 0;
        m_SelectedItem     = 0;
        Print();
    }        
    else
    {
        g_Display.SetCursor(1, 0);
        g_Display.Printf_P(PSTR("Failed to discover"));
        g_Display.SetCursor(3, 1);
        g_Display.Printf_P(PSTR("Access Points"));
        m_SelectedItem     = -1;
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

bool AccessPointSelector::Run(const Event& event)
{
    bool eventHandled = false;
    if (event.type == EventID::e_TimeTick1000)
    {
        m_ScrollOffset++;
        Print();
    }    
    else if (event.type == EventID::e_KnobTurned)
    {
        if (m_AccessPointCount <= 0) {
            EventSys::AddEvent(Event(EventID::e_AccessPointSelected, -1));
        } else if (m_AccessPointCount > 4) {
            SelectItem(m_SelectedItem + event.value);
        }            
        eventHandled = true;
    }
    else if (event.type == EventID::e_KeyDown)
    {
        switch(event.value)
        {
            case KbdKeyCodes::e_KnobButton:
                EventSys::AddEvent(Event(EventID::e_AccessPointSelected, m_SelectedItem));
                eventHandled = true;
                break;
            case KbdKeyCodes::e_Menu:
                EventSys::AddEvent(Event(EventID::e_AccessPointSelected, -1));
                eventHandled = true;
                break;
        }                
    }
    return eventHandled;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

const WifiAccessPoint* AccessPointSelector::GetSelectedAccessPoint() const
{
    if (m_SelectedItem >= 0 && m_SelectedItem < m_AccessPointCount) {
        return &m_AccessPoints[m_SelectedItem];
    } else {
        return nullptr;
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void AccessPointSelector::Print()
{
    for (uint8_t i = 0 ; i < 4 ; ++i)
    {
        int index = m_FirstVisibleItem + i;
        if (index < m_AccessPointCount)
        {
            uint8_t signalStrength = ESP8266::GetSignalPercentage(m_AccessPoints[index].m_Strength);

            int8_t level = min(5, signalStrength / 20 + 1);
            
            // 00-24 : 0
            // 25-49 : 1
            // 50-74 : 2
            // 75-99 : 3
            // 64-80 : 4

            // 00-19 : 0
            // 20-39 : 1
            // 40-59 : 2
            // 60-79 : 3
            // 80-00 : 4
            
            uint8_t charBm[8];
            memset(charBm, 0, sizeof(charBm));

            if (level > 0)
            {
                if (level == 1) charBm[7] = CB_10000;
                if (level > 1)
                {
                    charBm[6] = CB_11000;
                    if (level > 2)
                    {
                        charBm[4] = CB_11100;
                        if (level > 3)
                        {
                            charBm[2] = CB_11110;
                            if (level > 4)
                            {
                                charBm[0] = CB_11111;
                            }
                        }
                    }                        
                }                    
            }
            g_Display.WriteCustomCharBegin(i);
            for (uint8_t i = 0 ; i < 8; ++i) g_Display.WriteCustomCharRow(charBm[i]);
            
            if (signalStrength == 100) signalStrength = 99;

            g_Display.SetCursor(0, i);
            if ( index == m_SelectedItem ) {
                g_Display.WriteCharacter('\x7E'); // Arrow right
            } else if ( index != 0 && i == 0 ) {
                g_Display.WriteCharacter('\x06'); // Arrow up
            } else if ( index != m_AccessPointCount - 1 && i == 3 ) {
                g_Display.WriteCharacter('\x07'); // Arrow down
            } else {
                g_Display.WriteCharacter(' ');
            }
            if ( index == m_SelectedItem ) {
                g_Display.PrintScrollingText(m_AccessPoints[index].m_SSID, WIFI_MAX_SSID_LEN, true, 1, 16, i, &m_ScrollOffset);
                g_Display.WriteCharacter(' ');
            } else {                
                g_Display.Printf_P(PSTR("%-16.16s "), m_AccessPoints[index].m_SSID);
            }                
            g_Display.WriteCharacter((m_AccessPoints[index].m_Security == WifiSecurityMode_e::e_OPEN) ? 4 : 5);
            g_Display.WriteCharacter(i);
        }
        else
        {
            g_Display.SetCursor(0, i);
            g_Display.ClearToEndOfLine();
        }            
    }

}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void AccessPointSelector::SelectItem(int8_t selection)
{
    m_SelectedItem = clamp<int8_t>(0, m_AccessPointCount - 1, selection);
    const int8_t displayLines = 4; // m_DisplayEnd - m_DisplayStart + 1;
    m_FirstVisibleItem = m_SelectedItem - 2;
    if ( m_FirstVisibleItem < 0 ) m_FirstVisibleItem = 0;
    else if ( m_AccessPointCount - m_FirstVisibleItem < 4 ) m_FirstVisibleItem = m_AccessPointCount - displayLines;
    m_ScrollOffset = 0;
    Print();
}

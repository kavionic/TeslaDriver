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

#include "Config.h"

EEPROMContent g_EEPROMunmapped EEMEM;
EEPROMContent& g_EEPROM = *((EEPROMContent*)MAPPED_EEPROM_START);

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void Config::ResetConfig()
{
    // Global:
    eeprom_update_byte(&g_EEPROMunmapped.global.m_Version, EEPROMContent::VERSION);
    eeprom_update_byte(&g_EEPROMunmapped.global.m_StandbyState, 0);
    
    // Sound:
    eeprom_update_word(&g_EEPROMunmapped.sound.m_Frequency, 1000);
    eeprom_update_byte(&g_EEPROMunmapped.sound.m_Volume, 100);
    
    eeprom_update_byte(&g_EEPROMunmapped.sound.m_BeepLengths[BeepID::e_KeyPress], 10 / 5);
    eeprom_update_byte(&g_EEPROMunmapped.sound.m_BeepLengths[BeepID::e_KeyRepeat], 5 / 5);
    eeprom_update_byte(&g_EEPROMunmapped.sound.m_BeepLengths[BeepID::e_LongPress], 20 / 5);
    eeprom_update_byte(&g_EEPROMunmapped.sound.m_BeepLengths[BeepID::e_Knob], 5 / 5);
    eeprom_update_byte(&g_EEPROMunmapped.sound.m_BeepLengths[BeepID::e_Cancel], 20 / 5);
    eeprom_update_byte(&g_EEPROMunmapped.sound.m_BeepLengths[BeepID::e_StartCountDown], 25 / 5);
    eeprom_update_byte(&g_EEPROMunmapped.sound.m_BeepLengths[BeepID::e_FinishCountDown], 50 / 5);
    eeprom_update_byte(&g_EEPROMunmapped.sound.m_BeepLengths[BeepID::e_StartRadiating], 200 / 5);

    eeprom_update_byte(&g_EEPROMunmapped.sound.m_AlarmCycleCount, 20);
    
    eeprom_update_byte(&g_EEPROMunmapped.sound.m_AlarmCycle[0], 250 / 5);
    eeprom_update_byte(&g_EEPROMunmapped.sound.m_AlarmCycle[1], 125 / 5);
    
    eeprom_update_byte(&g_EEPROMunmapped.sound.m_AlarmCycle[2], 70  / 5);
    eeprom_update_byte(&g_EEPROMunmapped.sound.m_AlarmCycle[3], 125  / 5);
    
    eeprom_update_byte(&g_EEPROMunmapped.sound.m_AlarmCycle[4], 70  / 5);
    eeprom_update_byte(&g_EEPROMunmapped.sound.m_AlarmCycle[5], 125  / 5);
    
    eeprom_update_byte(&g_EEPROMunmapped.sound.m_AlarmCycle[6], 70  / 5);
    eeprom_update_byte(&g_EEPROMunmapped.sound.m_AlarmCycle[7], 125  / 5);


    eeprom_update_byte(&g_EEPROMunmapped.keyboard.m_RepeatDelay, 100);
    eeprom_update_byte(&g_EEPROMunmapped.keyboard.m_RepeatSpeed, 50);    
    
    for (uint8_t i = 0 ; i < ARRAY_COUNT(g_EEPROMunmapped.wifi.m_AccessPoints); ++i)
    {
        eeprom_update_byte((uint8_t*)&g_EEPROMunmapped.wifi.m_AccessPoints[i].m_SSID[0], 0);
        eeprom_update_byte((uint8_t*)&g_EEPROMunmapped.wifi.m_AccessPoints[i].m_Password[0], 0);
        eeprom_update_byte(&g_EEPROMunmapped.wifi.m_AccessPoints[i].m_SequenceNum, 255);
    }
    eeprom_update_byte((uint8_t*)&g_EEPROMunmapped.wifi.m_SelectedAccessPoint, -1);
    
    UpdateBool(&g_EEPROMunmapped.wifi.m_HotspotEnabled, false);
    char SSID[WIFI_MAX_SSID_LEN];
    strcpy_P(SSID, PSTR("Sparky"));
    UpdateBlock(SSID, g_EEPROMunmapped.wifi.m_HotspotSSID, strlen(SSID) + 1);
    UpdateByte(&g_EEPROMunmapped.wifi.m_HotspotPassword[0], 0);
    UpdateByte(&g_EEPROMunmapped.wifi.m_HotspotChannel, 6);
    UpdateByte(&g_EEPROMunmapped.wifi.m_HotspotEncryption, WifiSecurityMode_e::e_OPEN);
    UpdateBool(&g_EEPROMunmapped.wifi.m_HotspotHidden, false);
    

    MapEEPROM();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

uint8_t Config::GetAlarmCycleLength(uint8_t cycle)
{
    return (cycle < ARRAY_COUNT(g_EEPROM.sound.m_AlarmCycle)) ? g_EEPROM.sound.m_AlarmCycle[cycle] : 0;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

int8_t Config::FindAccessPoint(const char* SSID)
{
    for (uint8_t i = 0; i < ARRAY_COUNT(g_EEPROM.wifi.m_AccessPoints); ++i)
    {
        if (strncmp(g_EEPROM.wifi.m_AccessPoints[i].m_SSID, SSID, WIFI_MAX_SSID_LEN) == 0)
        {
            return i;
        }
    }
    return -1;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

const  WifiStoredAccessPoint* Config::GetAccessPoint(int8_t index)
{
    return (index >= 0 && index < I8(ARRAY_COUNT(g_EEPROM.wifi.m_AccessPoints))) ? &g_EEPROM.wifi.m_AccessPoints[index] : nullptr;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

int8_t Config::NewAccessPoint(const char*SSID)
{
    int8_t newIndex = -1;
/*    for (uint8_t i = 0; i < ARRAY_COUNT(g_EEPROM.wifi.m_AccessPoints); ++i)
    {
        const WifiStoredAccessPoint& accessPoint = g_EEPROM.wifi.m_AccessPoints[i];
        if (accessPoint.m_SSID[0] == '\0')
        {
            eeprom_write_byte(&g_EEPROMunmapped.wifi.m_AccessPoints[i].m_SequenceNum, 0)
            newIndex = i;
            break;
        }
    }*/
//    if (newIndex == -1)
//    {
        for (uint8_t i = 0; i < ARRAY_COUNT(g_EEPROM.wifi.m_AccessPoints); ++i)
        {
            MapEEPROM();
            const WifiStoredAccessPoint& accessPoint = g_EEPROM.wifi.m_AccessPoints[i];
            uint8_t currentSequence = accessPoint.m_SequenceNum;
            if (newIndex == -1 && currentSequence >= ARRAY_COUNT(g_EEPROM.wifi.m_AccessPoints) - 1)
            {
                // We have the oldest AP (ARRAY_COUNT(g_EEPROM.wifi.m_AccessPoints) - 1) or an unused AP (255)
                eeprom_write_byte(&g_EEPROMunmapped.wifi.m_AccessPoints[i].m_SequenceNum, 0);
                newIndex = i;
            }
            else
            {
                if (currentSequence != 255) {
                    eeprom_write_byte(&g_EEPROMunmapped.wifi.m_AccessPoints[i].m_SequenceNum, currentSequence + 1);
                }                    
            }
        }
//    }
    if (newIndex != -1)
    {
        int8_t len = strnlen(SSID, WIFI_MAX_SSID_LEN);
        if (len < WIFI_MAX_SSID_LEN) len++; // Include null termination if it fit.
        eeprom_update_block(SSID, g_EEPROMunmapped.wifi.m_AccessPoints[newIndex].m_SSID, len);
    }
    MapEEPROM();
    return newIndex;
/*    uint8_t sequenceMin = 255;
    uint8_t sequenceMax = 0;
    for (uint8_t i = 0; i < ARRAY_COUNT(g_EEPROM.wifi.m_AccessPoints); ++i)
    {
        const WifiStoredAccessPoint& accessPoint = g_EEPROM.wifi.m_AccessPoints[i];
        if (accessPoint.m_SSID[0] != '\0')
        {
            if (accessPoint.m_SequenceNum < sequenceMin) sequenceMin =  accessPoint.m_SequenceNum;
            if (accessPoint.m_SequenceNum > sequenceMax) sequenceMax =  accessPoint.m_SequenceNum;
        }
    } */
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void Config::SetAccessPointPassword( int8_t index, const char* passwd )
{
    if (index >= 0 && index < I8(ARRAY_COUNT(g_EEPROM.wifi.m_AccessPoints)))
    {
        int8_t len = strnlen(passwd, WIFI_MAX_PASSWD_LEN);
        if (len < WIFI_MAX_PASSWD_LEN) len++; // Include null termination if it fit.
        eeprom_update_block(passwd, g_EEPROMunmapped.wifi.m_AccessPoints[index].m_Password, len);
        MapEEPROM();
    }        
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void Config::UpdateAccessPoint(int8_t index, const WifiStoredAccessPoint* accessPoint)
{
    if (index >= 0 && index < I8(ARRAY_COUNT(g_EEPROM.wifi.m_AccessPoints)))
    {
        eeprom_update_block(accessPoint, &g_EEPROMunmapped.wifi.m_AccessPoints[index], sizeof(g_EEPROM.wifi.m_AccessPoints[0]));
        MapEEPROM();
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

int8_t Config::GetSelectedAccessPoint()
{
    return g_EEPROM.wifi.m_SelectedAccessPoint;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void Config::SetSelectedAccessPoint(int8_t index)
{
    eeprom_update_byte((uint8_t*)&g_EEPROMunmapped.wifi.m_SelectedAccessPoint, index);
    MapEEPROM();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void Config::SetHotspotEnabled(bool enable)
{
    UpdateBool(&g_EEPROMunmapped.wifi.m_HotspotEnabled, enable);
    MapEEPROM();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void Config::SetHotspotSSID(const char* SSID)
{
    UpdateBlock(SSID, g_EEPROMunmapped.wifi.m_HotspotSSID, min<int8_t, int8_t>(WIFI_MAX_SSID_LEN, strnlen(SSID, WIFI_MAX_SSID_LEN) + 1));
    MapEEPROM();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void Config::SetHotspotPassword(const char* password)
{
    UpdateBlock(password, g_EEPROMunmapped.wifi.m_HotspotPassword, min<int8_t, int8_t>(WIFI_MAX_PASSWD_LEN, strnlen(password, WIFI_MAX_PASSWD_LEN) + 1));
    MapEEPROM();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void Config::SetHotspotChannel(uint8_t channel)
{
    UpdateByte(&g_EEPROMunmapped.wifi.m_HotspotChannel, channel);
    MapEEPROM();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void Config::SetHotspotEncryption(WifiSecurityMode_e::Enum encryption)
{
    UpdateByte(&g_EEPROMunmapped.wifi.m_HotspotEncryption, encryption);
    MapEEPROM();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void Config::SetHotspotHidden(bool hidden)
{
    UpdateBool(&g_EEPROMunmapped.wifi.m_HotspotHidden, hidden);
    MapEEPROM();
}

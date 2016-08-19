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


#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdio.h>
#include <avr/eeprom.h>

#include "Hardware.h"

#include "Misc/Utils.h"
#include "ESP8266/ESP8266.h"

#define ASSERT_NOISR() do { if ( (SREG & BIT8(7,1)) == 0 ) { panic(); } } while(false)

namespace BeepID
{
    enum Enum
    {
        e_KeyPress,
        e_KeyRepeat,
        e_LongPress,
        e_Knob,
        e_Cancel,
        e_StartCountDown,
        e_FinishCountDown,
        e_StartRadiating,
        e_BeepCount
    };
}

struct EEPROMContent
{
    static const uint8_t VERSION = 2;
    struct Global
    {
        uint8_t        m_Version;
        uint8_t        m_StandbyState; // 1 when on, 0 if standby. If 1 when powered up it will activate, if not it will go to standby.
    } global;
    struct Sound
    {
        uint16_t       m_Frequency;
        uint8_t        m_Volume;
        uint8_t        m_BeepLengths[BeepID::e_BeepCount];
        uint8_t        m_AlarmCycle[8];
        uint8_t        m_AlarmCycleCount;
    } sound;
    struct Keyboard
    {
        uint8_t m_RepeatDelay;
        uint8_t m_RepeatSpeed;
    } keyboard;
    
    struct Wifi
    {
        WifiStoredAccessPoint    m_AccessPoints[30];
        int8_t                   m_SelectedAccessPoint;
        bool                     m_HotspotEnabled;
        char                     m_HotspotSSID[WIFI_MAX_SSID_LEN];
        char                     m_HotspotPassword[WIFI_MAX_PASSWD_LEN];
        uint8_t                  m_HotspotChannel;
        uint8_t                  m_HotspotEncryption;
        bool                     m_HotspotHidden;
    };
    struct Runtime
    {
        uint32_t       m_TotalRuntime[64];
        uint32_t       m_InverterRuntime[8];
        uint32_t       m_InverterNormalizedRuntime[8];
    };
    
    uint8_t m_Padding[EEPROM_SIZE - sizeof(Global) - sizeof(Sound) - sizeof(Keyboard) - sizeof(Wifi) - sizeof(Runtime)];
    Wifi         wifi;
    Runtime      runtime;
};

extern EEPROMContent& g_EEPROM;

#if !defined(VA_VERSION)
extern EEPROMContent g_EEPROMunmapped EEMEM;
#else
extern EEPROMContent g_EEPROMunmapped; // Never defined, but works with visual assist.
#endif

class Config
{
public:
    // Sound:
    static void ResetConfig();
    
    static uint8_t GetBuzzerVolume()              { ASSERT_NOISR(); return g_EEPROM.sound.m_Volume; }
    static void    SetBuzzerVolume(uint8_t value) { ASSERT_NOISR(); return eeprom_update_byte(&g_EEPROMunmapped.sound.m_Volume, value); }

    static uint16_t GetBuzzerFrequency()              { ASSERT_NOISR(); return g_EEPROM.sound.m_Frequency; }
    static void     SetBuzzerFrequency(uint16_t value) { ASSERT_NOISR(); return eeprom_update_word(&g_EEPROMunmapped.sound.m_Frequency, value); }
    
    static uint8_t GetBeepLength(BeepID::Enum beepID) { return g_EEPROM.sound.m_BeepLengths[beepID]; }
    static void    SetBeepLength(BeepID::Enum beepID, uint8_t length);

    static uint8_t GetAlarmCycleCount() { return g_EEPROM.sound.m_AlarmCycleCount; }
    static void    SetAlarmCycleCount(uint8_t count);

    static uint8_t GetAlarmCycleLength(uint8_t cycle);

    // Keyboard:
    static uint8_t GetKeyRepeatDelay() { return g_EEPROM.keyboard.m_RepeatDelay; }
    //    static void    SetKeyRepeatDelay(uint8_t delay);

    static uint8_t GetKeyRepeatSpeed() { return g_EEPROM.keyboard.m_RepeatSpeed; }
    //    static void    SetKeyRepeatSpeed(uint8_t speed);

    // Wifi:
    static int8_t FindAccessPoint(const char* SSID);
    static const WifiStoredAccessPoint* GetAccessPoint(int8_t index);
    static int8_t NewAccessPoint( const char*SSID );
    static void   SetAccessPointPassword(int8_t index, const char* passwd);
    static void   UpdateAccessPoint(int8_t index, const WifiStoredAccessPoint* accessPoint);
    static int8_t GetSelectedAccessPoint();
    static void   SetSelectedAccessPoint(int8_t index);


    static void SetHotspotEnabled(bool enable);
    static bool IsHotspotEnabled() { return g_EEPROM.wifi.m_HotspotEnabled; }

    static void SetHotspotSSID(const char* SSID);
    static const char* GetHotspotSSID() { return g_EEPROM.wifi.m_HotspotSSID; }

    static void SetHotspotPassword(const char* password);
    static const char* GetHotspotPassword() { return g_EEPROM.wifi.m_HotspotPassword; }

    static void  SetHotspotChannel(uint8_t channel); // CH 1-13
    static uint8_t GetHotspotChannel() { return g_EEPROM.wifi.m_HotspotChannel; }

    static void                     SetHotspotEncryption(WifiSecurityMode_e::Enum encryption);
    static WifiSecurityMode_e::Enum GetHotspotEncryption() { return WifiSecurityMode_e::Enum(g_EEPROM.wifi.m_HotspotEncryption); }

    static bool IsHotspotHidden() { return g_EEPROM.wifi.m_HotspotHidden; }
    static void SetHotspotHidden(bool hidden);
    
    static void MapEEPROM() { eeprom_busy_wait(); NVM.CTRLB = NVM_EEMAPEN_bm; } // Map EEPROM into MAPPED_EEPROM_START

    static void WriteByte(uint8_t* dst, uint8_t value)              { eeprom_write_byte (dst, value); }
    static void WriteWord(uint16_t* dst, uint16_t value)            { eeprom_write_word (dst, value); }
    static void WriteDWord(uint32_t* dst, uint32_t value)           { eeprom_write_dword (dst, value); }
    static void WriteFloat(float* dst, float value)                 { eeprom_write_float (dst, value); }
    static void WriteBlock(const void *src, void *dst, size_t size) { eeprom_write_block (src, dst, size); }
        
    static void UpdateByte(int8_t* dst, uint8_t value)               { eeprom_update_byte((uint8_t*)dst, value); }
    static void UpdateByte(uint8_t* dst, uint8_t value)              { eeprom_update_byte(dst, value); }
    static void UpdateByte(char* dst, uint8_t value)                 { eeprom_update_byte((uint8_t*)dst, value); }
    static void UpdateBool(bool* dst, bool value)                    { eeprom_update_byte((uint8_t*)dst, value); }
    static void UpdateWord(uint16_t* dst, uint16_t value)            { eeprom_update_word(dst, value); }
    static void UpdateDWord(uint32_t* dst, uint32_t value)           { eeprom_update_dword(dst, value); }
    static void UpdateFloat(float* dst, float value)                 { eeprom_update_float(dst, value); }
    static void UpdateBlock(const void* src, void* dst, size_t size) { eeprom_update_block(src, dst, size); }
    
private:
    Config( const Config &c );
    Config& operator=( const Config &c );

};

#endif //__CONFIG_H__

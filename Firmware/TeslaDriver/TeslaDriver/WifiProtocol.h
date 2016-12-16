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


#ifndef WIFIPROTOCOL_H_
#define WIFIPROTOCOL_H_


enum class WifiCmd_e : uint8_t
{
    // Shared messages:
    e_Ping = 0,
    e_Pong = 1,
    e_GetSystemInfo = 2,
    e_GetSystemInfoReply = 3,
    e_SetPreferredBootMode = 4,
    e_Reboot = 5,
    e_GetRadioStatus = 6,
    e_GetRadioStatusReply = 7,
    e_BytesReceivedReply = 8,
    
    // Bootloader messages:
    e_EraseProgramSection = 18,
    e_EraseProgramSectionDone = 19,
    e_BeginWriteFlash = 20,
    e_BeginWriteFlashReply = 21, // WifiSetVal16: Flash page size.
    e_GetFlashCRC = 22,
    e_GetFlashCRCReply = 23,
    e_ReadFlashPages = 24,
    e_WriteFlashBytesReceivedReply = 25,
    
    
    // Inverter messages:
    e_SetPWMDutyCycle = 50,
    e_SetPWMPeriod,
    e_SetPWMDeadTime,
    e_SetCurrentLimits,
    e_SetPWMTemperatureResolution,
    e_GetPWMStatus,
    e_GetPWMStatusReply,
    e_WritePWMModulationData,
    e_WritePWMModulationDataDone,
    e_SetPWMModulationSampleRate,
    e_SetPWMModulationScale
};

static const uint16_t WIFI_PACKAGE_MAGIC = 0x20cb;

struct WifiPackageHeader
{
    template<typename T>
    static void InitMsg(T& msg, WifiCmd_e command)
    {
        msg.m_Magic = WIFI_PACKAGE_MAGIC;
        msg.m_PackageLength = sizeof(msg);
        msg.m_Command = command;
    }
    
    uint16_t  m_Magic;
    uint8_t   m_PackageLength;
    WifiCmd_e m_Command;
};

struct WifiSetVal8 : WifiPackageHeader
{
    uint8_t m_Value;
};

struct WifiSetVal16 : WifiPackageHeader
{
    uint16_t m_Value;
};

struct WifiSetVal32 : WifiPackageHeader
{
    uint32_t m_Value;
};

enum class WifiBootMode_e : uint8_t
{
    e_Application,
    e_BootLoader
};
 
struct WifiGetSystemInfoReply : WifiPackageHeader
{
#ifdef APP_SECTION_PAGE_SIZE
    void InitSystemInfo(WifiBootMode_e bootMode, WifiBootMode_e preferredBootMode, uint32_t uptime)
    {
        m_BootMode                 = bootMode;
        m_PreferredBootMode        = preferredBootMode;
        m_NetworkBufferSize        = WIFI_NETWORK_BUFFER_SIZE;
        m_Uptime                   = uptime;

        m_EEPROMPageSize           = EEPROM_PAGE_SIZE;
        m_ApplicationFlashPageSize = APP_SECTION_PAGE_SIZE;

        m_EEPROMStart              = EEPROM_START;
        m_EEPROMSize               = EEPROM_SIZE;

        m_SRAMStart                = INTERNAL_SRAM_START;
        m_SRAMSize                 = INTERNAL_SRAM_SIZE;

        m_ApplicationFlashStart    = APP_SECTION_START;
        m_ApplicationFlashSize     = APP_SECTION_SIZE;
    }    
#endif // APP_SECTION_PAGE_SIZE
    WifiBootMode_e m_BootMode;
    WifiBootMode_e m_PreferredBootMode;
    
    uint16_t       m_NetworkBufferSize;

    uint32_t       m_Uptime;
    
    uint16_t       m_EEPROMPageSize;
    uint16_t       m_ApplicationFlashPageSize;

    uint32_t       m_EEPROMStart;
    uint32_t       m_EEPROMSize;

    uint32_t       m_SRAMStart;
    uint32_t       m_SRAMSize;
    
    uint32_t       m_ApplicationFlashStart;
    uint32_t       m_ApplicationFlashSize;
};

struct WifiSetPreferredBootMode : WifiPackageHeader
{
    WifiBootMode_e m_BootMode;
    bool           m_Reboot;
};


struct WifiSetPWMDeadTime : WifiPackageHeader
{
    uint8_t m_DeadTimeLS;
    uint8_t m_DeadTimeHS;
};

struct WifiSetPWMCurrentLimits : WifiPackageHeader
{
    uint16_t m_LimitLow;
    uint16_t m_LimitHigh;
};

struct WifiSetPWMModulationScale : WifiPackageHeader
{
    uint8_t m_Multiplier;
    uint8_t m_RightShifter;
};

struct WifiGetRadioStatusReply : WifiPackageHeader
{
    int16_t m_Strength;
    int16_t m_Channel;
};

struct WifiGetPWMStatusReply : WifiPackageHeader
{
    uint32_t m_Frequency;
    uint16_t m_PWMPeriode;
    uint16_t m_PWMDutyCycle;
    uint8_t  m_PWMDeadTimeLS;
    uint8_t  m_PWMDeadTimeHS;
    uint16_t m_PWMCurrentLimitLow;
    uint16_t m_PWMCurrentLimitHigh;
    uint8_t  m_ModulationMultiplier;    // Samples are multiplied by this
    uint8_t  m_ModulationShifter;       // And then shifted right this many bits.
    uint16_t m_ModulationSampleRate;
    int16_t  m_Temperature1;
    int16_t  m_Temperature2;
    int8_t   m_TemperatureResolution; // Number of bits between 9 and 12 inclusive
    uint8_t  m_Padding;
    int16_t  m_CurrentLow;
    int16_t  m_CurrentHigh;
};

struct WifiBeginWriteFlashPages : WifiPackageHeader
{
    uint32_t m_Address;
    uint32_t m_Length;
};

struct WifiGetFlashCRC : WifiPackageHeader
{
    uint32_t m_StartAddress;
    uint32_t m_EndAddress;
};

struct WifiReadFlashPages : WifiPackageHeader
{
    uint32_t m_Address;
    uint32_t m_Length;
};

#endif /* WIFIPROTOCOL_H_ */
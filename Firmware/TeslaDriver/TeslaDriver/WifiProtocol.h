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
    e_Ping,
    e_Pong,
    e_GetRadioStatus,
    e_GetRadioStatusReply,
    e_SetPWMDutyCycle,
    e_SetPWMPeriod,
    e_SetPWMDeadTime,
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

struct WifiSetPWMDeadTime : WifiPackageHeader
{
    uint8_t m_DeadTimeLS;
    uint8_t m_DeadTimeHS;
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
    uint8_t  m_ModulationMultiplier;    // Samples are multiplied by this
    uint8_t  m_ModulationShifter;       // And then shifted right this many bits.
    uint16_t m_ModulationSampleRate;
    int16_t  m_Temperature1;
    int16_t  m_Temperature2;
    int8_t   m_TemperatureResolution; // Number of bits between 9 and 12 inclusive
};


#endif /* WIFIPROTOCOL_H_ */
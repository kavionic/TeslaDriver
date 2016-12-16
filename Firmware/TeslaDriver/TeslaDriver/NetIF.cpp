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

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Misc/Utils.h"

#include "NetIF.h"
#include "UserIF.h"
#include "PWMController.h"
#include "DS18B20.h"
#include "EventSys.h"
#include "Hardware.h"
#include "Config.h"

#include "Misc/Clock.h"
#include "Misc/SpinTimer.h"


///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

NetIF::NetIF()
{
    //printf_P(PSTR("NetIF::NetIF()\n"));
    //InitRadio();
}

void NetIF::RestartRadio()
{
    uint32_t startTime = Clock::GetTime();
    printf_P(PSTR("Initializing radio.\n"));
    //g_Display.ClearDisplay();
    //g_Display.SetCursor(0, 0);
    //g_Display.Printf_P(PSTR("Restarting radio..."));
    //g_Display.ClearToEndOfLine();
    g_WifiDevice.ResetRadio();
    //SpinTimer::SleepMS(1000);
    /*        if (!g_WifiDevice.RestartRadio()){
        continue;
    }*/
    g_WifiDevice.EnableEcho(false);
    //g_Display.SetCursor(0, 0);
    //g_Display.Printf_P(PSTR("Set ESP baudrate..."));
    //g_Display.ClearToEndOfLine();
    if (!g_WifiDevice.SetBaudrate()){
        return;
    }
    //g_Display.SetCursor(0, 0);
    //g_Display.Printf_P(PSTR("Misc radio setup..."));
    //g_Display.ClearToEndOfLine();
//    if (!g_WifiDevice.GetModuleVersion()){
//        return;
//    }
    g_WifiDevice.SetMuxMode(WifiMuxMode_e::e_Multiple);
/*    if (!g_WifiDevice.SetWifiMode(WifiMode_e::e_Station)){
        return;
    }*/
/*    if (Config::IsHotspotEnabled()) {
        UpdateHotspot();
    }*/
    printf_P(PSTR("Radio reset took %ldmS\n"), Clock::GetTime() - startTime);
    
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void NetIF::Stop()
{
    if (m_State == e_StateConnectingToAP) {
        RestartRadio();
    } else if (m_State == e_StateRunning) {
        g_WifiDevice.DisconnectFromAP();
    }
    m_NextConnectionTime = 0;
    m_CurReconnectionDelay = 1;
    m_State = e_StateIdle;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////
/*
void NetIF::InitRadio()
{
    printf_P(PSTR("Restarting radio!\n"));
    RestartRadio();
    m_State = e_StateInitRadio;
}*/

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void NetIF::ReconfigureRadio()
{
/*    if (m_State == e_StateInitRadio) {
        return;
    }
    if (m_State == e_StateConnectingToAP || m_State == e_StateUninitialized) {
        RestartRadio();
    } else if (m_State == e_StateRunning) {
        g_WifiDevice.DisconnectFromAP();
        g_WifiDevice.SetupHotspot(false, "", "", 0, WifiSecurityMode_e::e_WPA2_PSK, 0, false);
    }*/
    RestartRadio();
    m_NextConnectionTime = 0;
    m_State = e_StateInitRadio;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void NetIF::UpdateHotspot()
{
    g_WifiDevice.DisconnectFromAP();
    g_WifiDevice.SetupHotspot(Config::IsHotspotEnabled(), Config::GetHotspotSSID(), Config::GetHotspotPassword(), Config::GetHotspotChannel(), Config::GetHotspotEncryption(), 4, Config::IsHotspotHidden());
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void NetIF::Run(const Event& event)
{
    if (m_State != e_StateConnectingToAP && (g_WifiDevice.GetStatusFlags() & ESP8266::e_StatusConnectingToAP))
    {
        RestartRadio();
        //g_WifiDevice.PollConnectToAP(); // Stay away from the radio until we have gotten a reply. Even if we changed our mind.
        //return;
    }
    if (m_State == e_StateInitRadio)
    {
        if (Config::IsHotspotEnabled())
        {
            g_WifiDevice.DisconnectFromAP();
            if (g_WifiDevice.SetupHotspot(Config::IsHotspotEnabled(), Config::GetHotspotSSID(), Config::GetHotspotPassword(), Config::GetHotspotChannel(), Config::GetHotspotEncryption(), 4, Config::IsHotspotHidden()))
            {
                if (!g_WifiDevice.StartServer(42)){
                    return;
                }
                m_State = e_StateRunning;                
            }
        }
        else
        {
            g_WifiDevice.SetupHotspot(false, "", "", 0, WifiSecurityMode_e::e_WPA2_PSK, 0, false);
            if (m_NextConnectionTime == 0 || I32(m_NextConnectionTime - Clock::GetTime()) > 0)
            {
                int8_t selectedAP = Config::GetSelectedAccessPoint();
                printf_P(PSTR("Connecting to stored AP %d\n"), selectedAP);
                if (selectedAP != -1)
                {
                    const WifiStoredAccessPoint* accessPoint = Config::GetAccessPoint(selectedAP);
                    printf_P(PSTR("'%s' : '%s'\n"), accessPoint->m_SSID, accessPoint->m_Password);
                    if (!g_WifiDevice.ConnectToAP(accessPoint->m_SSID, accessPoint->m_Password)){
                        return;
                    }
                    m_State = e_StateConnectingToAP;
                    m_NextConnectionTime = Clock::GetTime() + 30000; // Timeout for connection result.
                }
                else
                {
                    g_WifiDevice.DisconnectFromAP();
                    m_State = e_StateRunning;
                }
            }
        }            
    }
    else if (m_State == e_StateConnectingToAP)
    {
        int8_t result = g_WifiDevice.PollConnectToAP();
        if (result < 0)
        {
            printf_P(PSTR("Poll result: %d\n"), result);
            if (m_CurReconnectionDelay < 63) {
                m_NextConnectionTime = Clock::GetTime() + U16(m_CurReconnectionDelay) * 1000;
                m_CurReconnectionDelay <<= 1;
            }  else {
                m_NextConnectionTime = 0;
                m_CurReconnectionDelay = 1;
                RestartRadio();                
            }
            m_State = e_StateInitRadio;
        }
        else if (result > 0)
        {
            m_CurReconnectionDelay = 1;
            
            g_WifiDevice.UpdateStationAddress();
            printf_P(PSTR("Signal strength: %d\n"), g_WifiDevice.GetSignalStrength());
            //g_Display.SetCursor(0, 0);
            //g_Display.Printf_P(PSTR("Starting server..."));        
            //g_Display.ClearToEndOfLine();
            
            m_BytesReceived             = 0;
            m_CurrentLinkID             = -1;
            m_PendingReplies            = 0;
            m_SendPWMStatusLinkIDs      = 0;
            m_SendRadioStatusLinkIDs    = 0;
            m_ModulationBytesToReceive  = 0;
            m_ModulationReceivedLinkIDs = 0;
            m_ModulationDataRequestSent = false;
            
            if (!g_WifiDevice.StartServer(42)){
                return;
            }            
            m_State = e_StateRunning;
            printf_P(PSTR("Done starting server.\n"));
        } else if (I32(m_NextConnectionTime - Clock::GetTime()) <= 0) {
            printf_P(PSTR("ERROR: Timeout while connecting to AP.\n"));
            m_CurReconnectionDelay = 1;
            m_NextConnectionTime = 0;
            RestartRadio();
            m_State = e_StateInitRadio;
        }
    }
    else if (m_State == e_StateRunning)
    {
        if (m_PendingReplies & BIT8(e_PendingReplyPong, 1))
        {
            m_PendingReplies &= U8(~BIT8(e_PendingReplyPong, 1));
            WifiPackageHeader reply;
            WifiPackageHeader::InitMsg(reply, WifiCmd_e::e_Pong);
            
            g_WifiDevice.SendIPData(m_CurrentLinkID, &reply, sizeof(reply));
        }
        if (m_PendingReplies & BIT8(e_PendingReplyGetSystemInfo, 1))
        {
            m_PendingReplies &= U8(~BIT8(e_PendingReplyGetSystemInfo, 1));
            
            WifiGetSystemInfoReply reply;
            WifiPackageHeader::InitMsg(reply, WifiCmd_e::e_GetSystemInfoReply);
            reply.InitSystemInfo(WifiBootMode_e::e_Application, WifiBootMode_e(g_EEPROM.global.m_PreferredBootMode), Clock::GetTime());
            g_WifiDevice.SendIPData(m_CurrentLinkID, &reply, sizeof(reply));
        }
        
    //    static uint8_t prevModulationBuffer = 1;
    
    //    if ( g_PWMController.GetCurrentModulationBuffer() != prevModulationBuffer )
        if (m_CurrentLinkID != -1 && ((!m_ModulationDataRequestSent && g_PWMController.GetAvaialbleModulationDataSpace() >= 2048) || (event.type == EventID::e_TimeTick100 && g_PWMController.GetAvaialbleModulationDataSpace() >= 8192)))
        {
            m_ModulationDataRequestSent = true;
    //        prevModulationBuffer = g_PWMController.GetCurrentModulationBuffer();
            //printf_P(PSTR("C\n"));
        
            WifiPackageHeader reply;
            WifiPackageHeader::InitMsg(reply, WifiCmd_e::e_WritePWMModulationDataDone);
        
            g_WifiDevice.SendIPData(m_CurrentLinkID, &reply, sizeof(reply));
        }
    
        if (m_SendPWMStatusLinkIDs != 0)
        {        
            uint8_t receivers = m_SendPWMStatusLinkIDs;
            m_SendPWMStatusLinkIDs = 0;
            WifiGetPWMStatusReply reply;
            WifiPackageHeader::InitMsg(reply, WifiCmd_e::e_GetPWMStatusReply);
            
            reply.m_Frequency = CPU_FREQ;
            reply.m_PWMPeriode = g_PWMController.GetPeriod();
            reply.m_PWMDutyCycle = g_PWMController.GetDutyCycle();
            g_PWMController.GetDeadTime(&reply.m_PWMDeadTimeLS, &reply.m_PWMDeadTimeHS);
            g_PWMController.GetCurrentLimits(&reply.m_PWMCurrentLimitLow, &reply.m_PWMCurrentLimitHigh);
            g_PWMController.GetModulationScale(reply.m_ModulationMultiplier, reply.m_ModulationShifter);
            reply.m_ModulationSampleRate  = g_PWMController.GetModulationSampleRate();
            reply.m_Temperature1          = DS18B20::GetTemp1();
            reply.m_Temperature2          = DS18B20::GetTemp2();
            reply.m_TemperatureResolution = DS18B20::GetResolution();
            cli();
            reply.m_CurrentLow            = ADCB.CH0.RES;
            reply.m_CurrentHigh           = ADCB.CH1.RES;
            sei();
            //printf_P(PSTR("Sending PWM status.\n"));
            for (uint8_t i = 0 ; i < ESP8266::WIFI_MAX_LINKS ; ++i) {
                if (receivers & BIT8(i, 1)) g_WifiDevice.SendIPData(i, &reply, sizeof(reply));
            }
            //printf_P(PSTR("Done sending PWM status.\n"));
            m_SendPWMStatusLinkIDs = 0;
        }
        if (m_SendRadioStatusLinkIDs != 0)
        {
            uint8_t receivers = m_SendRadioStatusLinkIDs;
            m_SendRadioStatusLinkIDs = 0;
            WifiGetRadioStatusReply reply;        
            WifiPackageHeader::InitMsg(reply, WifiCmd_e::e_GetRadioStatusReply);
        
            reply.m_Strength = g_WifiDevice.GetSignalStrength();        
            for (uint8_t i = 0 ; i < ESP8266::WIFI_MAX_LINKS ; ++i) {
                if (receivers & BIT8(i, 1)) g_WifiDevice.SendIPData(i, &reply, sizeof(reply));
            }
        }
/*        uint16_t bytesReceived = g_WifiDevice.GetAndClearChannelBytesReceived(m_CurrentLinkID);
        if (bytesReceived != 0)
        {
            WifiSetVal16 msg;
            WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_BytesReceivedReply);
            msg.m_Value = bytesReceived;
            g_WifiDevice.SendIPData(m_CurrentLinkID, &msg, sizeof(msg));
        }*/
    }        
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void NetIF::ConnectionChanged(uint8_t linkID, bool isConnected)
{
    if ( isConnected ) {
        m_CurrentLinkID = linkID;
    }
    printf_P(PSTR("Connection changed: %d\n"), isConnected);
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void NetIF::DataReceived(uint8_t linkID, int16_t size)
{
    m_CurrentLinkID = linkID;
    //printf_P(PSTR("NetIF Received %d bytes on %d:\n"), size, linkID);
//    const uint8_t* data = static_cast<const uint8_t*>(buffer);


    while(size > 0)
    {
        if (m_ModulationBytesToReceive)
        {
            for(;;)
            {
                int16_t len = min(m_ModulationBytesToReceive, size);
                if ( len == 0 ) {
                    break;
                }
                int16_t bytesReceived;
                const uint8_t* data = reinterpret_cast<const uint8_t*>(g_WifiDevice.GetReceiveBuffer(len, &bytesReceived));
                if (data != nullptr)
                {
                    if ( bytesReceived != g_PWMController.WriteModulationData(data, bytesReceived)) {
                        printf_P(PSTR("ERROR: PWMController modulation buffer full!\n"));
                    }
                    g_WifiDevice.ConsumeReceiveBytes(bytesReceived);
                    size -= bytesReceived;
                    m_ModulationBytesToReceive -= bytesReceived;
                    //printf_P(PSTR("Received %d modulation bytes: %d (%d)\n"), bytesReceived, m_ModulationBytesToReceive, size);
                }
                else
                {
                    printf_P(PSTR("ERROR: Failed to receive modulation data\n"));
                    size = 0;
                    break;
                }
            }
//            data += len;            
            continue;
        }
        if (m_BytesReceived < sizeof(m_Packages.m_Header.m_Magic))
        {
            uint8_t len = min(I16(sizeof(m_Packages.m_Header.m_Magic) - m_BytesReceived), size);
            //memcpy(m_Packages.m_Buffer + m_BytesReceived, data, len);
            if (g_WifiDevice.Read(m_Packages.m_Buffer + m_BytesReceived, len) != len) {
                printf_P("ERROR: Failed to receive package magic number!\n");
                m_BytesReceived = 0;
                g_WifiDevice.CloseConnection(linkID);
                return;
            }
            m_BytesReceived += len;
            size -= len;
            if (m_BytesReceived == sizeof(m_Packages.m_Header.m_Magic) && m_Packages.m_Header.m_Magic != WIFI_PACKAGE_MAGIC)
            {
                printf_P(PSTR("ERROR: Received wifi package with invalid magic number: %04x\n"), m_Packages.m_Header.m_Magic);
//                size--; // Skip a single byte to try finding a valid packet start.
//                data++;
                m_BytesReceived = 0;
                g_WifiDevice.CloseConnection(linkID);
                return;
            }
            //data += len;
        }
        if (m_BytesReceived < sizeof(m_Packages.m_Header))
        {
            uint8_t len = min(I16(sizeof(m_Packages.m_Header) - m_BytesReceived), size);
//            memcpy(m_Packages.m_Buffer + m_BytesReceived, data, len);
            if (g_WifiDevice.Read(m_Packages.m_Buffer + m_BytesReceived, len) != len) {
                printf_P("ERROR: Failed to receive package header!\n");
                m_BytesReceived = 0;
                g_WifiDevice.CloseConnection(linkID);
                return;
            }
            m_BytesReceived += len;            
            size -= len;
//            data += len;
        }
        if (m_BytesReceived >= sizeof(m_Packages.m_Header))
        {
            if (m_Packages.m_Header.m_PackageLength > sizeof(m_Packages))
            {
                printf_P(PSTR("ERROR: Invalid message size: %u (%u)\n"), m_Packages.m_Header.m_PackageLength, sizeof(m_Packages));
                m_BytesReceived = 0;
                g_WifiDevice.CloseConnection(linkID);
                return;
            }
            int16_t len = min<int16_t>(m_Packages.m_Header.m_PackageLength - m_BytesReceived, size);
            //memcpy(m_Packages.m_Buffer + m_BytesReceived, data, len);
            if (g_WifiDevice.Read(m_Packages.m_Buffer + m_BytesReceived, len) != len) {
                printf_P("ERROR: Failed to receive package body!\n");
                m_BytesReceived = 0;
                g_WifiDevice.CloseConnection(linkID);
                return;
            }
            m_BytesReceived += len;
            size -= len;
            //data += len;
            if (m_BytesReceived == m_Packages.m_Header.m_PackageLength)
            {
                ProcessMessage(linkID);
                m_BytesReceived = 0;
            }
        }
    }        
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void NetIF::ProcessMessage(uint8_t linkID)
{
    
    switch(m_Packages.m_Header.m_Command)
    {
        case WifiCmd_e::e_Ping:
            //printf_P(PSTR("Get Radio status\n"));
            m_PendingReplies |= BIT8(e_PendingReplyPong, 1);
            break;
        case WifiCmd_e::e_SetPreferredBootMode:
            eeprom_write_byte(&g_EEPROMunmapped.global.m_PreferredBootMode, U8(m_Packages.m_SetPreferredBootMode.m_BootMode));
            eeprom_busy_wait();
            Config::MapEEPROM();
            if (!m_Packages.m_SetPreferredBootMode.m_Reboot) {
                break;
            }
            // FALL THROUGH //                
        case WifiCmd_e::e_Reboot:
            CCP = CCP_IOREG_gc;
            RST.CTRL = RST_SWRST_bm;
            break;
        case WifiCmd_e::e_GetSystemInfo:
            m_PendingReplies |= BIT8(e_PendingReplyGetSystemInfo, 1);
            break;
        case WifiCmd_e::e_SetPWMDutyCycle:
            //printf_P(PSTR("Set PWM: %d\n"), uint16_t((uint32_t(m_Packages.m_SetValue16.m_Value) * 100 + 32768) / 65536));
            g_PWMController.SetDutyCycle(m_Packages.m_SetValue16.m_Value);
            break;
        case WifiCmd_e::e_SetPWMPeriod:
            //printf_P(PSTR("Set PWM period: %d (%.4fkHz)\n"), m_Packages.m_SetValue16.m_Value, double(CPU_FREQ) / double(m_Packages.m_SetValue16.m_Value) / 1000.0);
            g_PWMController.SetPeriod(m_Packages.m_SetValue16.m_Value);
            break;
        case WifiCmd_e::e_SetPWMDeadTime:
            printf_P(PSTR("Set dead time: %d/%d\n"), m_Packages.m_SetPWMDeadTime.m_DeadTimeLS, m_Packages.m_SetPWMDeadTime.m_DeadTimeHS);
            g_PWMController.SetDeadTime(m_Packages.m_SetPWMDeadTime.m_DeadTimeLS, m_Packages.m_SetPWMDeadTime.m_DeadTimeHS);
            break;
        case WifiCmd_e::e_SetCurrentLimits:
            g_PWMController.SetCurrentLimits(m_Packages.m_SetPWMCurrentLimits.m_LimitLow, m_Packages.m_SetPWMCurrentLimits.m_LimitHigh);
        case WifiCmd_e::e_SetPWMTemperatureResolution:
            DS18B20::SetResolution(m_Packages.m_SetValue8.m_Value);
            break;
        case WifiCmd_e::e_GetRadioStatus:
            //printf_P(PSTR("Get Radio status\n"));
            m_SendRadioStatusLinkIDs |= BIT8(linkID, 1);
            break;
        case WifiCmd_e::e_GetPWMStatus:
            //printf_P(PSTR("Get PWM status\n"));
            m_SendPWMStatusLinkIDs |= BIT8(linkID, 1);
            break;
        case WifiCmd_e::e_WritePWMModulationData:
            m_ModulationDataRequestSent = false;
            m_ModulationBytesToReceive += m_Packages.m_SetValue16.m_Value;
            //printf_P(PSTR("Received modulation data: %d\n"), m_ModulationBytesToReceive);
            break;
        case WifiCmd_e::e_SetPWMModulationSampleRate:
            g_PWMController.SetModulationSampleRate(m_Packages.m_SetValue16.m_Value);
            break;
        case WifiCmd_e::e_SetPWMModulationScale:
            g_PWMController.SetModulationScale(m_Packages.m_SetPWMModulationScale.m_Multiplier, m_Packages.m_SetPWMModulationScale.m_RightShifter);
            break;
        default:
            printf_P(PSTR("ERROR: NetIF::ProcessMessage() received unknown command: %d\n"), m_Packages.m_Header.m_Command);
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ESP8266_ConnectionChangedCallback(uint8_t linkID, bool isConnected)
{
    g_NetIF.ConnectionChanged(linkID, isConnected);
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ESP8266_DataReceivedCallback(uint8_t linkID, int16_t size)
{
    g_NetIF.DataReceived(linkID, size);
}

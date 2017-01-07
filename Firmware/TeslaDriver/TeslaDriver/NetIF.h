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


#ifndef __NETIF_H__
#define __NETIF_H__

#include "ESP8266/ESP8266.h"

#include "WifiProtocol.h"

class Event;

class NetIF
{
public:
    enum State
    {
        e_StateUninitialized,
        e_StateIdle,
        e_StateInitRadio,
        e_StateConnectingToAP,
        e_StateRunning
    };

    NetIF();

    void Initialize();
    void RestartRadio();    
    void Stop();
//    void InitRadio();
    void ReconfigureRadio();
    void UpdateHotspot();
    void Run(const Event& event);

    State GetState() const { return m_State; }

    void ConnectionChanged(uint8_t linkID, bool isConnected);
    void DataReceived(uint8_t linkID, int16_t size);

private:
    enum PendingReplies_e
    {
        e_PendingReplyPong             = 0x01,
        e_PendingReplyGetSystemInfo    = 0x02,
        e_PendingReplyReadVoltageRails = 0x04
    };
    void ProcessMessage(uint8_t linkID);
    
    
    union
    {
        uint8_t                   m_Buffer[1];
        WifiPackageHeader         m_Header;
        WifiSetVal8               m_SetValue8;
        WifiSetVal16              m_SetValue16;
        WifiSetVal32              m_SetValue32;
        WifiSetPreferredBootMode  m_SetPreferredBootMode;
        WifiSetPWMDeadTime        m_SetPWMDeadTime;
        WifiSetPWMCurrentLimits   m_SetPWMCurrentLimits;
        WifiSetPWMModulationScale m_SetPWMModulationScale;
    } m_Packages;
    
    static_assert(sizeof(m_Packages) <= 256, "ERROR: Wifi message larget than 256 bytes.");

    State    m_State = e_StateUninitialized;
    uint32_t m_NextConnectionTime; // Clock::Time() for next attempt to connect to AP.
    uint8_t  m_CurReconnectionDelay = 1; // Current delay between AP connection attempts in seconds.
    
    int8_t   m_CurrentLinkID = -1;
    uint8_t  m_BytesReceived = 0;
    uint8_t  m_PendingReplies = 0;

    uint8_t  m_SendPWMStatusLinkIDs = 0;
    uint8_t  m_SendRadioStatusLinkIDs = 0;
    
    int16_t  m_ModulationBytesToReceive = 0;
    uint8_t  m_ModulationReceivedLinkIDs = 0;
    bool     m_ModulationDataRequestSent = false;
        
};

extern NetIF g_NetIF;

#endif //__NETIF_H__

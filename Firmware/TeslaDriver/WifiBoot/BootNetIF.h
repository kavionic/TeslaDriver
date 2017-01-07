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


#ifndef __BOOTNETIF_H__
#define __BOOTNETIF_H__

#include "ESP8266/ESP8266.h"

#include "WifiProtocol.h"

class Event;

class BootNetIF // : public WifiSocket
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

    void Initialize();
    void RestartRadio();    
    void Stop();
    void ReconfigureRadio();
    void Run(/*const Event& event*/);

    State GetState() const { return m_State; }

//    void ConnectionChanged(uint8_t linkID, bool isConnected);
    void DataReceived(uint8_t linkID, int16_t size);

private:
    void     ProcessMessage(uint8_t linkID);
    void     EraseAppSection();
    void     FlushFlashPages();
    uint32_t GetFlashCRC(uint32_t startAddress, uint32_t endAddress);
    
    union
    {
        uint8_t                   m_Buffer[1];
        WifiPackageHeader         m_Header;
        WifiSetVal8               m_SetValue8;
        WifiSetVal16              m_SetValue16;
        WifiSetVal32              m_SetValue32;
        WifiSetPreferredBootMode  m_SetPreferredBootMode;
        WifiBeginWriteFlashPages  m_BeginWriteFlashPages;
        WifiGetFlashCRC           m_GetFLashCRC;
        WifiReadFlashPages        m_ReadFlashPages;
    } m_Packages;
    
    static_assert(sizeof(m_Packages) <= 256, "ERROR: Wifi message larget than 256 bytes.");

    enum PendingReplies_e
    {
        e_PendingReplyPong,
        e_PendingReplyGetSystemInfo,
        e_PendingReplyFlashBufferUpdate,
        e_PendingReplyFlashEraseDone,
        e_PendingReplyFlashWriteBegin
    };

    State    m_State = e_StateUninitialized;
    uint32_t m_NextConnectionTime; // Clock::Time() for next attempt to connect to AP.
    uint8_t  m_CurReconnectionDelay = 1; // Current delay between AP connection attempts in seconds.
    
    int8_t   m_CurrentLinkID = -1;
    uint8_t  m_BytesReceived = 0;
    uint8_t  m_SendRadioStatusLinkIDs = 0;
    uint8_t  m_PendingReplies = 0;    
    uint32_t  m_FlashWriteAddress = 0;
    uint16_t  m_FlashBytesReceived = 0;
    uint16_t  m_FlashBytesToReceive = 0;
    uint16_t  m_BytesInPageBuffer = 0;
    uint32_t  m_FlashCRCStart = 0;
    uint32_t  m_FlashCRCEnd = 0;
    static const uint16_t FLASH_PAGE_BUFFER_SIZE = SPM_PAGESIZE;
    uint8_t   m_FlashPageBuffer[FLASH_PAGE_BUFFER_SIZE];
};
/*
inline void ESP8266_ConnectionChanged(uint8_t linkID, bool isConnected)
{
    //    g_BootNetIF.ConnectionChanged(linkID, isConnected);
}*/

extern BootNetIF g_BootNetIF;

#endif //__BOOTNETIF_H__

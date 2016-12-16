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
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Misc/Utils.h"

#include "BootNetIF.h"
#include "Hardware.h"
#include "Config.h"

#include "Misc/Clock.h"
#include "Misc/SpinTimer.h"


///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void BootNetIF::RestartRadio()
{
//    uint32_t startTime = Clock::GetTime();
    DB_LOG(DP_INFO, PSTRL("Initializing radio."));
    g_WifiDevice.ResetRadio();
    g_WifiDevice.EnableEcho(false);
    if (!g_WifiDevice.SetBaudrate()){
        return;
    }
    g_WifiDevice.SetMuxMode(WifiMuxMode_e::e_Multiple);
//    DB_LOG(DP_INFO, PSTRL("Radio reset took "), Clock::GetTime() - startTime, PSTRL("mS"));
    DB_LOG(DP_INFO, PSTRL("Radio reset done."));    
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void BootNetIF::Stop()
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

void BootNetIF::ReconfigureRadio()
{
    RestartRadio();
    m_NextConnectionTime = 0;
    m_State = e_StateInitRadio;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////
/*
void BootNetIF::UpdateHotspot()
{
    g_WifiDevice.DisconnectFromAP();
    g_WifiDevice.SetupHotspot(Config::IsHotspotEnabled(), Config::GetHotspotSSID(), Config::GetHotspotPassword(), Config::GetHotspotChannel(), Config::GetHotspotEncryption(), 4, Config::IsHotspotHidden());
}*/

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void BootNetIF::Run(/*const Event& event*/)
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
                DB_LOG(DP_INFO, PSTRL("Connecting to stored AP "), selectedAP);
                if (selectedAP != -1)
                {
                    const WifiStoredAccessPoint* accessPoint = Config::GetAccessPoint(selectedAP);
                    DB_LOG(DP_INFO, PSTRL("'"), accessPoint->m_SSID, PSTRL("' : '"), accessPoint->m_Password, PSTRL("'"));
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
            DB_LOG(DP_ERROR, PSTRL("Poll result: "), result);
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
            
//            g_WifiDevice.UpdateStationAddress();
//            DB_LOG(DP_INFO, PSTRL("Signal strength: "), g_WifiDevice.GetSignalStrength());
            
            m_BytesReceived             = 0;
            m_CurrentLinkID             = -1;
            m_SendRadioStatusLinkIDs    = 0;
            m_PendingReplies            = 0;
            m_FlashBytesToReceive       = 0;
            m_FlashBytesReceived        = 0;
            m_BytesInPageBuffer         = 0;
            m_FlashCRCStart             = 0;
            m_FlashCRCEnd               = 0;


            if (!g_WifiDevice.StartServer(42)){
                return;
            }            
            m_State = e_StateRunning;
            DB_LOG(DP_INFO, PSTRL("Done starting server."));
        } else if (I32(m_NextConnectionTime - Clock::GetTime()) <= 0) {
            DB_LOG(DP_ERROR, PSTRL("ERROR: Timeout while connecting to AP."));
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
            reply.InitSystemInfo(WifiBootMode_e::e_BootLoader, WifiBootMode_e(eeprom_read_byte(&g_EEPROMunmapped.global.m_PreferredBootMode)), Clock::GetTime());

            g_WifiDevice.SendIPData(m_CurrentLinkID, &reply, sizeof(reply));
        }
        if (m_SendRadioStatusLinkIDs != 0)
        {
            m_SendRadioStatusLinkIDs = 0;
            WifiGetRadioStatusReply reply;
            WifiPackageHeader::InitMsg(reply, WifiCmd_e::e_GetRadioStatusReply);
        
            reply.m_Strength = g_WifiDevice.GetSignalStrength();        
            g_WifiDevice.SendIPData(m_CurrentLinkID, &reply, sizeof(reply));
        }
        if (m_PendingReplies & BIT8(e_PendingReplyFlashEraseDone, 1))
        {
            m_PendingReplies &= U8(~BIT8(e_PendingReplyFlashEraseDone, 1));
            WifiPackageHeader reply;
            WifiPackageHeader::InitMsg(reply, WifiCmd_e::e_EraseProgramSectionDone);
            g_WifiDevice.SendIPData(m_CurrentLinkID, &reply, sizeof(reply));
        }
        if (m_PendingReplies & BIT8(e_PendingReplyFlashBufferUpdate, 1))
        {
            m_PendingReplies &= U8(~BIT8(e_PendingReplyFlashBufferUpdate, 1));
            WifiSetVal16 reply;
            WifiPackageHeader::InitMsg(reply, WifiCmd_e::e_WriteFlashBytesReceivedReply);
            reply.m_Value = m_FlashBytesReceived;
//            DB_LOG(DP_CRITICAL, PSTRL("Buf size: "), reply.m_Value);
            g_WifiDevice.SendIPData(m_CurrentLinkID, &reply, sizeof(reply));
        }
        if (m_PendingReplies & BIT8(e_PendingReplyFlashWriteBegin, 1))
        {
            m_PendingReplies &= U8(~BIT8(e_PendingReplyFlashWriteBegin, 1));
            WifiSetVal16 reply;
            WifiPackageHeader::InitMsg(reply, WifiCmd_e::e_BeginWriteFlashReply);
            reply.m_Value = SPM_PAGESIZE;
            g_WifiDevice.SendIPData(m_CurrentLinkID, &reply, sizeof(reply));
        }
        if (m_FlashCRCStart != m_FlashCRCEnd)
        {
            WifiSetVal32 reply;
            WifiPackageHeader::InitMsg(reply, WifiCmd_e::e_GetFlashCRCReply);
            reply.m_Value = GetFlashCRC(m_FlashCRCStart, m_FlashCRCEnd);
            g_WifiDevice.SendIPData(m_CurrentLinkID, &reply, sizeof(reply));
            m_FlashCRCStart = 0;
            m_FlashCRCEnd = 0;
        }
        uint16_t bytesReceived = g_WifiDevice.GetAndClearChannelBytesReceived(m_CurrentLinkID);
        if (bytesReceived != 0)
        {
            WifiSetVal16 msg;
            WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_BytesReceivedReply);
            msg.m_Value = bytesReceived;
            g_WifiDevice.SendIPData(m_CurrentLinkID, &msg, sizeof(msg));
        }
  }        
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////
/*
void BootNetIF::ConnectionChanged(uint8_t linkID, bool isConnected)
{
    if ( isConnected ) {
        m_CurrentLinkID = linkID;
    }
    DB_LOG(DP_INFO, PSTRL("Connection changed: "), isConnected);
}*/
    
///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void BootNetIF::DataReceived(uint8_t linkID, int16_t size)
{
    m_CurrentLinkID = linkID;

    while(size > 0)
    {
        if (m_FlashBytesToReceive != 0 && m_FlashBytesToReceive != m_FlashBytesReceived)
        {
            for(;;)
            {
                int16_t len = min(m_FlashBytesToReceive - m_FlashBytesReceived, U16(size));
                if ( len == 0 ) {
                    break;
                }
                if (m_BytesInPageBuffer + len > FLASH_PAGE_BUFFER_SIZE)
                {
                    len = FLASH_PAGE_BUFFER_SIZE - m_BytesInPageBuffer;
                }
                int16_t bytesReceived = g_WifiDevice.Read(m_FlashPageBuffer + m_BytesInPageBuffer, len);
                if (bytesReceived == len)
                {
                    size -= bytesReceived;
                    m_FlashBytesReceived += bytesReceived;
                    m_BytesInPageBuffer += bytesReceived;
                    if (m_FlashBytesReceived == m_FlashBytesToReceive || m_BytesInPageBuffer == FLASH_PAGE_BUFFER_SIZE)
                    {
//                        DB_LOG(DP_CRITICAL, PSTRL("Flush pages: "), m_BytesInPageBuffer, ", ", m_FlashBytesReceived, ", ", m_FlashBytesToReceive);
                        FlushFlashPages();
//                        m_BytesInPageBuffer = 0;
                        if (m_FlashBytesReceived == m_FlashBytesToReceive)
                        {
                            m_FlashCRCEnd += m_FlashBytesToReceive - 1;
                            //DB_LOG(DP_CRITICAL, PSTRL("All pages received"));
                            m_FlashBytesReceived = 0;
                            m_FlashBytesToReceive = 0;
                            break;
                        }
                    }
                    m_PendingReplies |= BIT8(e_PendingReplyFlashBufferUpdate, 1);
                }
                else
                {
                    DB_LOG(DP_ERROR, PSTRL("ERROR: Failed to receive flash data."));
                    size = 0;
                    break;
                }
            }
        }
        if (m_BytesReceived < sizeof(m_Packages.m_Header.m_Magic))
        {
            uint8_t len = min(I16(sizeof(m_Packages.m_Header.m_Magic) - m_BytesReceived), size);
            if (g_WifiDevice.Read(m_Packages.m_Buffer + m_BytesReceived, len) != len) {
                DB_LOG(DP_ERROR, PSTRL("ERROR: Failed to receive package magic number!"));
                m_BytesReceived = 0;
                g_WifiDevice.CloseConnection(linkID);
                return;
            }
            m_BytesReceived += len;
            size -= len;
            if (m_BytesReceived == sizeof(m_Packages.m_Header.m_Magic) && m_Packages.m_Header.m_Magic != WIFI_PACKAGE_MAGIC)
            {
                DB_LOG(DP_ERROR, PSTRL("ERROR: Received wifi package with invalid magic number: "), m_Packages.m_Header.m_Magic);
                m_BytesReceived = 0;
                g_WifiDevice.CloseConnection(linkID);
                return;
            }
        }
        if (m_BytesReceived < sizeof(m_Packages.m_Header))
        {
            uint8_t len = min(I16(sizeof(m_Packages.m_Header) - m_BytesReceived), size);
            if (g_WifiDevice.Read(m_Packages.m_Buffer + m_BytesReceived, len) != len) {
                DB_LOG(DP_ERROR, PSTRL("ERROR: Failed to receive package header!"));
                m_BytesReceived = 0;
                g_WifiDevice.CloseConnection(linkID);
                return;
            }
            m_BytesReceived += len;            
            size -= len;
        }
        if (m_BytesReceived >= sizeof(m_Packages.m_Header))
        {
            if (m_Packages.m_Header.m_PackageLength > sizeof(m_Packages))
            {
                DB_LOG(DP_ERROR, PSTRL("ERROR: Invalid message size: "), m_Packages.m_Header.m_PackageLength, PSTRL(" ("), sizeof(m_Packages), PSTRL(")"));
                m_BytesReceived = 0;
                g_WifiDevice.CloseConnection(linkID);
                return;
            }
            int16_t len = min<int16_t>(m_Packages.m_Header.m_PackageLength - m_BytesReceived, size);
            if (g_WifiDevice.Read(m_Packages.m_Buffer + m_BytesReceived, len) != len) {
                DB_LOG(DP_ERROR, PSTRL("ERROR: Failed to receive package body!"));
                m_BytesReceived = 0;
                g_WifiDevice.CloseConnection(linkID);
                return;
            }
            m_BytesReceived += len;
            size -= len;
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

void BootNetIF::ProcessMessage(uint8_t linkID)
{
    
    switch(m_Packages.m_Header.m_Command)
    {
        case WifiCmd_e::e_Ping:
            m_PendingReplies |= BIT8(e_PendingReplyPong, 1);
            break;
        case WifiCmd_e::e_SetPreferredBootMode:
            eeprom_write_byte(&g_EEPROMunmapped.global.m_PreferredBootMode, U8(m_Packages.m_SetPreferredBootMode.m_BootMode));
            eeprom_busy_wait();
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
/*        case WifiCmd_e::e_SetPWMTemperatureResolution:
            DS18B20::SetResolution(m_Packages.m_SetValue8.m_Value);
            break;*/
        case WifiCmd_e::e_GetRadioStatus:
            m_SendRadioStatusLinkIDs |= BIT8(linkID, 1);
            break;
        case WifiCmd_e::e_EraseProgramSection:
            //DB_LOG(DP_CRITICAL, PSTRL("Start erase"));
            EraseAppSection();
            m_PendingReplies |= BIT8(e_PendingReplyFlashEraseDone, 1);
            //DB_LOG(DP_CRITICAL, PSTRL("Erase done"));
            break;
        case WifiCmd_e::e_BeginWriteFlash:
            m_FlashWriteAddress = m_Packages.m_BeginWriteFlashPages.m_Address;
            m_FlashBytesToReceive = m_Packages.m_BeginWriteFlashPages.m_Length;
            m_FlashCRCStart = m_FlashWriteAddress;
            m_FlashCRCEnd   = m_FlashWriteAddress;
            //DB_LOG(DP_CRITICAL, PSTRL("Begin: "), m_FlashWriteAddress, ", ", m_FlashBytesToReceive);
            m_PendingReplies |= BIT8(e_PendingReplyFlashBufferUpdate, 1);
            break;
        case WifiCmd_e::e_GetFlashCRC:
            m_FlashCRCStart = m_Packages.m_GetFLashCRC.m_StartAddress;
            m_FlashCRCEnd = m_Packages.m_GetFLashCRC.m_EndAddress;
            break;        
        default:
            DB_LOG(DP_ERROR, PSTRL("ERROR: BootNetIF::ProcessMessage() received unknown command: "), uint8_t(m_Packages.m_Header.m_Command));
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void BootNetIF::EraseAppSection()
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        while(NVM.STATUS & (NVM_NVMBUSY_bm | NVM_FBUSY_bm));
        uint8_t oldRAMPZ = RAMPZ;
        RAMPZ = 0;
        NVM.CMD = NVM_CMD_ERASE_APP_gc;
        asm volatile("sts %0, %1\n\t"
                     "spm\n\t" : :  "i" (_SFR_MEM_ADDR(CCP)), "r" ((uint8_t)(CCP_SPM_gc)), "z" (0));
        RAMPZ = oldRAMPZ;
        while(NVM.STATUS & (NVM_NVMBUSY_bm | NVM_FBUSY_bm));
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void BootNetIF::FlushFlashPages()
{
    uint32_t pageAddress = m_FlashWriteAddress;
    for (uint16_t j = 0; j < m_BytesInPageBuffer; j += 2)
    {
        uint16_t word = m_FlashPageBuffer[j];
        word |= U16(m_FlashPageBuffer[j+1]) << 8;
                
        while(NVM.STATUS & (NVM_NVMBUSY_bm | NVM_FBUSY_bm));
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            uint8_t oldRAMPZ = RAMPZ;
    
            NVM.CMD = NVM_CMD_LOAD_FLASH_BUFFER_gc;
            RAMPZ = U8(m_FlashWriteAddress>>16);
            asm volatile("movw  r0, %1\n\t"
                         "spm\n\t"
                         "clr  r1\n\t"
                            :
                            : "z" ((uint16_t)(m_FlashWriteAddress)), "r" ((uint16_t)(word))
                            : "r0");
            RAMPZ = oldRAMPZ;
        }
        m_FlashWriteAddress += 2;
    }            
//            DB_LOG(DP_CRITICAL, PSTRL("Flush "), pageAddress);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        uint8_t oldRAMPZ = RAMPZ;
        NVM.CMD = NVM_CMD_WRITE_APP_PAGE_gc;
        RAMPZ = U8(pageAddress>>16);
        asm volatile(   "sts %0, %1\n\t"
                        "spm\n\t"
                        "clr  r1\n\t"
                        :
                        : "i" (_SFR_MEM_ADDR(CCP)),
                        "r" ((uint8_t)(CCP_SPM_gc)),
                        "z" ((uint16_t)(pageAddress)));
        RAMPZ = oldRAMPZ;
    }
    while(NVM.STATUS & (NVM_NVMBUSY_bm | NVM_FBUSY_bm));
    m_BytesInPageBuffer = 0;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

uint32_t BootNetIF::GetFlashCRC(uint32_t startAddress, uint32_t endAddress)
{
    uint32_t result;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        while(NVM.STATUS & (NVM_NVMBUSY_bm | NVM_FBUSY_bm));
        NVM.CMD = NVM_CMD_FLASH_RANGE_CRC_gc;
        NVM.ADDR0 = startAddress & 0xff;
        NVM.ADDR1 = (startAddress>>8) & 0xff;
        NVM.ADDR2 = (startAddress>>16) & 0xff;
        NVM.DATA0 = endAddress & 0xff;
        NVM.DATA1 = (endAddress>>8) & 0xff;
        NVM.DATA2 = (endAddress>>16) & 0xff;
        CCP = CCP_IOREG_gc;
        NVM.CTRLA = NVM_CMDEX_bm;
        while(NVM.STATUS & (NVM_NVMBUSY_bm | NVM_FBUSY_bm));
        result = NVM.DATA0 | (U16(NVM.DATA1) << 8) | (U32(NVM.DATA2) << 16);
    }
    return result;        
}

//void ESP8266_ConnectionChangedCallback(uint8_t linkID, bool isConnected)
//{
//}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ESP8266_DataReceivedCallback(uint8_t linkID, int16_t size)
{
    g_BootNetIF.DataReceived(linkID, size);
}

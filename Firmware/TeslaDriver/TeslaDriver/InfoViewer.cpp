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

#include <stdio.h>
#include <string.h>

#include <avr/pgmspace.h>

#include "InfoViewer.h"
#include "EventSys.h"
#include "Hardware.h"
#include "SystemStats.h"
#include "NetIF.h"
#include "PWMController.h"
#include "FanController.h"
#include "Config.h"

#include "Misc/Utils.h"
#include "DS18B20.h"

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void InfoViewer::Initialize()
{ 
    m_CurrentPage     = InfoViewPage::e_PageSummary;
    m_PrevPage        = InfoViewPage::e_PageCount;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

bool InfoViewer::Run(const Event& event)
{
    switch(event.type)
    {
        case EventID::e_KeyDown:
            g_Display.SetCursor(event.value, 2);
            g_Display.WriteCharacter('*');
            break;
        case EventID::e_KeyRepeat:
        {
            static uint16_t repeatCount;
            g_Display.SetCursor(event.value, 2);
            g_Display.Printf_P(PSTR("%d"), repeatCount % 10);
            repeatCount++;
            break;
        }            
        case EventID::e_KeyUp:
            g_Display.SetCursor(event.value, 2);
            g_Display.WriteCharacter('-');
            break;
        case EventID::e_KnobTurned:
        {
            static int16_t knobValue;
            knobValue += event.value;
            g_Display.SetCursor(0, 3);
            g_Display.Printf_P(PSTR("%d (%u)"), knobValue, QUAD_DECODER_TIMER.CNT >> 2);
            g_Display.ClearToEndOfLine();
            break;
        }
        default:
            break;
    }
    
    bool pageChanged = m_CurrentPage != m_PrevPage;
    
    if (pageChanged) {
        g_Display.ClearDisplay();
    }        
    switch(m_CurrentPage)
    {
        case InfoViewPage::e_PageSummary:
            DisplaySummary(event, pageChanged);
            break;
        case InfoViewPage::e_PageNetwork1:
            DisplayNetwork1(event, pageChanged);
            break;
        case InfoViewPage::e_PageNetwork2:
            DisplayNetwork2(event, pageChanged);
            break;
        case InfoViewPage::e_PagePWM:
            DisplayPWM(event, pageChanged);
            break;
        case InfoViewPage::e_PageSystem:
            DisplaySystem(event, pageChanged);
            break;
        case InfoViewPage::e_PageCount:
            break;
    }
    m_PrevPage = m_CurrentPage;
    switch(event.type)
    {
        case EventID::e_KnobTurned:
            m_CurrentPage = static_cast<InfoViewPage::Enum>(clamp<int8_t>(0, I8(InfoViewPage::e_PageCount) - 1, I8(m_CurrentPage) + event.value));
            return true;
        default:
            return false;
    }        
    return true;
 }

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

bool InfoViewer::DisplaySummary(const Event& event, bool initialize)
{
    if (event.type == EventID::e_TimeTick100)
    {
        g_Display.SetCursor(0, 0);
        
        switch(g_NetIF.GetState())
        {
            case NetIF::e_StateIdle:
                g_Display.Printf_P(PSTR("Stopped..."));
                break;
            case NetIF::e_StateInitRadio:
                g_Display.Printf_P(PSTR("Initializing..."));
                break;
            case NetIF::e_StateConnectingToAP:
                g_Display.Printf_P(PSTR("Connecting to AP..."));
                break;
            case NetIF::e_StateRunning:
            {
                uint32_t serverAddress = (Config::IsHotspotEnabled()) ? g_WifiDevice.GetHotspotAddress() : g_WifiDevice.GetStationAddress();
            
                g_Display.Printf_P(PSTR("%d.%d.%d.%d"), U16((serverAddress >> 24) & 0xff), U16((serverAddress >> 16) & 0xff), U16((serverAddress >> 8) & 0xff), U16(serverAddress & 0xff));
                
                if (Config::IsHotspotEnabled()) {
                    g_Display.ClearTo(16);
                    g_Display.Printf_P(PSTR("HtSp"));
                } else {
                    g_Display.ClearTo(16);
                    g_Display.Printf_P(PSTR("%d%%"), ESP8266::GetSignalPercentage(g_WifiDevice.GetSignalStrength()));
                }                    
                break;
            }
            default:
                g_Display.Printf_P(PSTR("Invalid state %d"), g_NetIF.GetState());
                break;
        }
        g_Display.ClearToEndOfLine();
        g_Display.SetCursor(0, 1);
        g_Display.Printf_P(PSTR("F: %dkHz"), U16(U32(CPU_FREQ) * 4 / 1000 / g_PWMController.GetPeriod()));
        g_Display.ClearToEndOfLine();
        
        g_Display.SetCursor(10, 1);
        g_Display.Printf_P(PSTR("PWM: %d%%"), U32(g_PWMController.GetDutyCycle()) * 100 >> 16);
        g_Display.ClearToEndOfLine();
        
        g_Display.SetCursor(0, 2);
        g_Display.Printf_P(PSTR("FAN RPM: %lu / %lu\n"), g_FanController.GetRPM(FanController::e_Fan1), g_FanController.GetRPM(FanController::e_Fan2));
        g_Display.ClearToEndOfLine();

        g_Display.SetCursor(0, 3);
//        uint8_t curTempReadingIndex = DS18B20::GetReadingIndex();
//        if (curTempReadingIndex != tempReadingIndex)
        {
//            tempReadingIndex = curTempReadingIndex;
            
            uint16_t temp1 = DS18B20::GetTemp1();
            uint16_t temp2 = DS18B20::GetTemp2();
            g_Display.Printf_P(PSTR("TEMP: %d.%d/%d.%d"), temp1 / 16, (temp1 % 16) * 100 / 16, temp2 / 16, (temp2 % 16) * 100 / 16 );
        }        
//        g_Display.Printf_P(PSTR("%lu / %lu\n"), U32(CPU_FREQ) / g_FanController.GetRPM(FanController::e_Fan1), CPU_FREQ / g_FanController.GetRPM(FanController::e_Fan2));
        g_Display.ClearToEndOfLine();
    }
   return false;    
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

bool InfoViewer::DisplayNetwork1(const Event& event, bool initialize)
{
    static int8_t apScrollOffset;
    if (initialize)
    {
        apScrollOffset = 0;
    }
    if (event.type == EventID::e_TimeTick1000)
    {
        apScrollOffset++;
    }
    if (event.type == EventID::e_TimeTick100)
    {
        g_Display.SetCursor(0, 0);
        if (Config::IsHotspotEnabled())
        {
            uint32_t serverAddress = g_WifiDevice.GetHotspotAddress();
            uint32_t serverGateway = g_WifiDevice.GetHotspotGateway();
            uint32_t serverNetMask = g_WifiDevice.GetHotspotNetMask();
                
            g_Display.Printf_P(PSTR("Adr: %d.%d.%d.%d"), U16((serverAddress >> 24) & 0xff), U16((serverAddress >> 16) & 0xff), U16((serverAddress >> 8) & 0xff), U16(serverAddress & 0xff));
            g_Display.ClearToEndOfLine();
            g_Display.SetCursor(0, 1);
            g_Display.Printf_P(PSTR("GW: %d.%d.%d.%d"), U16((serverGateway >> 24) & 0xff), U16((serverGateway >> 16) & 0xff), U16((serverGateway >> 8) & 0xff), U16(serverGateway & 0xff));
            g_Display.ClearToEndOfLine();
            g_Display.SetCursor(0, 2);
            g_Display.Printf_P(PSTR("NM: %d.%d.%d.%d"), U16((serverNetMask >> 24) & 0xff), U16((serverNetMask >> 16) & 0xff), U16((serverNetMask >> 8) & 0xff), U16(serverNetMask & 0xff));
            g_Display.ClearToEndOfLine();
            const char* hotspotSSID = "ThisIsAVeryLongConnectedAPName12"; // g_WifiDevice.GetConnectedAP();

            g_Display.SetCursor(0, 3);
            g_Display.Printf_P(PSTR("HS: "));
            g_Display.PrintScrollingText(hotspotSSID, WIFI_MAX_SSID_LEN, false, 4, 16, 3, &apScrollOffset);
        }
        else
        {
            if (g_WifiDevice.GetStatusFlags() & ESP8266::e_StatusConnectedToAP)
            {
                uint32_t serverAddress = g_WifiDevice.GetStationAddress();
            
                g_Display.Printf_P(PSTR("Adr: %d.%d.%d.%d"), U16((serverAddress >> 24) & 0xff), U16((serverAddress >> 16) & 0xff), U16((serverAddress >> 8) & 0xff), U16(serverAddress & 0xff));
                g_Display.ClearToEndOfLine();
                g_Display.SetCursor(0, 1);
                g_Display.Printf_P(PSTR("AP: "));
                
                const char* connectedAP = "ThisIsAVeryLongConnectedAPName12"; // g_WifiDevice.GetConnectedAP();

                g_Display.PrintScrollingText(connectedAP, WIFI_MAX_SSID_LEN, false, 4, 16, 1, &apScrollOffset);            
                int16_t signalStrength = g_WifiDevice.GetSignalStrength();
            
                if ( signalStrength < -100 ) {
                    signalStrength = 0;
                    } else if (signalStrength > -50) {
                    signalStrength = 100;
                    } else {
                    signalStrength = 100 + (50 + signalStrength) * 2;
                }
                g_Display.SetCursor(0, 2);
                g_Display.Printf_P(PSTR("Signal: %d%%"), signalStrength);
                g_Display.PrintHorizontalBar(3, signalStrength, 0);
            }
            else
            {
                g_Display.Printf_P(PSTR("NO NETWORK"));
                g_Display.ClearToEndOfLine();
            }
        }            
    }        
    return false;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

bool InfoViewer::DisplayNetwork2(const Event& event, bool initialize)
{
    if (event.type == EventID::e_TimeTick100)
    {
        g_Display.SetCursor(0, 0);
        if (Config::IsHotspotEnabled())
        {
            uint32_t serverAddress = g_WifiDevice.GetHotspotAddress();
            g_Display.Printf_P(PSTR("Adr: %d.%d.%d.%d"), U16((serverAddress >> 24) & 0xff), U16((serverAddress >> 16) & 0xff), U16((serverAddress >> 8) & 0xff), U16(serverAddress & 0xff));
            g_Display.ClearToEndOfLine();
        }
        else
        {
            if (g_WifiDevice.GetStatusFlags() & ESP8266::e_StatusConnectedToAP)
            {
                uint32_t       serverAddress = g_WifiDevice.GetStationAddress();
                const uint8_t* stationMAC     = g_WifiDevice.GetStationMAC();
            
                g_Display.Printf_P(PSTR("Adr: %d.%d.%d.%d"), U16((serverAddress >> 24) & 0xff), U16((serverAddress >> 16) & 0xff), U16((serverAddress >> 8) & 0xff), U16(serverAddress & 0xff));
                g_Display.ClearToEndOfLine();
                g_Display.SetCursor(0, 1);
                g_Display.Printf_P(PSTR("%02x:%02x:%02x:%02x:%02x:%02x"), stationMAC[0], stationMAC[1], stationMAC[2], stationMAC[3], stationMAC[4], stationMAC[5]);
                int16_t signalStrength = g_WifiDevice.GetSignalStrength();
            
                if ( signalStrength < -100 ) {
                    signalStrength = 0;
                    } else if (signalStrength > -50) {
                    signalStrength = 100;
                    } else {
                    signalStrength = 100 + (50 + signalStrength) * 2;
                }
                g_Display.SetCursor(0, 2);
                g_Display.Printf_P(PSTR("Signal: %d%%"), signalStrength);
                g_Display.PrintHorizontalBar(3, signalStrength, 0);
            }
            else
            {
                g_Display.Printf_P(PSTR("NO NETWORK"));
                g_Display.ClearToEndOfLine();
            }
        }            
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

bool InfoViewer::DisplayPWM(const Event& event, bool initialize)
{
    if (event.type == EventID::e_TimeTick100)
    {
        g_Display.SetCursor(0, 0);
        g_Display.Printf_P(PSTR("F: %dkHz"), U16(U32(CPU_FREQ) * 4 / 1000 / g_PWMController.GetPeriod()));
        g_Display.ClearToEndOfLine();
        
        g_Display.SetCursor(10, 0);
        g_Display.Printf_P(PSTR("PWM: %d%%"), U32(g_PWMController.GetDutyCycle()) * 100 >> 16);
        g_Display.ClearToEndOfLine();
    }        
    return false;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

bool InfoViewer::DisplaySystem(const Event& event, bool initialize)
{
    if (event.type == EventID::e_TimeTick500)
    {
        g_Display.SetCursor(0, 0);
        g_Display.Printf_P(PSTR("FPS: %ld / %ld"), CPU_FREQ / g_SystemStats.m_CyclesPerFrameMax, CPU_FREQ / g_SystemStats.m_CyclesPerFrameAvg);
        g_Display.ClearToEndOfLine();

        g_Display.SetCursor(0, 1);
        uint32_t maxDSPCycles = (CPU_FREQ / g_PWMController.GetModulationSampleRate()) * PWMController::MODULATION_DMA_BUFFER_SIZE;
        g_Display.Printf_P(PSTR("DSP: %04u/%04u %02u/%02u"), g_SystemStats.m_DSPCyclesMin, g_SystemStats.m_DSPCyclesMax, U16(U32(g_SystemStats.m_DSPCyclesMin) * 100 / maxDSPCycles), U16(U32(g_SystemStats.m_DSPCyclesMax) * 100 / maxDSPCycles) );
        g_Display.ClearToEndOfLine();
    
        g_Display.SetCursor(0, 2);
        g_Display.Printf_P(PSTR("WIFI Rcv buf: %d"), g_SystemStats.m_WifiReceiveBufferMax);
        g_Display.ClearToEndOfLine();
    }        
    return false;
}

// This file is part of TeslaTuner.
//
// Copyright (C) 2016 Kurt Skauen <http://kavionic.com/>
//
// TeslaTuner is free software : you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// TeslaTuner is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TeslaTuner. If not, see < http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeviceConnector.h"


DeviceConnector::DeviceConnector(QObject* parent) : QObject(parent)
{
//    connect(&m_DeviceSocket, &QAbstractSocket::stateChanged, this, &DeviceConnector::SlotConnectionStateChanged);
    connect(&m_DeviceSocket, &QTcpSocket::readyRead, this, &DeviceConnector::SlotSocketDataReady);
}


DeviceConnector::~DeviceConnector()
{
}

void DeviceConnector::SlotSocketDataReady()
{
    size_t size = m_DeviceSocket.bytesAvailable();
    while (size > 0)
    {
        if (m_ResponseBytesReceived < sizeof(m_ResponsePackages.m_Header.m_Magic))
        {
            int len = std::min(sizeof(m_ResponsePackages.m_Header.m_Magic) - m_ResponseBytesReceived, size);
            m_DeviceSocket.read(m_ResponsePackages.m_Buffer + m_ResponseBytesReceived, len);
            size -= len;
            m_ResponseBytesReceived += len;
            if (m_ResponseBytesReceived == sizeof(m_ResponsePackages.m_Header.m_Magic) && m_ResponsePackages.m_Header.m_Magic != WIFI_PACKAGE_MAGIC)
            {
                printf("ERROR: Received wifi package with invalid magic number: %04x\n", m_ResponsePackages.m_Header.m_Magic);
                m_ResponseBytesReceived = 0;
                continue;
            }
        }
        if (m_ResponseBytesReceived < sizeof(m_ResponsePackages.m_Header))
        {
            int len = std::min(sizeof(m_ResponsePackages.m_Header) - m_ResponseBytesReceived, size);
            m_DeviceSocket.read(m_ResponsePackages.m_Buffer + m_ResponseBytesReceived, len);
            m_ResponseBytesReceived += len;
            size -= len;
        }
        if (m_ResponseBytesReceived >= sizeof(m_ResponsePackages.m_Header))
        {
            if (m_ResponsePackages.m_Header.m_PackageLength > sizeof(m_ResponsePackages))
            {
                printf("ERROR: Invalid message size: %u\n", m_ResponsePackages.m_Header.m_PackageLength);
                m_ResponseBytesReceived = 0;
                continue;
            }
            uint16_t len = std::min<uint16_t>(m_ResponsePackages.m_Header.m_PackageLength - m_ResponseBytesReceived, size);
            m_DeviceSocket.read(m_ResponsePackages.m_Buffer + m_ResponseBytesReceived, len);
            m_ResponseBytesReceived += len;
            size -= len;
            if (m_ResponseBytesReceived == m_ResponsePackages.m_Header.m_PackageLength)
            {
//                ProcessResponse();
                m_ResponseBytesReceived = 0;
            }
        }
    }
}

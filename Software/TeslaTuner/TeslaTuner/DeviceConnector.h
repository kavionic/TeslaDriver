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

#pragma once

class DeviceConnector : public QObject
{
public:
    DeviceConnector(QObject* parent);
    ~DeviceConnector();

private slots:
    void SlotConnectionStateChanged(QAbstractSocket::SocketState state);
    void SlotSocketDataReady();

private:
    QTcpSocket m_DeviceSocket;

    union
    {
        char                    m_Buffer[1];
        WifiPackageHeader       m_Header;
        WifiSetVal16            m_SetVal16;
        WifiSetVal32            m_SetVal32;
        WifiGetSystemInfoReply  m_GetSystemInfoReply;
        WifiGetRadioStatusReply m_GetRadioStatusReply;
        WifiGetPWMStatusReply   m_GetPWMStatusReply;
    } m_ResponsePackages;
    int m_ResponseBytesReceived = 0;

};


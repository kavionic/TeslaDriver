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

#ifndef CONNECTIONPANEL_H
#define CONNECTIONPANEL_H

#include <QtWidgets/qframe.h>
#include "ui_ConnectionPanel.h"

class ConnectionPanel : public QFrame, public Ui_ConnectionPanelClass
{
    Q_OBJECT

public:
    ConnectionPanel(QWidget *parent = 0);
    ~ConnectionPanel();

    void SetSocket(QTcpSocket* socket);

    void ReconnectDevice();

    void LoadSettings();
    void SaveSettings();

    void HandleNetMsg(const WifiGetRadioStatusReply& msg);
    void HandleNetPong();
    void Run();
    void ConnectionStateChanged(QAbstractSocket::SocketState state);

private slots:
    void SlotDeviceAddressSelected(int index);
    void SlotDevicePortChanged();
    void SlotConnectCheckBoxChanged();


private:

    void SendPing();
    void RequestRadioStatus();


    QTcpSocket* m_DeviceSocket = nullptr;

    QElapsedTimer m_TimeSincePingSent;
    QElapsedTimer m_TimeSinceRadioStatusRequest;
    int64_t       m_Latency;
    bool                  m_IsPingPending = false;
    bool                  m_IsRadioStatusUpdatePending = false;
    bool                  m_IsRadioStatusValid = false;

};

#endif // CONNECTIONPANEL_H

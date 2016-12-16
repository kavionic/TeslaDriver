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
#include "ConnectionPanel.h"

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

ConnectionPanel::ConnectionPanel(QWidget *parent)
    : QFrame(parent)
{
    setupUi(this);

    connect(m_DeviceAddressView, SIGNAL(activated(int)), this, SLOT(SlotDeviceAddressSelected(int)));
    //    connect(m_DeviceAddressView, &QComboBox::currentTextChanged, this, &ConnectionPanel::SlotDeviceAddressSelected);
    connect(m_DevicePortView, &QLineEdit::editingFinished, this, &ConnectionPanel::SlotDevicePortChanged);
    connect(m_ConnectCheckBox, &QCheckBox::stateChanged, this, &ConnectionPanel::SlotConnectCheckBoxChanged);

    m_ConnectionStatusView->setText("NO CONNECTION");

    m_DeviceAddressView->setMaxCount(30);

}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

ConnectionPanel::~ConnectionPanel()
{

}

void ConnectionPanel::SetSocket(QTcpSocket* socket)
{
    m_DeviceSocket = socket;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ConnectionPanel::ReconnectDevice()
{
    m_DeviceSocket->disconnectFromHost();
    m_DeviceSocket->abort();
    if (m_ConnectCheckBox->checkState())
    {
        m_DeviceSocket->connectToHost(m_DeviceAddressView->currentText(), m_DevicePortView->text().toInt());
        m_DeviceSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ConnectionPanel::LoadSettings()
{
    QSettings settings;

    int addressCount = settings.beginReadArray("Network/DeviceAddressHistory");
    for (int i = 0 ; i < addressCount; ++i)
    {
        settings.setArrayIndex(i);
        m_DeviceAddressView->addItem(settings.value("address").toString());
    }
    settings.endArray();
    m_DevicePortView->setText(QString::asprintf("%d", settings.value("Network/DevicePort").toInt()));
    m_ConnectCheckBox->setChecked(settings.value("Network/Connected").toBool());
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ConnectionPanel::SaveSettings()
{
    QSettings settings;

    settings.beginWriteArray("Network/DeviceAddressHistory", m_DeviceAddressView->count());
    for (int i = 0, e = m_DeviceAddressView->count() ; i < e; ++i)
    {
        settings.setArrayIndex(i);
        settings.setValue("address", m_DeviceAddressView->itemText(i));
    }
    settings.endArray();
    settings.setValue("Network/DevicePort", m_DevicePortView->text().toInt());

    settings.setValue("Network/Connected", m_ConnectCheckBox->isChecked());
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ConnectionPanel::HandleNetMsg(const WifiGetRadioStatusReply& msg)
{
    m_IsRadioStatusUpdatePending = false;
    m_IsRadioStatusValid = true;

    int strength = msg.m_Strength;
    if (strength < -100) {
        strength = 0;
    }
    else if (strength > -50) {
        strength = 100;
    }
    else {
        strength = 100 + (50 + strength) * 2;
    }
    //            m_SignalStrengthView->setText(QString::asprintf("%d%%", strength));
    m_SignalStrengthView->setValue(strength);
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ConnectionPanel::HandleNetPong()
{
    m_Latency = m_TimeSincePingSent.nsecsElapsed();
    m_IsPingPending = false;
    m_LatencyView->setText(QString::asprintf("%.3fmS", double(m_Latency/1000)/1000.0));
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ConnectionPanel::Run()
{
    if (m_DeviceSocket->state() == QAbstractSocket::UnconnectedState)
    {
        ReconnectDevice();
        return;
    }
    if (m_IsPingPending)
    {
        int64_t latency = m_TimeSincePingSent.nsecsElapsed();

        if (latency > 2e9) {
            m_IsPingPending = false;
            m_DeviceSocket->disconnectFromHost();
            m_DeviceSocket->abort();
        } else if (latency > m_Latency + 500e3) {
            m_Latency = latency;
            m_LatencyView->setText(QString::asprintf("%.3fmS", double(m_Latency/1000)/1000.0));
        }
    } else if (m_TimeSincePingSent.elapsed() > 1000) {
        SendPing();
    }
    if (!m_IsRadioStatusUpdatePending && m_TimeSinceRadioStatusRequest.elapsed() > 1000) {
        RequestRadioStatus();
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ConnectionPanel::ConnectionStateChanged(QAbstractSocket::SocketState state)
{
    m_IsRadioStatusUpdatePending = false;
    m_IsRadioStatusValid = false;

    m_LatencyView->setText("--");

    switch (state)
    {
    case QAbstractSocket::UnconnectedState:
        m_ConnectionStatusView->setText("NO CONNECTION");
        ReconnectDevice();
        break;
    case QAbstractSocket::HostLookupState:
        m_ConnectionStatusView->setText("LOOKUP");
        break;
    case QAbstractSocket::ConnectingState:
        m_ConnectionStatusView->setText("CONNECTING");
        break;
    case QAbstractSocket::ConnectedState:
        m_ConnectionStatusView->setText("CONNECTED");
        SendPing();
        RequestRadioStatus();
        break;
    case QAbstractSocket::ClosingState:
        m_ConnectionStatusView->setText("CLOSING");
        break;
    default:
        break;
    }

}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ConnectionPanel::SlotDeviceAddressSelected(int index)
{
    QString address = m_DeviceAddressView->itemText(index);
    if (index != 0) {
        m_DeviceAddressView->setItemText(index, m_DeviceAddressView->itemText(0));
        m_DeviceAddressView->setItemText(0, address);
        m_DeviceAddressView->setCurrentIndex(0);
    }
    //    m_DeviceAddressView->setEditText(address);
    //    m_DeviceAddressView->removeItem(index);
    //    m_DeviceAddressView->insertItem(0, address);
    //    m_DeviceAddressView->setCurrentIndex(0);
    ReconnectDevice();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ConnectionPanel::SlotDevicePortChanged()
{
    ReconnectDevice();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ConnectionPanel::SlotConnectCheckBoxChanged()
{
    ReconnectDevice();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ConnectionPanel::SendPing()
{
    if (!m_IsPingPending && m_DeviceSocket->state() == QAbstractSocket::ConnectedState)
    {
        m_IsPingPending = true;

        WifiPackageHeader msg;
        WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_Ping);
        m_DeviceSocket->write(reinterpret_cast<const char*>(&msg), sizeof(msg));
        m_TimeSincePingSent.start();
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void ConnectionPanel::RequestRadioStatus()
{
    if (!m_IsRadioStatusUpdatePending && m_DeviceSocket->state() == QAbstractSocket::ConnectedState)
    {
        m_IsRadioStatusUpdatePending = true;

        WifiPackageHeader msg;
        WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_GetRadioStatus);
        m_DeviceSocket->write(reinterpret_cast<const char*>(&msg), sizeof(msg));
        m_TimeSinceRadioStatusRequest.start();
    }
}

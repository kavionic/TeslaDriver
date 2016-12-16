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

#ifndef FIRMWAREWRITER_H
#define FIRMWAREWRITER_H

#include <QtWidgets/qframe.h>
#include "ui_FirmwareWriter.h"

class FirmwareWriter : public QFrame, public Ui::FirmwareWriterClass
{
    Q_OBJECT

public:
    FirmwareWriter(QWidget *parent = 0);
    ~FirmwareWriter();

    void SetSocket(QTcpSocket* socket);

    void LoadSettings();
    void SaveSettings();

    void Run();
    //void ConnectionStateChanged(QAbstractSocket::SocketState state);

    //void HandleNetGetSystemInfoReply(const WifiGetSystemInfoReply& reply);
    void HandleNetEraseProgramSectionDone();
    void HandleNetFlashBytesReceivedReply(uint16_t bytesReceived);
    void HandleNetFlashCRCReply(uint32_t crc);

    bool IsFlashing() { return m_State != e_StateIdle; }

    void DeviceConnected(bool isConnected);
    private slots:
    void SlotHexFileSelected(int index);
    void SlotBrowseHexFile();
    void SlotWriteButton();
    void SlotRebootButton();

private:
    enum State_e
    {
        e_StateWaitingForConnection,
        e_StateWaitingForReboot,
        e_StateIdle,
        e_StateWaitingForErase,
        e_StateWritingProgram,
        e_StateVerifyingFlash
    };
    void SetState(State_e state);
    bool WriteHexFile();
    void AbortFlashUpdate();
    void UpdateProgressDisplay();

    void DeviceSetBootMode(WifiBootMode_e mode, bool doReboot);


    State_e m_State = e_StateWaitingForConnection;
    bool m_RebootAfterUpdate = false;

    QTcpSocket* m_DeviceSocket = nullptr;

    QString     m_HexFilePath;

    std::map<uint32_t, std::vector<uint8_t>> m_PendingFlashSections;

    int32_t m_TotalBytesToSend = 0;
    int32_t m_TotalBytesSent = 0;
    int32_t m_SectionBytesSent = -1;
    int32_t m_SectionBytesReceived = 0;
    uint32_t m_CurrentFlashCRC = 0;

    bool m_IsProgramErasePending = false;
//    uint32_t m_TargetFlashBufferSpace = 0;
};

#endif // FIRMWAREWRITER_H

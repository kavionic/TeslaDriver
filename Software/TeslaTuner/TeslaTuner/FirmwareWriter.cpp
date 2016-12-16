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
#include "FirmwareWriter.h"
#include "TeslaTuner.h"

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

static uint32_t CalculateCRC24(const std::vector<uint8_t>& buffer, uint32_t start, int32_t end)
{
    const int32_t CRC32_POLY = 0x0080001b;
    uint32_t crc = 0;

    for (int32_t i = start; i < end; i += 2)
    {
        uint32_t data = (int)buffer[i] | ((int)buffer[i + 1] << 8);
        uint32_t tmp1 = (crc << 1) & 0x00FFFFFE;
        int32_t  tmp2 = (crc & (1 << 23)) ? 0x00FFFFFF : 0;
        crc = ((tmp1 ^ data) ^ (tmp2 & CRC32_POLY)) & 0x00FFFFFF;
    }
    return crc;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

FirmwareWriter::FirmwareWriter(QWidget *parent)
    : QFrame(parent)
{
    setupUi(this);
    connect(m_HexFileBrowseButton, &QToolButton::clicked, this, &FirmwareWriter::SlotBrowseHexFile);
    connect(m_HexFileDropdown, SIGNAL(activated(int)), this, SLOT(SlotHexFileSelected(int)));
    connect(m_WriteButton, &QToolButton::clicked, this, &FirmwareWriter::SlotWriteButton);
    connect(m_RebootButton, &QToolButton::clicked, this, &FirmwareWriter::SlotRebootButton);

    m_HexFileDropdown->setMaxCount(30);
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

FirmwareWriter::~FirmwareWriter()
{

}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void FirmwareWriter::SetSocket(QTcpSocket* socket)
{
    m_DeviceSocket = socket;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void FirmwareWriter::LoadSettings()
{
    QSettings settings;
    int pathCount = settings.beginReadArray("FirmwareWriter/HexFileHistory");
    for (int i = 0 ; i < pathCount; ++i)
    {
        settings.setArrayIndex(i);
        m_HexFileDropdown->addItem(settings.value("path").toString());
    }
    settings.endArray();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void FirmwareWriter::SaveSettings()
{
    QSettings settings;

    settings.beginWriteArray("FirmwareWriter/HexFileHistory", m_HexFileDropdown->count());
    for (int i = 0, e = m_HexFileDropdown->count() ; i < e; ++i)
    {
        settings.setArrayIndex(i);
        settings.setValue("path", m_HexFileDropdown->itemText(i));
    }
    settings.endArray();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void FirmwareWriter::SlotHexFileSelected(int index)
{
    QString path = m_HexFileDropdown->itemText(index);
    if (index != 0) {
        m_HexFileDropdown->setItemText(index, m_HexFileDropdown->itemText(0));
        m_HexFileDropdown->setItemText(0, path);
        m_HexFileDropdown->setCurrentIndex(0);
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void FirmwareWriter::SlotBrowseHexFile()
{
    QFileDialog dlg(this, "Open firmware file.", m_HexFileDropdown->currentText(), "Intel HEX file (*.hex);;Any files (*.*)");
    dlg.open();
    if (dlg.exec() == QDialog::Accepted)
    {
        QStringList files = dlg.selectedFiles();
        if (!files.empty())
        {
            m_HexFilePath = files[0];
            m_HexFileDropdown->insertItem(0, files[0]);
            m_HexFileDropdown->setCurrentIndex(0);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void FirmwareWriter::SlotWriteButton()
{
    if (TeslaTuner::GetDeviceInfo().m_CurrentBootMode == WifiBootMode_e::e_BootLoader)
    {
        m_RebootAfterUpdate = false;
        WriteHexFile();
    }
    else
    {
        DeviceSetBootMode(WifiBootMode_e::e_BootLoader, true);
        SetState(e_StateWaitingForReboot);
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void FirmwareWriter::SlotRebootButton()
{
    WifiPackageHeader msg;
    WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_Reboot);
    m_DeviceSocket->write(reinterpret_cast<const char*>(&msg), sizeof(msg));
    m_DeviceSocket->close();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void FirmwareWriter::Run()
{
    if (m_State == e_StateWritingProgram)
    {
        while (m_State == e_StateWritingProgram && !m_PendingFlashSections.empty() /*&& (m_SectionBytesSent == -1 || m_TargetFlashBufferSpace !=0)*/)
        {
            auto segment = m_PendingFlashSections.begin();
            if (m_SectionBytesSent == -1)
            {
                WifiBeginWriteFlashPages msg;
                WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_BeginWriteFlash);
                msg.m_Address = segment->first;
                msg.m_Length = segment->second.size();
                m_DeviceSocket->write(reinterpret_cast<const char*>(&msg), sizeof(msg));
                m_SectionBytesSent = 0;
                m_SectionBytesReceived = 0;
            }
            else
            {
                int32_t receiverBufferSpace = TeslaTuner::GetDeviceInfo().m_NetworkBufferSize - m_SectionBytesSent + m_SectionBytesReceived;
                if (receiverBufferSpace <= 0) {
                    break;
                }
                int len = std::min(receiverBufferSpace, int32_t(segment->second.size() - m_SectionBytesSent));
                len = m_DeviceSocket->write(reinterpret_cast<const char*>(&segment->second[m_SectionBytesSent]), len);
                m_SectionBytesSent += len;
                m_TotalBytesSent += len;

                if (m_SectionBytesSent == segment->second.size())
                {
                    m_CurrentFlashCRC = CalculateCRC24(segment->second, 0, segment->second.size() - 1); // qChecksum((const char*)segment->second.data(), segment->second.size());
                    m_PendingFlashSections.erase(segment);
                    m_SectionBytesSent = -1;
                    SetState(e_StateVerifyingFlash);
                }
                UpdateProgressDisplay();
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////


void FirmwareWriter::DeviceConnected(bool isConnected)
{
    if (isConnected)
    {
        if (m_State == e_StateWaitingForReboot) {
            SetState(e_StateIdle);
            m_RebootAfterUpdate = true;
            WriteHexFile();
        } else {
            SetState(e_StateIdle);
        }
    }
    else
    {
        if (m_State != e_StateWaitingForReboot)
        {
            AbortFlashUpdate();
            SetState(e_StateWaitingForConnection);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void FirmwareWriter::HandleNetEraseProgramSectionDone()
{
    if (m_State == e_StateWaitingForErase)
    {
        SetState(e_StateWritingProgram);
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void FirmwareWriter::HandleNetFlashBytesReceivedReply(uint16_t bytesReceived)
{
    m_SectionBytesReceived = bytesReceived;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void FirmwareWriter::HandleNetFlashCRCReply(uint32_t crc)
{
    if (m_State == e_StateVerifyingFlash)
    {
        if (crc == m_CurrentFlashCRC)
        {
            if (!m_PendingFlashSections.empty())
            {
                SetState(e_StateWritingProgram);
            }
            else
            {
                DeviceSetBootMode(WifiBootMode_e::e_Application, m_RebootAfterUpdate);
                m_RebootAfterUpdate = false;
                SetState(e_StateWaitingForConnection);
            }
        }
        else
        {
            AbortFlashUpdate();
            SetState(e_StateIdle);
            QMessageBox dialog(QMessageBox::Critical, "Flash CRC error!", "Failed to update flash. CRC check failed.", QMessageBox::Ok, this);

            dialog.open();
            dialog.exec();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void FirmwareWriter::SetState(State_e state)
{
    m_State = state;
    UpdateProgressDisplay();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

bool FirmwareWriter::WriteHexFile()
{
    if (TeslaTuner::GetDeviceInfo().m_CurrentBootMode != WifiBootMode_e::e_BootLoader)
    {
        return false;
    }
    QFile file(m_HexFileDropdown->currentText());

    m_TotalBytesToSend = 0;
    m_TotalBytesSent = 0;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        std::vector<uint8_t> buffer;
        uint32_t currentAddress = ~0;
        uint32_t startAddress = ~0;
        uint32_t extendedAddress = 0;
        for (;;)
        {
            QByteArray line = file.readLine();
            if (line.isEmpty())
            {
                break;
            }
            if (line[0] == ':')
            {
                int dataLength = strtol(line.mid(1, 2).data(), nullptr, 16);
                int recordAddress = strtol(line.mid(3, 4).data(), nullptr, 16) | extendedAddress;
                int recordType = strtol(line.mid(7, 2).data(), nullptr, 16);
                if (recordType == 0)
                {
                    if (recordAddress != currentAddress)
                    {
                        if (!buffer.empty())
                        {
                            m_TotalBytesToSend += buffer.size();
                            m_PendingFlashSections[startAddress].swap(buffer);
                        }
                        currentAddress = recordAddress;
                        startAddress = recordAddress;
                    }
                    for (int i = 0; i < dataLength; ++i)
                    {
                        uint8_t data = strtol(line.mid(9+i*2, 2).data(), nullptr, 16);
                        buffer.push_back(data);
                        const int32_t MAX_BUFFER_SIZE = 16*1024;
                        if (buffer.size() == MAX_BUFFER_SIZE)
                        {
                            m_TotalBytesToSend += buffer.size();
                            m_PendingFlashSections[startAddress].swap(buffer);
                            startAddress += MAX_BUFFER_SIZE;
                        }
                    }
                    currentAddress += dataLength;
                }
                else if (recordType == 1) // End of file.
                {
                    break;
                }
                else if (recordType == 4) // Extended linear address record.
                {
                    extendedAddress = strtol(line.mid(9, 4).data(), nullptr, 16) << 16;
                }
                else
                {
                    printf("Unknown HEX record: %d\n", recordType);
                }
            }
        }
        if (!buffer.empty())
        {
            while ((buffer.size() % TeslaTuner::GetDeviceInfo().m_FlashPageSize) != 0) buffer.push_back(0xff);
//            for (int i = 0 ; i < 4096; ++i) buffer.push_back(0xff);
            m_TotalBytesToSend += buffer.size();
            m_PendingFlashSections[startAddress].swap(buffer);
        }
        SetState(e_StateWaitingForErase);
        WifiPackageHeader msg;
        WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_EraseProgramSection);
        m_DeviceSocket->write(reinterpret_cast<const char*>(&msg), sizeof(msg));
        return true;
    }
    return false;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void FirmwareWriter::AbortFlashUpdate()
{
    m_TotalBytesToSend = 0;
    m_TotalBytesSent = 0;
    m_SectionBytesSent = -1;
    m_PendingFlashSections.clear();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void FirmwareWriter::UpdateProgressDisplay()
{
//    if (m_State == e_StateWritingProgram)
    {
        if (m_State != e_StateIdle)
        {
            m_ProgressBar->setMaximum(m_TotalBytesToSend);
            m_ProgressBar->setValue(m_TotalBytesSent);
        }
        else
        {
            m_ProgressBar->setMaximum(1);
            m_ProgressBar->setValue(0);
        }
        m_ProgressLabel->setText("Bytes Written: ");
        m_ProgressView->setText(QString::asprintf("%d / %d", m_TotalBytesSent, m_TotalBytesToSend));
    }
/*    else
    {
        m_ProgressBar->setValue(0);
    }*/
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void FirmwareWriter::DeviceSetBootMode(WifiBootMode_e mode, bool doReboot)
{
    WifiSetPreferredBootMode msg;
    WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_SetPreferredBootMode);
    msg.m_BootMode = mode;
    msg.m_Reboot   = doReboot;
    m_DeviceSocket->write(reinterpret_cast<const char*>(&msg), sizeof(msg));

    if (doReboot)
    {
        m_DeviceSocket->close();
    }
}


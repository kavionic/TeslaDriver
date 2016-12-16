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
#include "TeslaTuner.h"

DeviceInfo TeslaTuner::s_DeviceInfo;

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////
#define MOD_BUF_SIZE 1024
TeslaTuner::TeslaTuner(QWidget *parent) : QMainWindow(parent), m_AudioPlayer(this)
{
    QSettings settings;
    setupUi(this);

    connect(m_FrequencySlider, &QAbstractSlider::valueChanged, this, &TeslaTuner::SlotFrequencySliderChanged);
    connect(m_DutyCycleSlider, &QAbstractSlider::valueChanged, this, &TeslaTuner::SlotDutyCycleSliderChanged);

    connect(m_DeadTimeLSSlider, &QAbstractSlider::valueChanged, this, &TeslaTuner::SlotDeadTimeLSSliderChanged);
    connect(m_DeadTimeHSSlider, &QAbstractSlider::valueChanged, this, &TeslaTuner::SlotDeadTimeHSSliderChanged);

    connect(m_CurrentLimitLowSlider, &QAbstractSlider::valueChanged, this, &TeslaTuner::SlotCurrentLimitLowSliderChanged);
    connect(m_CurrentLimitHighSlider, &QAbstractSlider::valueChanged, this, &TeslaTuner::SlotCurrentLimitHighSliderChanged);

    connect(m_TemperatureResolutionSlider, &QAbstractSlider::valueChanged, this, &TeslaTuner::SlotTemperatureResolutionSliderChanged);


    connect(m_AudioFileBrowseButton, &QToolButton::clicked, this, &TeslaTuner::SlotBrowseAudioFile);
    connect(m_AudioFileView, SIGNAL(activated(int)), this, SLOT(SlotAudioPathSelected(int)));
    connect(m_AudioPlayButton, &QToolButton::toggled, this, &TeslaTuner::SlotPlayButton);
    connect(m_AudioStopButton, &QToolButton::clicked, this, &TeslaTuner::SlotStopButton);
    connect(m_AudioPosition, &QAbstractSlider::valueChanged, this, &TeslaTuner::SlotAudioPositionSliderChanged);
    connect(m_VolumeSlider, &QAbstractSlider::valueChanged, this, &TeslaTuner::SlotVolumeSliderChanged);
    connect(m_SampleRateSlider, &QAbstractSlider::valueChanged, this, &TeslaTuner::SlotSampleRateSliderChanged);

    connect(&m_DeviceSocket, &QAbstractSocket::stateChanged, this, &TeslaTuner::SlotConnectionStateChanged);
    connect(&m_DeviceSocket, &QTcpSocket::readyRead, this, &TeslaTuner::SlotSocketDataReady);

    m_ConnectionPanel->SetSocket(&m_DeviceSocket);
    m_ConnectionPanel->LoadSettings();
    m_FirmwareWriter->SetSocket(&m_DeviceSocket);
    m_FirmwareWriter->LoadSettings();
    UpdateTemperatureResolutionValue();
    UpdateFrequencySliderValue();
    UpdateDutyCycleSliderValue();
    UpdateDeadTimeLSSliderValue();
    UpdateDeadTimeHSSliderValue();
    UpdateCurrentLimitLowSliderValue();
    UpdateCurrentLimitHighSliderValue();
    UpdateVolumeSliderValue();
    UpdateSampleRateSliderValue();

    EnableControls(false);

        //m_AudioFileView->setInsertPolicy(QComboBox::InsertAtTop);
    m_AudioFileView->setMaxCount(30);

    int pathCount = settings.beginReadArray("Audio/FileHistory");   
    for (int i = 0 ; i < pathCount; ++i)
    {
        settings.setArrayIndex(i);
        m_AudioFileView->addItem(settings.value("path").toString());
    }
    settings.endArray();

    connect(&m_AudioPlayer, &AudioPlayer::SignalStateChanged, this, &TeslaTuner::SlotAudioPlayerStateChanged);
    connect(&m_AudioPlayer, &AudioPlayer::SignalDurationChanged, this, &TeslaTuner::SlotAudioDurationChanged);
    connect(&m_AudioPlayer, &AudioPlayer::SignalPlayPositionChanged, this, &TeslaTuner::SlotAudioPositionChanged);
    m_PWMStatus.m_Frequency = 32000000;
    m_PWMStatus.m_PWMPeriode = 1600;

    connect(&m_RefreshTimer, &QTimer::timeout, this, &TeslaTuner::SlotRefreshTimer);
//    m_RefreshTimer.setInterval(250);
    m_RefreshTimer.start(1);


    uint32_t baudrate = 3110400;

    int8_t bscale;
    uint16_t bsel;
    
    int32_t smallestOffset = 10000000;

#define CPU_FREQ 32000000
    for (int8_t bscaleCur = -7 ; bscaleCur <= 7 ; ++bscaleCur)
    {
        if (bscaleCur < 0)
        {
            float bscalePow2 = (1<<(-bscaleCur));
            uint16_t bselCur = round(((double(CPU_FREQ)/(8*baudrate) - 1) * bscalePow2));
//            uint16_t bselCur = round(((double(CPU_FREQ)/(8*baudrate) - 1)/pow(2.0, bscaleCur)));
//            uint16_t bselCur = round((1.0/pow(2.0, bscaleCur)) * (CPU_FREQ/(8*baudrate) - 1));
//            uint32_t realBaudrate = CPU_FREQ / (8*( pow(2.0, bscaleCur) * bselCur + 1 ));
            uint32_t realBaudrate = CPU_FREQ / (8*(bselCur / bscalePow2 + 1));
            int32_t offset = abs(int32_t(realBaudrate - baudrate));
            if (offset < smallestOffset)
            {
                bscale = bscaleCur;
                bsel = bselCur;
                smallestOffset = offset;
            }
        }
        else
        {
            uint16_t bselCur = double(CPU_FREQ) / ((1<<bscaleCur) * 8 * baudrate) - 1;
            uint32_t realBaudrate = CPU_FREQ / ((1<<bscaleCur) * 8 * (bselCur + 1));
            int32_t offset = abs(int32_t(realBaudrate - baudrate));
            if (offset < smallestOffset)
            {
                bscale = bscaleCur;
                bsel = bselCur;
                smallestOffset = offset;
            }
        }
    }
//    m_AudioFileView->setCurrentIndex(0);
//    m_DeviceAddressView->setCurrentIndex(0);

    SlotAudioPathSelected(0);
//    m_AudioPlayer.LoadFile(m_AudioFilePath/*"TestAudio.mp3"*/);
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

TeslaTuner::~TeslaTuner()
{

}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::closeEvent(QCloseEvent* event)
{
    QSettings settings;

    m_ConnectionPanel->SaveSettings();
    m_FirmwareWriter->SaveSettings();
    settings.beginWriteArray("Audio/FileHistory", m_AudioFileView->count());
    for (int i = 0, e = m_AudioFileView->count() ; i < e; ++i)
    {
        settings.setArrayIndex(i);
        settings.setValue("path", m_AudioFileView->itemText(i));
    }
    settings.endArray();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotConnectionStateChanged(QAbstractSocket::SocketState state)
{
    m_IsStatusUpdatePending = false;
    m_IsModulationDataPending = false;
    m_IsPWMStatusValid = false;

    m_ConnectionPanel->ConnectionStateChanged(state);
//    m_FirmwareWriter->ConnectionStateChanged(state);
    UpdatePWMStatusView();

    if (state != QAbstractSocket::ConnectedState)
    {
        m_FirmwareWriter->DeviceConnected(false);
    }
    switch (state)
    {
    case QAbstractSocket::UnconnectedState:
        EnableControls(false);
        break;
    case QAbstractSocket::HostLookupState:
        break;
    case QAbstractSocket::ConnectingState:
        break;
    case QAbstractSocket::ConnectedState:
        WifiPackageHeader msg;
        WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_GetSystemInfo);
        m_DeviceSocket.write(reinterpret_cast<const char*>(&msg), sizeof(msg));
        m_TimeSinceSystemInfoRequest.start();

//        m_UpdateSliders = true;
//        RequestPWMStatus();
        break;
    case QAbstractSocket::ClosingState:
        break;
    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotAudioPlayerStateChanged(AudioPlayer::State_e state)
{
    m_AudioStopButton->setEnabled(m_AudioPlayButton->isEnabled() && state != AudioPlayer::e_StateStopped);
    m_AudioPlayButton->setChecked(state == AudioPlayer::e_StatePlaying);
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotAudioDurationChanged(double duration)
{
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotAudioPositionChanged(double position)
{
    if (!m_AudioPosition->isSliderDown())
    {
        m_AudioPosition->setSliderPosition(int(m_AudioPosition->maximum() * m_AudioPlayer.GetPlayPosition() + 0.5));
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotAudioPositionSliderChanged(int value)
{
    if (m_AudioPosition->isSliderDown())
    {
        m_AudioPlayer.SetPlayPosition(double(m_AudioPosition->sliderPosition()) / double(m_AudioPosition->maximum()));
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotSocketDataReady()
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
                ProcessResponse();
                m_ResponseBytesReceived = 0;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::ProcessResponse()
{
    switch (m_ResponsePackages.m_Header.m_Command)
    {
        case WifiCmd_e::e_Pong:
            m_ConnectionPanel->HandleNetPong();
            break;
        case WifiCmd_e::e_GetSystemInfoReply:
            s_DeviceInfo.m_CurrentBootMode   = m_ResponsePackages.m_GetSystemInfoReply.m_BootMode;
            s_DeviceInfo.m_PreferredBootMode = m_ResponsePackages.m_GetSystemInfoReply.m_PreferredBootMode;
            s_DeviceInfo.m_NetworkBufferSize = m_ResponsePackages.m_GetSystemInfoReply.m_NetworkBufferSize;
            s_DeviceInfo.m_FlashPageSize     = m_ResponsePackages.m_GetSystemInfoReply.m_ApplicationFlashPageSize;
            s_DeviceInfo.m_FlashStartAddress = m_ResponsePackages.m_GetSystemInfoReply.m_ApplicationFlashStart;
            s_DeviceInfo.m_FlashSize         = m_ResponsePackages.m_GetSystemInfoReply.m_ApplicationFlashSize;
            m_TimeSinceSystemInfoRequest.invalidate();

            setWindowTitle((s_DeviceInfo.m_CurrentBootMode == WifiBootMode_e::e_Application) ? "TeslaTuner (Normal)" : "TeslaTuner (Boot Loader)");
            if (s_DeviceInfo.m_CurrentBootMode == WifiBootMode_e::e_Application)
            {
                m_UpdateSliders = true;
                RequestPWMStatus();
            }
            m_FirmwareWriter->DeviceConnected(true);
//            m_FirmwareWriter->HandleNetGetSystemInfoReply(m_ResponsePackages.m_GetSystemInfoReply);
            break;
        case WifiCmd_e::e_GetRadioStatusReply:
            m_ConnectionPanel->HandleNetMsg(m_ResponsePackages.m_GetRadioStatusReply);
            break;
        case WifiCmd_e::e_GetPWMStatusReply:
            //m_Latency = m_TimeSincePWMStatusRequest.nsecsElapsed();
            m_PWMStatus = m_ResponsePackages.m_GetPWMStatusReply;
//            WifiGetPWMStatusReply msg;
//            m_DeviceSocket.read(reinterpret_cast<char*>(&m_PWMStatus), sizeof(m_PWMStatus));
            m_IsStatusUpdatePending = false;
            m_IsPWMStatusValid = true;
            UpdatePWMStatusView();
//            RequestPWMStatus();
            break;
        case WifiCmd_e::e_WritePWMModulationDataDone:
            m_IsModulationDataPending = false;
            SendModulationData();
            break;
        case WifiCmd_e::e_EraseProgramSectionDone:
            m_FirmwareWriter->HandleNetEraseProgramSectionDone();
            break;
        case WifiCmd_e::e_WriteFlashBytesReceivedReply:
            m_FirmwareWriter->HandleNetFlashBytesReceivedReply(m_ResponsePackages.m_SetVal16.m_Value);
            break;
        case WifiCmd_e::e_GetFlashCRCReply:
            m_FirmwareWriter->HandleNetFlashCRCReply(m_ResponsePackages.m_SetVal32.m_Value);
            break;
        default:
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotRefreshTimer()
{
    if (m_TimeSinceSystemInfoRequest.isValid())
    {
        if (m_TimeSinceSystemInfoRequest.elapsed() > 1000)
        {
            m_DeviceSocket.close();
            m_TimeSinceSystemInfoRequest.invalidate();
        }
        return;
    }
    m_FirmwareWriter->Run();
    if (!m_FirmwareWriter->IsFlashing())
    {
        m_ConnectionPanel->Run();
        if (!m_IsStatusUpdatePending && m_TimeSincePWMStatusRequest.elapsed() > 100) {
            RequestPWMStatus();
        }
        /*    if (!m_IsModulationDataPending && (m_TimeSinceModulationSent.elapsed() - m_ModulationSendTime) > 1000) {
                SendModulationData();
                }*/
        if (!m_IsModulationDataPending && m_AudioPlayer.GetState() == AudioPlayer::e_StatePlaying && (m_TimeSinceModulationSent.elapsed() - m_ModulationSendTime) > 100) {
            SendModulationData();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotFrequencySliderChanged(int value)
{
    uint16_t period = m_PWMStatus.m_Frequency * 4 / value;
    if (m_DeviceSocket.state() == QAbstractSocket::ConnectedState && m_IsPWMStatusValid && m_PWMStatus.m_PWMPeriode != period)
    {
        WifiSetVal16 msg;
        WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_SetPWMPeriod);
        msg.m_Value = period;
        m_DeviceSocket.write(reinterpret_cast<const char*>(&msg), sizeof(msg));
    }
    UpdateFrequencySliderValue();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::UpdateFrequencySliderValue()
{
    uint16_t period = m_PWMStatus.m_Frequency * 4 / m_FrequencySlider->value();
    m_FrequencySliderValue->setText(QString::asprintf("%.3fkHz", double(m_PWMStatus.m_Frequency * 4) / double(period) / 1000.0));
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotDutyCycleSliderChanged(int value)
{
    if (m_DeviceSocket.state() == QAbstractSocket::ConnectedState && m_IsPWMStatusValid && value != m_PWMStatus.m_PWMDutyCycle)
    {
        WifiSetVal16 msg;
        WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_SetPWMDutyCycle);
        msg.m_Value = value;

        m_DeviceSocket.write(reinterpret_cast<const char*>(&msg), sizeof(msg));
    }
    UpdateDutyCycleSliderValue();
    RequestPWMStatus();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::UpdateDutyCycleSliderValue()
{
    m_DutyCycleSliderValue->setText(QString::asprintf("%.2f%%", double(m_DutyCycleSlider->value()) * 100.0 / 65536.0));
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotDeadTimeLSSliderChanged(int value)
{
    if (m_DeviceSocket.state() == QAbstractSocket::ConnectedState && m_IsPWMStatusValid && m_PWMStatus.m_PWMDeadTimeLS != value)
    {
        WifiSetPWMDeadTime msg;
        WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_SetPWMDeadTime);
        msg.m_DeadTimeLS = value;
        msg.m_DeadTimeHS = m_DeadTimeHSSlider->value();

        m_DeviceSocket.write(reinterpret_cast<const char*>(&msg), sizeof(msg));
    }
    UpdateDeadTimeLSSliderValue();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotDeadTimeHSSliderChanged(int value)
{
    if (m_DeviceSocket.state() == QAbstractSocket::ConnectedState && m_IsPWMStatusValid && m_PWMStatus.m_PWMDeadTimeHS != value)
    {
        WifiSetPWMDeadTime msg;
        WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_SetPWMDeadTime);
        msg.m_DeadTimeLS = m_DeadTimeLSSlider->value();
        msg.m_DeadTimeHS = value;

        m_DeviceSocket.write(reinterpret_cast<const char*>(&msg), sizeof(msg));
    }
    UpdateDeadTimeHSSliderValue();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotCurrentLimitLowSliderChanged(int value)
{
    if (m_DeviceSocket.state() == QAbstractSocket::ConnectedState && m_IsPWMStatusValid && m_PWMStatus.m_PWMCurrentLimitLow != value)
    {
        WifiSetPWMCurrentLimits msg;
        WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_SetCurrentLimits);
        msg.m_LimitLow= value;
        msg.m_LimitHigh = m_CurrentLimitHighSlider->value();

        m_DeviceSocket.write(reinterpret_cast<const char*>(&msg), sizeof(msg));
    }
    UpdateCurrentLimitLowSliderValue();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotCurrentLimitHighSliderChanged(int value)
{
    if (m_DeviceSocket.state() == QAbstractSocket::ConnectedState && m_IsPWMStatusValid && m_PWMStatus.m_PWMCurrentLimitHigh != value)
    {
        WifiSetPWMCurrentLimits msg;
        WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_SetCurrentLimits);
        msg.m_LimitLow = m_CurrentLimitLowSlider->value();
        msg.m_LimitHigh = value;

        m_DeviceSocket.write(reinterpret_cast<const char*>(&msg), sizeof(msg));
    }
    UpdateCurrentLimitHighSliderValue();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::UpdateDeadTimeLSSliderValue()
{
    m_DeadTimeLSSliderValue->setText(QString::asprintf("%.2fnS", double(m_DeadTimeLSSlider->value()) * 1e9 / double(m_PWMStatus.m_Frequency)));
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::UpdateDeadTimeHSSliderValue()
{
    m_DeadTimeHSSliderValue->setText(QString::asprintf("%.2fnS", double(m_DeadTimeHSSlider->value()) * 1e9 / double(m_PWMStatus.m_Frequency)));
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::UpdateCurrentLimitLowSliderValue()
{
    m_CurrentLimitLowSliderValue->setText(QString::asprintf("%.0f", double(m_CurrentLimitLowSlider->value())));
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::UpdateCurrentLimitHighSliderValue()
{
    m_CurrentLimitHighSliderValue->setText(QString::asprintf("%.0f", double(m_CurrentLimitHighSlider->value())));
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotTemperatureResolutionSliderChanged(int value)
{
    if (m_DeviceSocket.state() == QAbstractSocket::ConnectedState && m_IsPWMStatusValid && m_PWMStatus.m_TemperatureResolution != value)
    {
        WifiSetVal8 msg;
        WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_SetPWMTemperatureResolution);
        msg.m_Value = value;

        m_DeviceSocket.write(reinterpret_cast<const char*>(&msg), sizeof(msg));
    }
    UpdateTemperatureResolutionValue();
    RequestPWMStatus();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::UpdateTemperatureResolutionValue()
{
    m_TemperatureResolutionSliderValue->setText(QString::asprintf("%d bits", m_TemperatureResolutionSlider->value()));

}


///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotVolumeSliderChanged(int value)
{
    WifiSetPWMModulationScale msg;
    WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_SetPWMModulationScale);

    double scale = double(m_PWMStatus.m_PWMPeriode) * double(value) / (100.0*255.0);

    double smallestError = std::numeric_limits<double>::max();
    for (int shifter = 0 ; shifter < 16 ; ++shifter)
    {
        int multiplier = scale * (1 << shifter);
        if (multiplier < 255) {
            double realScale = double(multiplier) / double(1<<shifter);
            double error = abs(realScale - scale);
            if (error < smallestError)
            {
                smallestError = error;
                msg.m_Multiplier = multiplier;
                msg.m_RightShifter = shifter;
            }
        }
    }
    if (msg.m_Multiplier != m_PWMStatus.m_ModulationMultiplier || msg.m_RightShifter != m_PWMStatus.m_ModulationShifter)
    {
        m_DeviceSocket.write(reinterpret_cast<const char*>(&msg), sizeof(msg));
    }
    UpdateVolumeSliderValue();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::UpdateVolumeSliderValue()
{
    m_VolumeValueView->setText(QString::asprintf("%d%%", m_VolumeSlider->value()));
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotSampleRateSliderChanged(int value)
{
    m_AudioPlayer.SetSampleRate(value);
    if (m_DeviceSocket.state() == QAbstractSocket::ConnectedState && m_IsPWMStatusValid && m_PWMStatus.m_ModulationSampleRate != value)
    {
        WifiSetVal16 msg;
        WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_SetPWMModulationSampleRate);
        msg.m_Value = value;
        m_DeviceSocket.write(reinterpret_cast<const char*>(&msg), sizeof(msg));
    }
    UpdateSampleRateSliderValue();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::UpdateSampleRateSliderValue()
{
    m_SampleRateValueView->setText(QString::asprintf("%d", m_SampleRateSlider->value()));
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotAudioPathSelected(int index)
{
    QString path = m_AudioFileView->itemText(index);
    if (index != 0) {
        m_AudioFileView->setItemText(index, m_AudioFileView->itemText(0));
        m_AudioFileView->setItemText(0, path);
        m_AudioFileView->setCurrentIndex(0);
    }

//    m_AudioFileView->insertItem(0, path);
    //    m_AudioPlayer.Stop();
    m_AudioPlayer.LoadFile(path);
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotBrowseAudioFile()
{
    QFileDialog dlg(this, "Open audio file.", m_AudioFileView->currentText(), "Audio (*.mp3 *.m4a);;Any files (*.*)");
    dlg.open();
    if (dlg.exec() == QDialog::Accepted)
    {
        QStringList files = dlg.selectedFiles();
        if (!files.empty())
        {
            m_AudioFilePath = files[0];
            m_AudioFileView->insertItem(0, files[0]);
            m_AudioFileView->setCurrentIndex(0);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotPlayButton()
{
    bool doPlay = m_AudioPlayButton->isChecked();

/*    if (m_AudioPlayer.GetState() == AudioPlayer::e_StateStopped && m_AudioPlayButton->isChecked())
    {
        QString path = m_AudioFileView->currentText();
        if (!path.isEmpty()) {
            m_AudioPlayer.LoadFile(path);
        } else {
            return;
        }
    }*/
    m_AudioPlayer.Play(doPlay);
/*    if (m_AudioPlayButton->isChecked()){
        m_AudioPlayer.RequestData(&m_DeviceSocket, 1);
//        m_AudioStopButton->setEnabled(true);
    } else {
//        m_AudioStopButton->setEnabled(false);
    }*/
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotStopButton()
{
    m_AudioPlayer.Stop();
    m_AudioPlayButton->setChecked(false);
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SendModulationData()
{
    int bytesRequested = m_AudioPlayer.RequestData(&m_DeviceSocket, 2048);
    if (bytesRequested > 0)
    {
        int64_t elapsed =  m_TimeSinceModulationSent.elapsed();
        int latency = elapsed - m_ModulationSendTime;
        m_ModulationSendTime = elapsed;

        if (latency > 0) {
            m_ModulationBitrate += ((MOD_BUF_SIZE * 1000.0 / latency) - m_ModulationBitrate) * 0.01;
        }
//        m_LatencyView->setText(QString::asprintf("%.3fkb/S", m_ModulationBitrate / 1024.0));
        m_IsModulationDataPending = true;
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::RequestPWMStatus()
{
    if (!m_IsStatusUpdatePending && m_DeviceSocket.state() == QAbstractSocket::ConnectedState)
    {
        m_IsStatusUpdatePending = true;

        WifiPackageHeader msg;
        WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_GetPWMStatus);
        m_DeviceSocket.write(reinterpret_cast<const char*>(&msg), sizeof(msg));
        m_TimeSincePWMStatusRequest.start();
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::EnableControls(bool enable)
{
    m_FrequencySlider->setEnabled(enable);
    m_DutyCycleSlider->setEnabled(enable);
    m_DeadTimeLSSlider->setEnabled(enable);
    m_DeadTimeHSSlider->setEnabled(enable);
    m_TemperatureResolutionSlider->setEnabled(enable);
    m_SampleRateSlider->setEnabled(enable);
    m_VolumeSlider->setEnabled(enable);

    m_AudioPlayButton->setEnabled(enable);
    m_AudioStopButton->setEnabled(enable && m_AudioPlayer.GetState() != AudioPlayer::e_StateStopped);
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::UpdatePWMStatusView()
{
    if (m_IsPWMStatusValid)
    {
        m_PWMFrequency->setText(QString::asprintf("%.3fkHz", double(m_PWMStatus.m_Frequency * 4) / double(m_PWMStatus.m_PWMPeriode) / 1000.0));
        m_PWMDutyCycle->setText(QString::asprintf("%.2f%%", double(m_PWMStatus.m_PWMDutyCycle) * 100.0 / 65536.0));
        m_PWMDeadTimeLS->setText(QString::asprintf("%.2fnS", double(m_PWMStatus.m_PWMDeadTimeLS) * 1e9 / double(m_PWMStatus.m_Frequency)));
        m_PWMDeadTimeHS->setText(QString::asprintf("%.2fnS", double(m_PWMStatus.m_PWMDeadTimeHS) * 1e9 / double(m_PWMStatus.m_Frequency)));
        m_PWMTemperature1->setText(QString::asprintf("<qt>%.2f&deg;C</qt>", double(m_PWMStatus.m_Temperature1) / 16.0));
        m_PWMTemperature2->setText(QString::asprintf("<qt>%.2f&deg;C</qt>", double(m_PWMStatus.m_Temperature2) / 16.0));
//        m_PWMCurrentLow->setText(QString::asprintf("%.2fA", double(m_PWMStatus.m_CurrentLo - 1024) / 1024.0 * 200.0));
//        m_PWMCurrentHigh->setText(QString::asprintf("%.2fA", double(m_PWMStatus.m_CurrentHi - 1024) / 1024.0 * 200.0));

        static double currentLow = 0.0f;
        static double currentHigh = 0.0f;

//        currentLow += 1;
//        currentHigh -= 1;
//        if (m_PWMStatus.m_CurrentLow < currentLow) currentLow = m_PWMStatus.m_CurrentLow;
//        if (m_PWMStatus.m_CurrentHigh > currentHigh) currentHigh = m_PWMStatus.m_CurrentHigh;
        currentLow +=  (double(m_PWMStatus.m_CurrentLow) - currentLow) * 0.5;
        currentHigh +=  (double(m_PWMStatus.m_CurrentHigh) - currentHigh) * 0.5;
//        currentLow = m_PWMStatus.m_CurrentLow;
//        currentHigh = m_PWMStatus.m_CurrentHigh;
        m_PWMCurrentLow->setText(QString::asprintf("%.1fA", currentLow * 200.0 / 2048.0));
        m_PWMCurrentHigh->setText(QString::asprintf("%.1fA", currentHigh * 200.0 / 2048.0));

        if (m_UpdateSliders)
        {
            m_UpdateSliders = false;
            m_FrequencySlider->setValue(m_PWMStatus.m_Frequency * 4 / m_PWMStatus.m_PWMPeriode);
            m_DutyCycleSlider->setValue(m_PWMStatus.m_PWMDutyCycle);
            m_DeadTimeLSSlider->setValue(m_PWMStatus.m_PWMDeadTimeLS);
            m_DeadTimeHSSlider->setValue(m_PWMStatus.m_PWMDeadTimeHS);
            m_CurrentLimitLowSlider->setValue(m_PWMStatus.m_PWMCurrentLimitLow);
            m_CurrentLimitHighSlider->setValue(m_PWMStatus.m_PWMCurrentLimitHigh);
            m_TemperatureResolutionSlider->setValue(m_PWMStatus.m_TemperatureResolution);

            m_SampleRateSlider->setValue(m_PWMStatus.m_ModulationSampleRate);
            double scale = double(m_PWMStatus.m_ModulationMultiplier) / double(1<<m_PWMStatus.m_ModulationShifter);
            int volume = scale * (100.0*255.0) / double(m_PWMStatus.m_PWMPeriode) + 0.5;
            m_VolumeSlider->setValue(volume);

            EnableControls(true);
        }
    }
    else
    {
        m_PWMFrequency->setText("--");
        m_PWMDutyCycle->setText("--");
        m_PWMDeadTimeLS->setText("--");
        m_PWMDeadTimeHS->setText("--");
        m_PWMTemperature1->setText("--");
        m_PWMTemperature2->setText("--");
    }
}

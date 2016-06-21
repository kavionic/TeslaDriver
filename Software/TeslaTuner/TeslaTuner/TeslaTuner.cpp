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

    connect(m_TemperatureResolutionSlider, &QAbstractSlider::valueChanged, this, &TeslaTuner::SlotTemperatureResolutionSliderChanged);

    connect(m_DeviceAddressView, SIGNAL(activated(int)), this, SLOT(SlotDeviceAddressSelected(int)));
//    connect(m_DeviceAddressView, &QComboBox::currentTextChanged, this, &TeslaTuner::SlotDeviceAddressSelected);
    connect(m_DevicePortView, &QLineEdit::editingFinished, this, &TeslaTuner::SlotDevicePortChanged);
    connect(m_ConnectCheckBox, &QCheckBox::stateChanged, this, &TeslaTuner::SlotConnectCheckBoxChanged);

    connect(m_AudioFileBrowseButton, &QToolButton::clicked, this, &TeslaTuner::SlotBrowseAudioFile);
    connect(m_AudioFileView, SIGNAL(activated(int)), this, SLOT(SlotAudioPathSelected(int)));
    connect(m_AudioPlayButton, &QToolButton::toggled, this, &TeslaTuner::SlotPlayButton);
    connect(m_AudioStopButton, &QToolButton::clicked, this, &TeslaTuner::SlotStopButton);
    connect(m_AudioPosition, &QAbstractSlider::valueChanged, this, &TeslaTuner::SlotAudioPositionSliderChanged);
    connect(m_VolumeSlider, &QAbstractSlider::valueChanged, this, &TeslaTuner::SlotVolumeSliderChanged);
    connect(m_SampleRateSlider, &QAbstractSlider::valueChanged, this, &TeslaTuner::SlotSampleRateSliderChanged);

    connect(&m_DeviceSocket, &QAbstractSocket::stateChanged, this, &TeslaTuner::SlotConnectionStateChanged);
    connect(&m_DeviceSocket, &QTcpSocket::readyRead, this, &TeslaTuner::SlotSocketDataReady);

    m_ConnectionStatusView->setText("NO CONNECTION");

    UpdateTemperatureResolutionValue();
    UpdateFrequencySliderValue();
    UpdateDutyCycleSliderValue();
    UpdateDeadTimeLSSliderValue();
    UpdateDeadTimeHSSliderValue();
    UpdateVolumeSliderValue();
    UpdateSampleRateSliderValue();

    EnableControls(false);

    m_DeviceAddressView->setMaxCount(30);

    int addressCount = settings.beginReadArray("Network/DeviceAddressHistory");
    for (int i = 0 ; i < addressCount; ++i)
    {
        settings.setArrayIndex(i);
        m_DeviceAddressView->addItem(settings.value("address").toString());
    }
    settings.endArray();
    m_DevicePortView->setText(QString::asprintf("%d", settings.value("Network/DevicePort").toInt()));
    m_ConnectCheckBox->setChecked(settings.value("Network/Connected").toBool());
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

    settings.beginWriteArray("Network/DeviceAddressHistory", m_DeviceAddressView->count());
    for (int i = 0, e = m_DeviceAddressView->count() ; i < e; ++i)
    {
        settings.setArrayIndex(i);
        settings.setValue("address", m_DeviceAddressView->itemText(i));
    }
    settings.endArray();
    settings.setValue("Network/DevicePort", m_DevicePortView->text().toInt());

    settings.setValue("Network/Connected", m_ConnectCheckBox->isChecked());

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
    m_IsRadioStatusUpdatePending = false;
    m_IsRadioStatusValid = false;
    m_IsStatusUpdatePending = false;
    m_IsModulationDataPending = false;
    m_IsPWMStatusValid = false;

    m_LatencyView->setText("--");
    UpdatePWMStatusView();
    switch (state)
    {
    case QAbstractSocket::UnconnectedState:
        EnableControls(false);
        m_ConnectionStatusView->setText("NO CONNECTION");
        break;
    case QAbstractSocket::HostLookupState:
        m_ConnectionStatusView->setText("LOOKUP");
        break;
    case QAbstractSocket::ConnectingState:
        m_ConnectionStatusView->setText("CONNECTING");
        break;
    case QAbstractSocket::ConnectedState:
        m_UpdateSliders = true;
        m_ConnectionStatusView->setText("CONNECTED");
        SendPing();
        RequestRadioStatus();
        RequestPWMStatus();
//        m_TimeSinceModulationSent.start();
//        SendModulationData();
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
            m_Latency = m_TimeSincePingSent.nsecsElapsed();
//            m_LatencyView->setText(QString::asprintf("%.3fmS", double(m_Latency/1000)/1000.0));
            m_IsPingPending = false;
            break;
        case WifiCmd_e::e_GetRadioStatusReply:
        {
            //m_Latency = m_TimeSinceRadioStatusRequest.nsecsElapsed();
            int strength = m_ResponsePackages.m_GetRadioStatusReply.m_Strength;
            if ( strength < -100 ) {
                strength = 0;
            } else if (strength > -50) {
                strength = 100;
            } else {
                strength = 100 + (50 + strength) * 2;
            }
//            m_SignalStrengthView->setText(QString::asprintf("%d%%", strength));
            m_SignalStrengthView->setValue(strength);
            m_IsRadioStatusUpdatePending = false;
            m_IsRadioStatusValid = true;
            break;
            }
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
        default:
            break;
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotRefreshTimer()
{
/*    if (m_IsStatusUpdatePending && m_TimeSincePWMStatusRequest.elapsed() > 1000) {
        //m_IsRadioStatusUpdatePending = false;
        //m_IsRadioStatusValid = false;
        m_IsStatusUpdatePending = false;
        //m_IsPWMStatusValid = false;
        //m_DeviceSocket.disconnectFromHost();
        RequestPWMStatus();
    }*/
    if (m_DeviceSocket.state() == QAbstractSocket::UnconnectedState)
    {
//        m_DeviceSocket.connectToHost("192.168.1.149", 42);
//        m_DeviceSocket.connectToHost("192.168.1.146", 42);
//        m_DeviceSocket.connectToHost("192.168.1.141", 42);
//        m_DeviceSocket.connectToHost("192.168.1.142", 42);
        ReconnectDevice();
        return;
    }
    if (!m_IsPingPending && m_TimeSincePingSent.elapsed() > 1000) {
        SendPing();
    }
    if (!m_IsRadioStatusUpdatePending && m_TimeSinceRadioStatusRequest.elapsed() > 1000) {
        //RequestRadioStatus();
    }
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

void TeslaTuner::SlotDeviceAddressSelected(int index)
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

void TeslaTuner::SlotDevicePortChanged()
{
    ReconnectDevice();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotConnectCheckBoxChanged()
{
    ReconnectDevice();
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SlotVolumeSliderChanged(int value)
{
    WifiSetPWMModulationScale msg;
    WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_SetPWMModulationScale);

    double scale = double(m_PWMStatus.m_PWMPeriode) * double(value) / (255.0*255.0);

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

void TeslaTuner::ReconnectDevice()
{
    m_DeviceSocket.disconnectFromHost();
    m_DeviceSocket.abort();
    if (m_ConnectCheckBox->checkState())
    {
        m_DeviceSocket.connectToHost(m_DeviceAddressView->currentText(), m_DevicePortView->text().toInt());
        m_DeviceSocket.setSocketOption(QAbstractSocket::LowDelayOption, 1);
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::SendPing()
{
    if (!m_IsPingPending && m_DeviceSocket.state() == QAbstractSocket::ConnectedState)
    {
        m_IsPingPending = true;

        WifiPackageHeader msg;
        WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_Ping);
        m_DeviceSocket.write(reinterpret_cast<const char*>(&msg), sizeof(msg));
        m_TimeSincePingSent.start();
    }
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
        m_LatencyView->setText(QString::asprintf("%.3fkb/S", m_ModulationBitrate / 1024.0));
        m_IsModulationDataPending = true;
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void TeslaTuner::RequestRadioStatus()
{
    if (!m_IsRadioStatusUpdatePending && m_DeviceSocket.state() == QAbstractSocket::ConnectedState)
    {
        m_IsRadioStatusUpdatePending = true;

        WifiPackageHeader msg;
        WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_GetRadioStatus);
        m_DeviceSocket.write(reinterpret_cast<const char*>(&msg), sizeof(msg));
        m_TimeSinceRadioStatusRequest.start();
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

        if (m_UpdateSliders)
        {
            m_UpdateSliders = false;
            m_FrequencySlider->setValue(m_PWMStatus.m_Frequency * 4 / m_PWMStatus.m_PWMPeriode);
            m_DutyCycleSlider->setValue(m_PWMStatus.m_PWMDutyCycle);
            m_DeadTimeLSSlider->setValue(m_PWMStatus.m_PWMDeadTimeLS);
            m_DeadTimeHSSlider->setValue(m_PWMStatus.m_PWMDeadTimeHS);
            m_TemperatureResolutionSlider->setValue(m_PWMStatus.m_TemperatureResolution);

            m_SampleRateSlider->setValue(m_PWMStatus.m_ModulationSampleRate);
            double scale = double(m_PWMStatus.m_ModulationMultiplier) / double(1<<m_PWMStatus.m_ModulationShifter);
            int volume = scale * (255.0*255.0) / double(m_PWMStatus.m_PWMPeriode) + 0.5;
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

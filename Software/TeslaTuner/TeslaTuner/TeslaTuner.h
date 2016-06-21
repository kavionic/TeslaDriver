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


#ifndef TESLATUNER_H
#define TESLATUNER_H

#include <QtWidgets/QMainWindow>
#include <QtNetwork/QTcpSocket.h>

#include "AudioPlayer.h"

#include "ui_TeslaTuner.h"

class TeslaTuner : public QMainWindow, public Ui::TeslaTunerClass
{
    Q_OBJECT

public:
    TeslaTuner(QWidget *parent = 0);
    ~TeslaTuner();

    virtual void closeEvent(QCloseEvent* event) override;

private slots:
    void SlotConnectionStateChanged(QAbstractSocket::SocketState state);
    void SlotAudioPlayerStateChanged(AudioPlayer::State_e state);
    void SlotAudioDurationChanged(double duration);
    void SlotAudioPositionChanged(double position);
    void SlotAudioPositionSliderChanged(int value);

    void SlotSocketDataReady();
    void SlotRefreshTimer();
    
    void SlotFrequencySliderChanged(int value);
    void UpdateFrequencySliderValue();

    void SlotDutyCycleSliderChanged(int value);
    void UpdateDutyCycleSliderValue();
   
    void SlotDeadTimeLSSliderChanged(int value);
    void SlotDeadTimeHSSliderChanged(int value);

    void UpdateDeadTimeLSSliderValue();
    void UpdateDeadTimeHSSliderValue();

    void SlotTemperatureResolutionSliderChanged(int value);
    void UpdateTemperatureResolutionValue();

    void SlotDeviceAddressSelected(int index);
    void SlotDevicePortChanged();
    void SlotConnectCheckBoxChanged();


    void SlotVolumeSliderChanged(int value);
    void UpdateVolumeSliderValue();

    void SlotSampleRateSliderChanged(int value);
    void UpdateSampleRateSliderValue();

    void SlotAudioPathSelected(int index);
    void SlotBrowseAudioFile();
    void SlotPlayButton();
    void SlotStopButton();

private:
    void ReconnectDevice();
    void SendPing();
    void RequestRadioStatus();
    void RequestPWMStatus();
    void SendModulationData();

    void EnableControls(bool enable);
    void UpdatePWMStatusView();
    void ProcessResponse();

    QTcpSocket m_DeviceSocket;
    QTimer     m_RefreshTimer;
    QElapsedTimer m_TimeSincePingSent;
    QElapsedTimer m_TimeSincePWMStatusRequest;
    QElapsedTimer m_TimeSinceRadioStatusRequest;
    QElapsedTimer m_TimeSinceModulationSent;
    uint64_t      m_ModulationSendTime = 0;
    int64_t       m_Latency;
    double        m_ModulationBitrate = 0.0;
    bool                  m_IsPingPending = false;
    bool                  m_IsRadioStatusUpdatePending = false;
    bool                  m_IsRadioStatusValid = false;
    bool                  m_IsStatusUpdatePending = false;
    bool                  m_IsPWMStatusValid = false;
    bool                  m_UpdateSliders = false; // Set to true when connection established and to false on the first update from the device.

    WifiGetPWMStatusReply m_PWMStatus;

    bool m_IsModulationDataPending = false;

    union
    {
        char                    m_Buffer[1];
        WifiPackageHeader       m_Header;
        WifiGetRadioStatusReply m_GetRadioStatusReply;
        WifiGetPWMStatusReply   m_GetPWMStatusReply;
    } m_ResponsePackages;
    int m_ResponseBytesReceived = 0;

    AudioPlayer m_AudioPlayer;
    QString     m_AudioFilePath;
};

#endif // TESLATUNER_H

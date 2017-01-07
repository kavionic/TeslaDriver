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
#include "AudioPlayer.h"

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

AudioPlayer::AudioPlayer(QObject *parent) : QObject(parent)
{
    m_AudioDecoder = new QAudioDecoder(this);

    QAudioFormat desiredFormat;
    desiredFormat.setChannelCount(2);
    desiredFormat.setCodec("audio/x-raw");
    desiredFormat.setSampleType(QAudioFormat::Float);
    desiredFormat.setSampleRate(44000);
    desiredFormat.setSampleSize(16);

    m_AudioDecoder->setAudioFormat(desiredFormat);

    connect(m_AudioDecoder, &QAudioDecoder::bufferReady, this, &AudioPlayer::SlotAudioDataReady);
    connect(m_AudioDecoder, &QAudioDecoder::durationChanged, this, &AudioPlayer::SlotAudioDurationChanged);
    connect(m_AudioDecoder, &QAudioDecoder::finished, this, &AudioPlayer::SlotAudioFinished);

}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

AudioPlayer::~AudioPlayer()
{
    delete m_AudioDecoder;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

bool AudioPlayer::LoadFile(const QString& path)
{
    if (m_AudioDecoder->state() != QAudioDecoder::StoppedState) m_AudioDecoder->stop();

    if (m_State != e_StateStopped) {
        Stop();
    }

    m_AudioDecoder->setSourceFilename(path);

    m_AudioDecoder->start();
    m_SampleBuffer.clear();
    m_IsPlaying = false;
    m_IsDecodingFinished = false;

    if (m_CurrentPos != 0.0)
    {
        m_CurrentPos = 0.0;
        emit SignalPlayPositionChanged(m_CurrentPos);
    }
    SetState(e_StatePaused);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void AudioPlayer::SetSampleRate(int sampleRate)
{
    m_SampleRate = sampleRate;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void AudioPlayer::Play(bool play)
{
    if (play != m_IsPlaying)
    {
        m_IsPlaying = play;
        if (m_IsDecodingFinished && m_SampleBuffer.empty()) {
            SetState(e_StateStopped);
        } else {
            SetState((play) ? e_StatePlaying : e_StatePaused);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void AudioPlayer::Stop()
{
    m_IsPlaying = false;
    if (m_CurrentPos != 0.0)
    {
        m_CurrentPos = 0.0;
        emit SignalPlayPositionChanged(m_CurrentPos);
    }
    SetState(e_StateStopped);
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void AudioPlayer::SetPlayPosition(double position)
{
    if (position != m_CurrentPos)
    {
        m_CurrentPos = position;
        emit SignalPlayPositionChanged(m_CurrentPos);
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

int AudioPlayer::RequestData(QTcpSocket* socket, int length)
{
    if (!m_IsPlaying) {
        return 0;
    }
    uint8_t modulationData[2048];

#if 1
    double timePerSample = (double(m_SourceSampleRate) / double(m_SampleRate)) / double(m_SampleBuffer.size());
    for (int i = 0 ; i < length ; ++i)
    {
        int j = int(m_CurrentPos * double(m_SampleBuffer.size()) + 0.5);
        if (j >= m_SampleBuffer.size()) {
            length = i;
            break;
        }
        modulationData[i] = m_SampleBuffer[j];
        m_CurrentPos += timePerSample;
    }
#else
    static uint32_t phase = 0;
    for (int i = 0 ; i < length ; ++i)
    {
#if 1
        uint32_t cyclePhase = phase % 510;
        if (cyclePhase < 256) {
            modulationData[i] = cyclePhase % 256;
        } else {
            modulationData[i] = 254 - cyclePhase % 256;
        }
        phase += 10;
#else
        uint32_t cyclePhase = phase % 100;
//        modulationData[i] = (cyclePhase < 90) ? (cyclePhase * 255 / 89) : 0;
        modulationData[i] = (cyclePhase > 98) ? 255 : 0;
        phase++;
#endif
    }
#endif

    if (length > 0)
    {
        WifiSetVal16 msg;
        WifiPackageHeader::InitMsg(msg, WifiCmd_e::e_WritePWMModulationData);
        msg.m_Value = length;

        socket->write(reinterpret_cast<const char*>(&msg), sizeof(msg));
        socket->write((char*)modulationData, length);
        emit SignalPlayPositionChanged(m_CurrentPos);
    }
    if (length == 0 && m_IsDecodingFinished)
    {
        SetState(e_StateStopped);
    }
    return length;
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void AudioPlayer::SlotAudioDurationChanged(qint64 duration)
{
    m_SourceSampleRate = m_AudioDecoder->audioFormat().sampleRate();
    m_SampleBuffer.reserve(duration * m_SourceSampleRate / 1000 + 1000);
    m_Duration = double(duration) / 1000.0f;
    emit SignalDurationChanged(m_Duration);
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void AudioPlayer::SlotAudioDataReady()
{
    QAudioFormat::SampleType sampleType = m_AudioDecoder->audioFormat().sampleType();
    while (m_AudioDecoder->bufferAvailable())
    {
        QAudioBuffer buffer = m_AudioDecoder->read();

        const int16_t* data = buffer.constData<int16_t>();

        for (int i = 0, e = buffer.sampleCount() / 2; i < e ; ++i)
        {
            uint8_t sample = (int32_t(data[i*2]) + int32_t(data[i*2+1])) / 512 + 127;
            m_SampleBuffer.push_back(sample);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void AudioPlayer::SlotAudioFinished()
{
    m_IsDecodingFinished = true;
    if (m_SampleBuffer.empty())
    {
        SetState(e_StateStopped);
    }
}

///////////////////////////////////////////////////////////////////////////////
///
///////////////////////////////////////////////////////////////////////////////

void AudioPlayer::SetState(State_e state)
{
    if (state != m_State)
    {
        m_State = state;
        emit SignalStateChanged(state);
    }
}

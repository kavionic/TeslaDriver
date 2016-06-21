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


#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

class AudioPlayer : public QObject
{
    Q_OBJECT
public:
    enum State_e { e_StateStopped, e_StatePlaying, e_StatePaused };

    AudioPlayer(QObject* parent);
    ~AudioPlayer();

    bool LoadFile(const QString& path);
    void SetSampleRate(int sampleRate);
    void Play(bool play);
    void Stop();
    bool HasDataToStream() const { return !m_SampleBuffer.empty(); }

    State_e GetState() const { return m_State; }

    double GetDuration() const { return m_Duration; }
    double GetPlayPosition() const { return m_CurrentPos; }
    void   SetPlayPosition(double position);
    int RequestData(QTcpSocket* socket, int length);

signals:
    void SignalStateChanged(State_e state);
    void SignalDurationChanged(double duration);
    void SignalPlayPositionChanged(double position);

private slots:
    void SlotAudioDurationChanged(qint64 duration);

    void SlotAudioDataReady();
    void SlotAudioFinished();
private:
    void SetState(State_e state);

    QAudioDecoder* m_AudioDecoder;
    //QAudioBuffer   m_CurrentAudioBuffer;
    //int            m_AudioBufferBytesSent = 0;
    State_e        m_State = e_StateStopped;
    double         m_Duration = 0.0;
    bool           m_IsDecodingFinished = false;
    int            m_SourceSampleRate = 44100;
    int            m_SampleRate = 22050;
    bool           m_IsPlaying = false;
    double         m_CurrentPos = 0.0;
    std::vector<uint8_t> m_SampleBuffer;
};

#endif // AUDIOPLAYER_H

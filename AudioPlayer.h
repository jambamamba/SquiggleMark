#pragma once

#include <functional>
#include <QObject>
#include <QString>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

struct PcmPlayer;
class AudioPlayer : public QObject
{
    Q_OBJECT
public:
    AudioPlayer();
    ~AudioPlayer();

    void openAudio(int channels, int sample_rate, enum AVSampleFormat sample_fmt);
    void closeAudio();
    void addAudioFrame(const AVFrame *frame);
    void setVolume(int percent);

private:

private:
    PcmPlayer *m_pcm;
    enum AVSampleFormat m_sample_fmt;
    void* m_data;
};

extern "C" {
#include "ffplay3.h"
}

#include <QFile>
#include <QString>
#include <QThread>

#include "AudioPlayer.h"
#include "PcmPlayer.h"

extern "C" void mylog(const char *fmt, ...);

namespace
{
int numChannels(const AVFrame* frame)
{
    return frame->channels > 2 ? 2 : frame->channels;
}

void *interleaveChannels(const AVFrame* frame,
                         enum AVSampleFormat sample_fmt,
                         void *data,
                         int bytes_per_channel)
{
    switch(sample_fmt)
    {
    case AV_SAMPLE_FMT_U8:          ///< unsigned 8 bits
    case AV_SAMPLE_FMT_S16:         ///< signed 16 bits
    case AV_SAMPLE_FMT_S32:         ///< signed 32 bits
    case AV_SAMPLE_FMT_FLT:         ///< float
    case AV_SAMPLE_FMT_DBL:         ///< double
        mylog("Unsupported audio format %i", sample_fmt);
        return nullptr;
    }

    int channels = numChannels(frame);
    data = realloc(data, channels * bytes_per_channel);

    for(int i = 0; i < frame->nb_samples; ++i)
    {
        for(int channel = 0; channel < channels; ++channel)
        {
            switch(sample_fmt)
            {
            case AV_SAMPLE_FMT_U8P:         ///< unsigned 8 bits, planar
                ((unsigned char*)data)[2 * i + channel] =
                        ((unsigned char*)frame->extended_data[channel])[i];
                break;
            case AV_SAMPLE_FMT_S16P:        ///< signed 16 bits, planar
                ((short*)data)[2 * i + channel] =
                        ((short*)frame->extended_data[channel])[i];
                break;
            case AV_SAMPLE_FMT_S32P:        ///< signed 32 bits, planar
                ((int*)data)[2 * i + channel] =
                        ((int*)frame->extended_data[channel])[i];
                break;
            case AV_SAMPLE_FMT_FLTP:        ///< float, planar
                ((float*)data)[2 * i + channel] =
                        ((float*)frame->extended_data[channel])[i];
                break;
            case AV_SAMPLE_FMT_DBLP:        ///< double, planar
                ((double*)data)[2 * i + channel] =
                        ((double*)frame->extended_data[channel])[i];
                break;
            }

        }
    }
    return data;
}

void flpToInt(float* data, int data_sz)
{
    int* j = (int*) data;
    for(int i = 0; i < data_sz/4; i++)
    {
        j[i] = (int)(data[i] * __INT_MAX__);
    }
}
}//namespace

AudioPlayer::AudioPlayer()
    : m_pcm(nullptr)
    , m_data(nullptr)
    , m_sample_fmt(AV_SAMPLE_FMT_NONE)
{
    m_data = (float*)malloc(1024);
}

AudioPlayer::~AudioPlayer()
{
    closeAudio();
    free(m_data);
}

void AudioPlayer::openAudio(int channels, int sample_rate, enum AVSampleFormat sample_fmt)
{
    closeAudio();

    m_pcm = PcmPlayer::create();
    if(!m_pcm->open(channels, sample_rate,
               av_get_bytes_per_sample(sample_fmt) * 8))
    {
        closeAudio();
    }
    m_sample_fmt = sample_fmt;
}

void AudioPlayer::closeAudio()
{
    if(m_pcm)
    {
        m_pcm->close();
        delete m_pcm;
        m_pcm = nullptr;
    }
}

void AudioPlayer::addAudioFrame(const AVFrame *frame)
{
    if(!m_pcm)
    {
        return;
    }
    int channels = numChannels(frame);
    int bytes_per_channel = frame->nb_samples *
            av_get_bytes_per_sample(m_sample_fmt);
    m_data = interleaveChannels(frame, m_sample_fmt,
                                m_data, bytes_per_channel);
    if(m_data)
    {
        if(m_sample_fmt == AV_SAMPLE_FMT_FLTP)
        {
            flpToInt((float*)m_data, channels * bytes_per_channel);
        }
        m_pcm->play((const unsigned char*)m_data, channels * bytes_per_channel);
    }
}

void AudioPlayer::setVolume(int percent)
{
    if(!m_pcm)
    {
        return;
    }
    m_pcm->setVolume(percent);
}

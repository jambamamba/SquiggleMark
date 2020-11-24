#pragma once

class PcmPlayer
{
public:
    PcmPlayer() {}
    virtual ~PcmPlayer() {}

    virtual bool open(int num_channels, int sample_rate, int bits_per_sample) = 0;
    virtual int play(const unsigned char* pcm_data, int pcm_data_sz) = 0;
    virtual void close() = 0;
    virtual void setVolume(int percent) = 0;
    static PcmPlayer* create();

private:
    PcmPlayer(const PcmPlayer&) = delete;
    const PcmPlayer& operator=(const PcmPlayer&) = delete;
};



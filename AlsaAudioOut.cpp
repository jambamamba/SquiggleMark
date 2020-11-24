#include "PcmPlayer.h"

class AlsaAudioOut : public PcmPlayer
{
public:
    AlsaAudioOut() {}
    virtual ~AlsaAudioOut() {}

    virtual bool open(int num_channels, int sample_rate, int bits_per_sample) override { return false; }
    virtual int play(const unsigned char* pcm_data, int pcm_data_sz) override { return false; }
    virtual void close() override {}
    virtual void setVolume(int percent) override {}

private:
};

PcmPlayer* PcmPlayer::create()
{
    return new AlsaAudioOut;
}

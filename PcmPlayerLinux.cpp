#include <stdio.h>


#include "PcmPlayer.h"

extern "C" void mylog(const char *fmt, ...);

class PcmPlayerLinux : public PcmPlayer
{
public:
    PcmPlayerLinux();
    ~PcmPlayerLinux();

    virtual bool open(int num_channels, int sample_rate, int bits_per_sample) override;
    virtual int play(const unsigned char* pcm_data, int pcm_data_sz) override;
    virtual void close() override;
    virtual void setVolume(int percent) override;

private:
};

PcmPlayer* PcmPlayer::create()
{
    return new PcmPlayerLinux;
}

PcmPlayerLinux::PcmPlayerLinux()
    : PcmPlayer()
{
}

PcmPlayerLinux::~PcmPlayerLinux()
{
}
bool PcmPlayerLinux::open(int num_channels, int sample_rate, int bits_per_sample)
{
    return false;
}

void PcmPlayerLinux::close()
{
}

int PcmPlayerLinux::play(const unsigned char* pcm_data, int pcm_data_sz)
{
    return 0;
}

void PcmPlayerLinux::setVolume(int percent)
{
}

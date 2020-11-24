#include <stdio.h>
#include <QDebug>
#include <QMutex>
#include <QSemaphore>

extern "C" {
#include </Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk/System/Library/Frameworks/AudioToolbox.framework/Versions/A/Headers/AudioQueue.h>
#include </Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk/System/Library/Frameworks/AudioToolbox.framework/Versions/A/Headers/AudioFormat.h>
#include </Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk/System/Library/Frameworks/AudioToolbox.framework/Versions/A/Headers/AudioFile.h>
#include </Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk/System/Library/Frameworks/AudioToolbox.framework/Versions/A/Headers/AudioQueue.h>
}

#include "PcmPlayer.h"

extern "C" void mylog(const char *fmt, ...);

class PcmPlayerOsx : public PcmPlayer
{
public:
    PcmPlayerOsx();
    ~PcmPlayerOsx();

    virtual bool open(int num_channels, int sample_rate, int bits_per_sample) override;
    virtual int play(const unsigned char* pcm_data, int pcm_data_sz) override;
    virtual void close() override;
    virtual void setVolume(int percent) override;

private:
    static const int NUM_BUFFERS = 100;
    static const int BUFFER_SIZE = 1024 *100;
    int m_head;
    int m_tail;
    QMutex m_av_mutex;
    QSemaphore m_free_sem;
    QSemaphore m_used_sem;
    int m_num_queued_buffers;
    AudioQueueBuffer *m_buffer[NUM_BUFFERS];
    AudioQueueRef m_queue;

    void onWriteComplete();
    static void AudioQueueOutputCallback(void *ptr, AudioQueueRef queue, AudioQueueBuffer* buffer);
};

PcmPlayer* PcmPlayer::create()
{
    return new PcmPlayerOsx;
}

PcmPlayerOsx::PcmPlayerOsx()
    : PcmPlayer()
    , m_head(0)
    , m_tail(0)
    , m_free_sem(NUM_BUFFERS)
    , m_used_sem()
    , m_num_queued_buffers(0)
{
    memset(m_buffer, 0, sizeof(m_buffer));
}

PcmPlayerOsx::~PcmPlayerOsx()
{
    close();
}

void PcmPlayerOsx::AudioQueueOutputCallback(void *ptr, AudioQueueRef queue, AudioQueueBuffer* buffer)
{
    PcmPlayerOsx* pthis = (PcmPlayerOsx*)ptr;
    pthis->onWriteComplete();
}

void PcmPlayerOsx::onWriteComplete()
{
    m_used_sem.acquire();
    m_av_mutex.lock();
    m_tail = (m_tail + 1) % NUM_BUFFERS;
    m_num_queued_buffers--;
    m_av_mutex.unlock();
    m_free_sem.release();

}

bool PcmPlayerOsx::open(int num_channels, int sample_rate, int bits_per_sample)
{
    num_channels = num_channels > 2 ? 2 : num_channels;

    AudioStreamBasicDescription fmt = {0};

    fmt.mSampleRate = sample_rate;
    fmt.mFormatID = kAudioFormatLinearPCM;
    fmt.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    fmt.mFramesPerPacket = 1;
    fmt.mBitsPerChannel = bits_per_sample;
    fmt.mBytesPerPacket = fmt.mBytesPerFrame = fmt.mBitsPerChannel/8 * num_channels;
    fmt.mChannelsPerFrame = num_channels;

    OSStatus status = AudioQueueNewOutput(&fmt, AudioQueueOutputCallback, this,
                      nullptr,
                      nullptr,
                      0, &m_queue);

    for(int i = 0; i < NUM_BUFFERS; i++)
    {
        status = AudioQueueAllocateBuffer(m_queue, BUFFER_SIZE, &m_buffer[i]);
    }

    return true;
}

void PcmPlayerOsx::close()
{
    while(m_num_queued_buffers>0) usleep(10 * 1000);
    m_av_mutex.lock();
    for(int i = 0; i < NUM_BUFFERS; i++)
    {
        AudioQueueFreeBuffer(m_queue, m_buffer[i]);
    }
    AudioQueueDispose(m_queue, false);
    m_head = m_tail = 0;
    m_num_queued_buffers = 0;
    m_av_mutex.unlock();
}

int PcmPlayerOsx::play(const unsigned char* pcm_data, int pcm_data_sz)
{
    OSStatus status = AudioQueueStart (m_queue, NULL);

    bool started = false;
    int bytes_written = 0;
    for(int pos = 0; pcm_data_sz > 0; )
    {
        while(!m_free_sem.tryAcquire())
        {
//            mylog("Audio queue is full, waiting");
            usleep(10 * 1000);
        }
        m_av_mutex.lock();
        AudioQueueBuffer* buf = m_buffer[m_head];
        m_head = (m_head + 1) % NUM_BUFFERS;
        m_num_queued_buffers++;
        m_av_mutex.unlock();
        m_used_sem.release();

        int nbytes = pcm_data_sz > BUFFER_SIZE ? BUFFER_SIZE : pcm_data_sz;
        memcpy(buf->mAudioData, &pcm_data[pos], nbytes);
        buf->mAudioDataByteSize = nbytes;
        pos += nbytes;
        pcm_data_sz -= nbytes;

        status = AudioQueueEnqueueBuffer (m_queue, buf, 0, NULL);

        if(!started)
        {
            started = true;
            AudioQueueStart(m_queue, 0);
        }
        bytes_written += nbytes;
    }
    return bytes_written;
}

void PcmPlayerOsx::setVolume(int percent)
{
    OSStatus status = AudioQueueSetParameter(m_queue, kAudioQueueParam_Volume, percent/100.0f);

}

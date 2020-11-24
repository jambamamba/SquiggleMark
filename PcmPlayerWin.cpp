#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <QDebug>
#include <QMutex>
#include <QSemaphore>

#include "PcmPlayer.h"

extern "C" void mylog(const char *fmt, ...);

class PcmPlayerWin : public PcmPlayer
{
public:
    PcmPlayerWin();
    ~PcmPlayerWin();

    virtual bool open(int num_channels, int sample_rate, int bits_per_sample) override;
    virtual int play(const unsigned char* pcm_data, int pcm_data_sz) override;
    virtual void close() override;
    virtual void setVolume(int percent) override;

private:
    static const int NUM_BUFFERS = 100;
    static const int BUFFER_SIZE = 1024*100;
    int m_head;
    int m_tail;
    QMutex m_av_mutex;
    QSemaphore m_free_sem;
    QSemaphore m_used_sem;
    int m_num_queued_buffers;
    mutable char m_error[1024];
    WAVEHDR m_wh[BUFFER_SIZE];
    HWAVEOUT m_hwo;

    void onWriteComplete();
    static void waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD* dwInstance, DWORD* dwParam1, DWORD* dwParam2);
};

PcmPlayer* PcmPlayer::create()
{
    return new PcmPlayerWin;
}

PcmPlayerWin::PcmPlayerWin()
    : PcmPlayer()
    , m_hwo(nullptr)
    , m_head(0)
    , m_tail(0)
    , m_free_sem(NUM_BUFFERS)
    , m_used_sem()
    , m_num_queued_buffers(0)
{
    memset(m_wh, 0, sizeof(WAVEHDR) * NUM_BUFFERS);
}

PcmPlayerWin::~PcmPlayerWin()
{
    close();
}

void PcmPlayerWin::waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD* dwInstance, DWORD* dwParam1, DWORD* dwParam2)
{
    switch(uMsg)
    {
    case WOM_OPEN:
        break;
    case WOM_CLOSE:
        break;
    case WOM_DONE:
        {
            PcmPlayerWin* pthis = (PcmPlayerWin*)dwInstance;
            pthis->onWriteComplete();
            break;
        }
    }
}

void PcmPlayerWin::onWriteComplete()
{
    m_used_sem.acquire();
    m_av_mutex.lock();
    m_tail = (m_tail + 1) % NUM_BUFFERS;
    m_num_queued_buffers--;
    m_av_mutex.unlock();
    m_free_sem.release();
}

bool PcmPlayerWin::open(int num_channels, int sample_rate, int bits_per_sample)
{
    UINT num_audio_dev = waveOutGetNumDevs();
    if(num_audio_dev==0)
    {
        return false;
    }

    WAVEFORMATEX wfx;
    memset(&wfx, 0, sizeof(WAVEFORMATEX));
    wfx.cbSize = sizeof(WAVEFORMATEX);
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = num_channels > 2 ? 2 : num_channels;
    wfx.nSamplesPerSec = sample_rate;
    wfx.wBitsPerSample = bits_per_sample;
    wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

    mylog("num_channels %i, sample_rate %i, bits_per_sample %i",
          num_channels, sample_rate, bits_per_sample);

    UINT_PTR uDeviceID;
    for(uDeviceID=0; uDeviceID<num_audio_dev; uDeviceID++)
    {
        MMRESULT res = waveOutOpen(&m_hwo, uDeviceID, &wfx, (DWORD_PTR)waveOutProc, (DWORD_PTR)this,
            WAVE_MAPPED|CALLBACK_FUNCTION );
        if(res!=MMSYSERR_NOERROR)
        {
            waveOutGetErrorTextA(res, m_error, sizeof(m_error));
            continue;
        }
        break;
    }

    if(!m_hwo || uDeviceID == num_audio_dev)
    {
        close();
        return false;
    }

    for(int i = 0; i < NUM_BUFFERS; ++i)
    {
        memset(&m_wh[i], 0, sizeof(WAVEHDR));
        m_wh[i].dwBufferLength = BUFFER_SIZE;
        m_wh[i].lpData = (LPSTR) malloc(m_wh[i].dwBufferLength);
        m_wh[i].dwFlags = 0;

        MMRESULT res = ::waveOutPrepareHeader(m_hwo, &m_wh[i], sizeof(WAVEHDR));
        if(res!=MMSYSERR_NOERROR)
        {
            waveOutGetErrorTextA(res, m_error, sizeof(m_error));
            close();
            return false;//
        }
    }

    return true;
}

void PcmPlayerWin::close()
{
    while(m_num_queued_buffers>0) Sleep(10);
    m_av_mutex.lock();
    if(m_hwo)
    {
        waveOutReset(m_hwo);
    }
    for(int i = 0; i < NUM_BUFFERS; ++i)
    {
        if(m_hwo)
        {
            /*MMRESULT res = */waveOutUnprepareHeader(m_hwo, &m_wh[i], sizeof(WAVEHDR));
        }
        if(m_wh[i].lpData)
        {
            free(m_wh[i].lpData);
            m_wh[i].lpData = nullptr;
        }
    }
    if(m_hwo)
    {
        waveOutClose(m_hwo);
        m_hwo = nullptr;
    }
    m_head = m_tail = 0;
    m_num_queued_buffers = 0;
    m_av_mutex.unlock();
}

int PcmPlayerWin::play(const unsigned char* pcm_data, int pcm_data_sz)
{
//    FILE *fp = fopen("a.pcm", "a+b");
//    fwrite(pcm_data, 1, pcm_data_sz, fp);
//    fclose(fp);

    int bytes_written = 0;
    for(int pos = 0; pcm_data_sz > 0; )
    {
        while(!m_free_sem.tryAcquire())
        {
//            mylog("Audio queue is full, waiting");
            Sleep(100);
        }
        m_av_mutex.lock();
        WAVEHDR &wh = m_wh[m_head];
        m_head = (m_head + 1) % NUM_BUFFERS;
        m_num_queued_buffers ++;
        m_av_mutex.unlock();
        m_used_sem.release();

        int nbytes = pcm_data_sz > BUFFER_SIZE ? BUFFER_SIZE : pcm_data_sz;
        memcpy(wh.lpData, &pcm_data[pos], nbytes);
        wh.dwBufferLength = nbytes;
        pos += nbytes;
        pcm_data_sz -= nbytes;

        MMRESULT res = waveOutWrite(m_hwo, &wh, sizeof(WAVEHDR));
        if(res != MMSYSERR_NOERROR)
        {
            waveOutGetErrorTextA(res, m_error, sizeof(m_error));
            qDebug() << "Error " << m_error;
            break;
        }
        bytes_written += nbytes;
    }
    return bytes_written;
}

void PcmPlayerWin::setVolume(int percent)
{
    unsigned long volume = percent * 0xFFFF / 100;
    volume |= (volume << 16);
    MMRESULT res = waveOutSetVolume(m_hwo, volume);
    if(res != MMSYSERR_NOERROR)
    {
        waveOutGetErrorText(res, m_error, sizeof(m_error));
    }
}

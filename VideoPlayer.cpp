//https://ffmpeg.org/doxygen/trunk/decoding__encoding_8c-source.html
//https://ffmpeg.zeranoe.com/builds/
//http://ksloan.net/watermarking-videos-from-the-command-line-using-ffmpeg-filters/
//http://dranger.com/ffmpeg/tutorial01.html

//https://raw.githubusercontent.com/FFmpeg/FFmpeg/master/doc/examples/decoding_encoding.c
//http://stackoverflow.com/questions/18857737/decoding-h264-frames-from-rtp-stream


/*
c:\work\ffmpeg-20160412-git-196cfc2-win32-shared\bin>ffmpeg.exe -i "C:\work\movi
e\squigglemark2.mp4" -i "C:\work\movie\osletek-logo.png"  -filter_complex "overl
ay=10:10" "c:\work\movie\squigglemark3.mp4"

*/
extern "C" {
#include "ffplay3.h"
}

#include <QFile>
#include <QtGui/QImage>
#include <QString>
#include <QThread>

#include "VideoPlayer.h"

extern "C" void mylog(const char *fmt, ...);

struct MOVIE_PLAYER_CB {
    bool **stop;
    std::function<void(AVFrame *frame, enum AVMediaType media_type)>recvdFrame;
};

struct DecoderOneTimeInitDeinit
{
    DecoderOneTimeInitDeinit() {
        decoder_register();
    }
    ~DecoderOneTimeInitDeinit() {
        uninit_opts();
    }
};
static const DecoderOneTimeInitDeinit decoderOneTimeInitDeinit;

RawFrame::RawFrame()
    : m_width(0)
    , m_height(0)
    , m_bytes_per_pixel(0)
    , m_frame_n(0)
    , m_pos(0)
    , m_bits(nullptr)
{
}

RawFrame::RawFrame(int width, int height, int bytes_per_pixel)
    : m_width(width)
    , m_height(height)
    , m_bytes_per_pixel(bytes_per_pixel)
    , m_frame_n(0)
    , m_pos(0)
    , m_bits(nullptr)
{
    m_bits = (uint8_t *)malloc(width * height * bytes_per_pixel);
}

bool RawFrame::fromAVFrame(const AVFrame *pFrame, int width, int height,
                          int bytes_per_pixel, enum AVPixelFormat pix_fmt,
                           int64_t frame_n, int64_t pos_ms)
{
    m_width = width;
    m_height = height;
    m_frame_n = frame_n;
    m_pos = pos_ms;
    m_bytes_per_pixel = bytes_per_pixel;

    m_bits = (uint8_t *)realloc(m_bits, width * height * bytes_per_pixel);

    SwsContext *sws_ctx = sws_getContext(
                pFrame->width,
                pFrame->height, pix_fmt,
                pFrame->width, pFrame->height,
                AV_PIX_FMT_RGB32, SWS_BICUBIC,
                NULL, NULL, NULL );

    uint8_t *dstSlice[] = { m_bits };
    int dstStride = m_width * m_bytes_per_pixel;
    int ret = sws_scale( sws_ctx, pFrame->data, pFrame->linesize,
                0, pFrame->height, dstSlice, &dstStride );
    sws_freeContext(sws_ctx);

    return (ret > 0 );
}

RawFrame::~RawFrame()
{
    if(m_bits)
    {
        free(m_bits);
    }
}

bool RawFrame::serialize(QString &file)
{
    bool ret = false;
    FILE *fp = fopen(file.toUtf8().data(), "wb");
    if(fp)
    {
        fwrite(&m_width, sizeof(m_width), 1, fp);
        fwrite(&m_height, sizeof(m_height), 1, fp);
        fwrite(&m_bytes_per_pixel, sizeof(m_bytes_per_pixel), 1, fp);
        fwrite(&m_frame_n, sizeof(m_frame_n), 1, fp);
        fwrite(&m_pos, sizeof(m_pos), 1, fp);

        int nbytes = m_width * m_height * m_bytes_per_pixel;
        if(1 == fwrite(m_bits, nbytes, 1, fp))
        {
            ret = true;
        }
        fclose(fp);
    }
    return ret;
}

bool RawFrame::deserialize(QString &file)
{
    bool ret = false;
    FILE *fp = fopen(file.toUtf8().data(), "rb");
    if(fp)
    {
        fread(&m_width, sizeof(m_width), 1, fp);
        fread(&m_height, sizeof(m_height), 1, fp);
        fread(&m_bytes_per_pixel, sizeof(m_bytes_per_pixel), 1, fp);
        fread(&m_frame_n, sizeof(m_frame_n), 1, fp);
        fread(&m_pos, sizeof(m_pos), 1, fp);

        if(m_width > 0 && m_height > 0 && m_bytes_per_pixel > 0)
        {
            int nbytes = m_width * m_height * m_bytes_per_pixel;
            m_bits = (uint8_t *)realloc(m_bits, nbytes);
            if(nbytes == fread(m_bits, 1, nbytes, fp))
            {
                ret = true;
            }
            else
            {
                free(m_bits);
                m_bits = nullptr;
            }
        }
        fclose(fp);
    }
    return ret;
}

VideoPlayer::VideoPlayer()
    : d(nullptr)
{
}

VideoPlayer::~VideoPlayer()
{
    close();
}

void VideoPlayer::close()
{
    if(d)
    {
        decoder_close(d);
        delete d;
        d = nullptr;
    }
}


bool VideoPlayer::open(const char* filename)
{
    close();
    d = new Decoder();
    bool ret = (decoder_open(d, filename) == 0);
    return ret;
}

const Decoder *VideoPlayer::decoder() const
{
    return d;
}
static int gotframe(void* cb_data, enum AVMediaType media_type, AVFrame* frame, int64_t dts, int pkt_size)
{
    MOVIE_PLAYER_CB *cb = (MOVIE_PLAYER_CB*)cb_data;
    cb->recvdFrame(frame, media_type);
    return (**cb->stop) ? 0 : 1;
}

int VideoPlayer::play(bool *stop,
                      std::function<void(AVFrame *, enum AVMediaType)> recvdFrame,
                      int seek_ms)
{
    MOVIE_PLAYER_CB cb;
    cb.stop = &stop;
    cb.recvdFrame = recvdFrame;
    return decoder_play(d, seek_ms, gotframe, &cb) == 0;
}

int VideoPlayer::getFrameRate() const
{
    return d->info.frame_rate;
}

int VideoPlayer::getFrameIntervalMSec() const
{
    return (d && d->info.frame_rate > 0) ? (1000 / d->info.frame_rate) : 0;
}

int VideoPlayer::getNumFrames() const
{
    return d ? d->info.num_frames : 0;
}

int VideoPlayer::getDurationInMilliseconds() const
{
    return d->ic->duration * 1000 / AV_TIME_BASE;
}

int VideoPlayer::getWidthHeight(uint32_t &width, uint32_t &height) const
{
    int ret = -1;
    {
        width = d->info.width;
        height = d->info.height;
        ret = 0;
    }
    return ret;
}

int VideoPlayer::avframeToQImage(const AVFrame *pFrame, QImage *pImage) const
{
    int stream_index = d->st_index[AVMEDIA_TYPE_VIDEO];
    SwsContext *sws_ctx = sws_getContext (
                pFrame->width,
                pFrame->height,
                d->avctx[stream_index]->pix_fmt,
                pFrame->width,
                pFrame->height,
                AV_PIX_FMT_RGB32,
                SWS_BICUBIC,
                NULL, NULL, NULL );

    uint8_t *dstSlice[] = { pImage->bits() };
    int dstStride = pImage->width() * 4;
    int height = sws_scale( sws_ctx, pFrame->data, pFrame->linesize,
                0, pFrame->height, dstSlice, &dstStride );
    sws_freeContext(sws_ctx);

    return height;

//    char outfile[32];
//    sprintf(outfile, "out%02i.bmp", frameNum);
//    qimg.save(outfile);
}

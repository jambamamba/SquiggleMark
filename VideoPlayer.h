#pragma once

#include <functional>
#include <QObject>
#include <QString>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/pixfmt.h>
#include <libavutil/imgutils.h>
}

class QImage;

struct RawFrame
{
    int m_width;
    int m_height;
    int m_bytes_per_pixel;
    int64_t m_frame_n;
    int64_t m_pos;
    unsigned char * m_bits;

    RawFrame();
    RawFrame(int width, int height, int bytes_per_pixel);
    ~RawFrame();
    bool fromAVFrame(const AVFrame *pFrame,
                     int width, int height,
                     int bytes_per_pixel,
                     enum AVPixelFormat pix_fmt,
                     int64_t frame_n, int64_t pos_ms);
    bool serialize(QString &file);
    bool deserialize(QString &file);
};

struct Decoder;
class VideoPlayer : public QObject
{
    Q_OBJECT
public:
    VideoPlayer();
    ~VideoPlayer();

    bool open(const char* filename);
    int play(bool *stop = nullptr,
             std::function<void(AVFrame *, enum AVMediaType)> recvdFrame = nullptr,
             int seekMilliseconds = 0 );
    void close();
    int getNumFrames() const;
    int getDurationInMilliseconds() const;
    int getFrameIntervalMSec() const;
    int getFrameRate() const;
    int getWidthHeight(uint32_t &width, uint32_t &height) const;
    int avframeToQImage(const AVFrame *pFrame, QImage *pImage) const;
    const Decoder *decoder() const;

private:
    Decoder *d;
};

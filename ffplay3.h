#ifndef MAIN3_H
#define MAIN3_H

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavutil/mathematics.h"

#include "libavutil/opt.h"
#include "cmdutils.h"

typedef int (*GOT_FRAME_CB)(void* user, enum AVMediaType media_type, AVFrame *frame, int64_t dts, int pkt_size);

typedef struct Info {
    int width;
    int height;
    int64_t duration;
    int64_t duration_ms;
    int64_t num_frames;
    int frame_rate;
}Info;

typedef struct Decoder {
    int packet_pending;
    int64_t start_pts, next_pts;
    AVRational start_pts_tb, next_pts_tb;
    AVPacket pkt_temp, flush_pkt;
    AVFormatContext *ic;
    AVCodecContext *avctx[AVMEDIA_TYPE_NB];
    int st_index[AVMEDIA_TYPE_NB];
    AVStream *stream[AVMEDIA_TYPE_NB];
    AVDictionary *format_opts1;
    AVDictionary *codec_opts1;
    struct Info info;
    GOT_FRAME_CB gotFrameCb;
    void* user_data;

}Decoder;

void decoder_register();
int decoder_open(Decoder *d, const char* filename);
int decoder_play(Decoder *d, int seek_ms, GOT_FRAME_CB gotFrameCb, void *user_data);
void decoder_close(Decoder *d);

#endif // MAIN3_H


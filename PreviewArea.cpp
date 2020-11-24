#include <QDebug>
#include <QApplication>
#include <QByteArray>
#include <QtConcurrent/QtConcurrent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QImageReader>
#include <QTransform>
#include <QUrl>
#include <QPainter>
#include <QtSvg/QSvgRenderer>

extern "C" {
#include "ffplay3.h"
}
#include "PreviewArea.h"
#include "VideoPlayer.h"
#include "MoviePlayerWidget.h"

extern "C" void mylog(const char *fmt, ...);

static const int MAX_FRAMES_IN_Q = 256;

PreviewArea::PreviewArea(QWidget *parent)
    : QWidget(parent)
    , m_imageHelper(NULL)
    , m_seek_frame(0)
    , m_stopPlaying(false)
    , m_video_frames(MAX_FRAMES_IN_Q)
    , m_free_sem(MAX_FRAMES_IN_Q)
    , m_used_sem()
    , m_getImageFileToPreview(nullptr)
    , m_onBgImapeSelected(nullptr)
{
    loadDefaultImage();
    m_moviePlayerTimer = new QTimer(this);
    connect(m_moviePlayerTimer, SIGNAL(timeout()), this, SLOT(onUpdateMovieFrame()), Qt::QueuedConnection);
}

void PreviewArea::loadDefaultImage()
{
    QImage image(":/resources/checkeredBackground.jpg");
    m_pixPreviewSizeImage = m_pixOriginalSizeImage = QPixmap::fromImage(image);
    update();
}

void PreviewArea::init(std::function<QString()>getImageFileToPreview,
                       std::function<void()>onBgImapeSelected,
                       ImageHelper *imageHelper,
                       MoviePlayerWidget *moviePlayerWidget)
{
    m_getImageFileToPreview = getImageFileToPreview;
    m_onBgImapeSelected = onBgImapeSelected;
    m_imageHelper = imageHelper;
    connect(moviePlayerWidget, SIGNAL(play()), this, SLOT(playMovie()), Qt::QueuedConnection);
    connect(moviePlayerWidget, SIGNAL(stop()), this, SLOT(stopMovie()), Qt::QueuedConnection);
    connect(moviePlayerWidget, SIGNAL(seek(int)), this, SLOT(seekMovie(int)), Qt::QueuedConnection);
    connect(moviePlayerWidget, SIGNAL(volumeChanged(int)), this, SLOT(volumeChanged(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(currentPosition(int)), moviePlayerWidget, SLOT(setVideoSliderValue(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(moviePlayerWidgetHide()), moviePlayerWidget, SLOT(hide()), Qt::QueuedConnection);
    connect(this, SIGNAL(moviePlayerWidgetShow()), moviePlayerWidget, SLOT(show()), Qt::QueuedConnection);
    connect(this, SIGNAL(moviePlayerWidgetReset()), moviePlayerWidget, SLOT(reset()), Qt::QueuedConnection);
    connect(this, SIGNAL(moviePlayerWidgetSetDuration(int)), moviePlayerWidget, SLOT(setDuration(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(moviePlayerStopTimer()), this, SLOT(stopMoivePlayerTimer()), Qt::QueuedConnection);
    moviePlayerWidget->setVisible(false);
}

void PreviewArea::resizeEvent (QResizeEvent *e)
{
    update();
}

void PreviewArea::paintEvent(QPaintEvent *e)
{
    if(!m_pixPreviewSizeImage.isNull())
    {
        QImage checkeredBg(":/resources/checkeredBackground.jpg");
        QPainter painter;
        painter.begin(this);
        int w = width() > m_pixPreviewSizeImage.width() ? width() : m_pixPreviewSizeImage.width();
        int h = height() > m_pixPreviewSizeImage.height() ? width() : m_pixPreviewSizeImage.height();
        painter.drawTiledPixmap(0, 0, w, h,
                                QPixmap::fromImage(checkeredBg), 0, 0);
        //problem is m_pixPreviewSizeImage keeps chainging from big and small
//        mylog("paint %ix%i", m_pixPreviewSizeImage.width(), m_pixPreviewSizeImage.height());
        painter.drawPixmap(e->rect(), m_pixPreviewSizeImage, e->rect());
        painter.end();
    }
}

void PreviewArea::scaleForPreview(const QSize &outputImageSize, const QSize &previewWindowSize, int rotationAngle)
{
    if(m_pixOriginalSizeImage.isNull())
    {
        return;
    }

    m_pixPreviewSizeImage = m_pixOriginalSizeImage;
    if(!outputImageSize.isNull() && outputImageSize.isValid())
    {
        m_pixPreviewSizeImage =  m_pixPreviewSizeImage.scaled(outputImageSize);
    }
    if(!previewWindowSize.isNull() && previewWindowSize.isValid())
    {
//        scale = min( dst.width/src.width, dst.height/src.height)
        float f1 = previewWindowSize.width()/(float)m_pixPreviewSizeImage.width();
        float f2 = previewWindowSize.height()/(float)m_pixPreviewSizeImage.height();
        float scale = qMin<float>(f1, f2);

//        qDebug() << f1 << f2 << scale << scale * m_pixPreviewSizeImage.width() << scale * m_pixPreviewSizeImage.height();
        m_pixPreviewSizeImage = m_pixPreviewSizeImage.scaled(
                    scale * m_pixPreviewSizeImage.width(),
                    scale * m_pixPreviewSizeImage.height());

    }
    else if(outputImageSize.isNull() || !outputImageSize.isValid())
    {
        m_pixPreviewSizeImage = m_pixPreviewSizeImage.scaled(getOriginalImageSize());
    }
    if(rotationAngle != 0)
    {
        QTransform original;
        QTransform rotated = original.rotate(rotationAngle);
        m_pixPreviewSizeImage = m_pixPreviewSizeImage.transformed(rotated);
    }
    //        mylog("previ %ix%i", m_pixPreviewSizeImage.width(), m_pixPreviewSizeImage.height());
    setMinimumSize(QSize(m_pixPreviewSizeImage.width(), m_pixPreviewSizeImage.height()));
}

void PreviewArea::drawImage(const QSize &outputImageSize, const QSize &previewWindowSize, int rotationAngle)
{
    scaleForPreview(outputImageSize, previewWindowSize, rotationAngle);
    update();
    if(!m_onBgImapeSelected) { mylog("m_onBgImapeSelected"); }
    m_onBgImapeSelected();
}

void PreviewArea::setPreviewImage(const QImage &image)
{
    m_pixPreviewSizeImage = m_pixOriginalSizeImage = QPixmap::fromImage(image);
}

QImage PreviewArea::getVideoFirstFrame(const QString &src)
{
    QImage image;
    emit moviePlayerWidgetShow();
    stopMovie();
    m_av_mutex.lock();
    if(m_videoPlayer.open(src.toUtf8().data()))
    {
        const Decoder *d = m_videoPlayer.decoder();
        int audio_stream_index = d->st_index[AVMEDIA_TYPE_AUDIO];
        if(audio_stream_index >= 0 &&
                d->avctx[audio_stream_index])
        {
            m_audioPlayer.openAudio(
                    d->avctx[audio_stream_index]->channels,
                    d->avctx[audio_stream_index]->sample_rate,
                    d->avctx[audio_stream_index]->sample_fmt);
        }

        m_seek_frame = 0;
        int duration = m_videoPlayer.getDurationInMilliseconds();
        emit moviePlayerWidgetSetDuration(duration);
        m_stopPlaying = false;
        m_videoPlayer.play(&m_stopPlaying, [this,&image](AVFrame* frame, enum AVMediaType media_type){
            if(!frame)
            {
                return 0;
            }
            if(media_type != AVMEDIA_TYPE_VIDEO)
            {
                return 0;
            }
            if(frame->pict_type != AV_PICTURE_TYPE_NONE &&
                    frame->width > 0 &&
                    frame->height > 0)
            {
                image = QImage(frame->width, frame->height, QImage::Format_RGB32);
                m_videoPlayer.avframeToQImage(frame, &image);
            }
            av_frame_free(&frame);
            m_stopPlaying = true;
            return 1;
        });
    }
    m_av_mutex.unlock();
    return image;
}

void PreviewArea::setPixmapSrc()
{
    mylog("enter setPixmapSrc");
    emit moviePlayerWidgetHide();
    if(!m_getImageFileToPreview) { mylog("m_getImageFileToPreview"); }
    while(true)
    {
        QString src = m_getImageFileToPreview();
        if(src.size() == 0)
        {
            break;
        }
        QImage image;

        if(m_imageHelper->isVideoFile(src.toUtf8().data()))
        {
            image = getVideoFirstFrame(src);
        }
        else if(m_imageHelper->isSvg(src.toUtf8().data()))
        {
            image = m_imageHelper->getQImageFromSvg(src);
        }
        else
        {
            image.load(src);
        }

        if(!image.isNull())
        {
            setPreviewImage(image);
        }
        else
        {
            mylog("Failed to load %s. Check that you have enough memory and hard drive space..", src.size() ?
                      src.toUtf8().data() : "");
            QString supportedImages;
            foreach(QByteArray arr, QImageReader::supportedImageFormats())
            {
                supportedImages.append(arr.data());
                supportedImages.append(", ");
            }
            mylog("Supported formats are: %s", supportedImages.toUtf8().data());
        }
    }
    emit signalLoadedImageToPreview();
    mylog("leave setPixmapSrc");
}

QSize PreviewArea::getOriginalImageSize() const
{
    return QSize(m_pixOriginalSizeImage.width(), m_pixOriginalSizeImage.height());
}

QSize PreviewArea::getPreviewImageSize() const
{
    return !m_pixPreviewSizeImage.isNull() ?
                QSize(m_pixPreviewSizeImage.width(), m_pixPreviewSizeImage.height()) :
                QSize();
}

void PreviewArea::seekMovie(int milliseconds)
{
    stopMovie();

    QImage image;
    m_av_mutex.lock();
    m_stopPlaying = false;
    m_videoPlayer.play(&m_stopPlaying, [this,&image](AVFrame* frame, enum AVMediaType media_type){
        if(media_type != AVMEDIA_TYPE_VIDEO)
        {
            return 0;
        }
        image = QImage(frame->width, frame->height, QImage::Format_RGB32);
        m_videoPlayer.avframeToQImage(frame, &image);
        av_frame_free(&frame);

        m_stopPlaying = true;
        return 1;

    }, milliseconds);
    m_seek_frame = millisecondsToFrame(milliseconds);

    m_av_mutex.unlock();
    setPreviewImage(image);
    emit signalLoadedImageToPreview();
}

void PreviewArea::stopMovie()
{
    m_av_mutex.lock();
    m_stopPlaying = true;
    m_av_mutex.unlock();

    m_moviePlayerSession.cancel();
    m_moviePlayerSession.waitForFinished();

    emit moviePlayerWidgetReset();
    emit moviePlayerStopTimer();
    freeMovieBuffer();
}

void PreviewArea::stopMoivePlayerTimer()
{
    m_moviePlayerTimer->stop();
}

void PreviewArea::playMovie()
{
    if(m_moviePlayerTimer->isActive())
    {
        stopMovie();
    }
    else
    {
        m_moviePlayerSession.cancel();
        m_moviePlayerSession.waitForFinished();
        m_stopPlaying = false;
        int interval = m_videoPlayer.getFrameIntervalMSec();
        if(interval > 0)
        {
            m_moviePlayerTimer->start(interval);
            m_play_time = QTime::currentTime();
            mylog("Frame rate %i, timer interval %i ms",
                  m_videoPlayer.getFrameRate(),
                  m_moviePlayerTimer->interval());

            m_moviePlayerSession =
            QtConcurrent::run(this, &PreviewArea::playMovieAsync,
                            frameToMilliseconds(m_seek_frame));
        }
        else
        {
            mylog("Could not determine frame rate (frames/second). Cannot play.");
        }
    }
}

void PreviewArea::playMovieAsync(int startMilliseconds)
{
    m_av_mutex.lock();
    m_stopPlaying = false;
    m_last_buffered_frame = 0;
    m_first_buffered_frame = 0;
    m_play_time.start();
    m_videoPlayer.play(&m_stopPlaying,
        [this](AVFrame* frame, enum AVMediaType media_type){

        int handled = 0;
        switch(media_type)
        {
        case AVMEDIA_TYPE_DATA:
        case AVMEDIA_TYPE_VIDEO:
            m_av_mutex.unlock();
            handled = addVideoFrame(frame) ? 1:0;
            m_av_mutex.lock();
            break;
        case AVMEDIA_TYPE_AUDIO:
            m_av_mutex.unlock();
            handled = addAudioFrame(frame) ? 1:0;
            m_av_mutex.lock();
            break;
        default:
            mylog("recvd frame of type %i\n", media_type);
            break;
        }
        return handled;
    },
    startMilliseconds);
    m_av_mutex.unlock();
}

bool PreviewArea::addAudioFrame(AVFrame *frame)
{
    if(!frame)
    {
        return false;
    }

    m_audioPlayer.addAudioFrame(frame);
    av_frame_free(&frame);

    return true;
}

#define USE_MEM_Q
//#define USE_FILE_Q
bool PreviewArea::addVideoFrame(AVFrame *frame)
{
#ifdef USE_MEM_Q
//    mylog("Video buffer %i%%", m_used_sem.available()* 100 /MAX_FRAMES_IN_Q);
    while(!m_free_sem.tryAcquire())
    {
//        mylog("Video queue is full, waiting");
        QThread::msleep(10);

        m_av_mutex.lock();
        if(m_stopPlaying)
        {
            m_av_mutex.unlock();
            return false;
        }
        m_av_mutex.unlock();
    }

    RawFrame *raw_frame = new RawFrame;
    raw_frame->fromAVFrame(frame, frame->width, frame->height, 4,
            (AVPixelFormat) frame->format,
            frame->pkt_pts, frame->pkt_pos);
    av_frame_free(&frame);

    m_video_frames.add(raw_frame);
    m_used_sem.release();

    return true;

#endif//USE_MEM_Q

#ifdef USE_FILE_Q
    RawFrame raw_frame;
    raw_frame.fromAVFrame(frame, frame->width, frame->height, 4,
                          (AVPixelFormat) frame->format,
                          frame->pkt_pts, frame->pkt_pos);
    av_frame_free(&frame);
    QString path = QString("%1/frame%2.raw")
            .arg(m_tmp_dir.path())
            .arg(m_first_buffered_frame++, 5, 10, QChar('0'));
    raw_frame.serialize(path);

    return true;
#endif//USE_FILE_Q

}

void PreviewArea::playFrame(RawFrame *raw_frame)
{
    if(!raw_frame)
    {
        return;
    }
    if(!raw_frame->m_bits)
    {
        delete raw_frame;
        return;
    }
    QImage image(raw_frame->m_width, raw_frame->m_height, QImage::Format_RGB32);
    if(!image.isNull())
    {
        memcpy(image.bits(), raw_frame->m_bits,
               raw_frame->m_width * raw_frame->m_height * 4);

        m_av_mutex.lock();
        if(m_stopPlaying)
        {
            m_av_mutex.unlock();
            return;
        }
        m_av_mutex.unlock();

        setPreviewImage(image);
        emit signalLoadedImageToPreview();
        emit currentPosition(frameToMilliseconds(raw_frame->m_frame_n));
    }
    delete raw_frame;
}

void PreviewArea::onUpdateMovieFrame()
{
//    mylog("timer %i", m_free_sem.available());
#ifdef USE_MEM_Q
    RawFrame *raw_frame = nullptr;
    while(m_free_sem.available() < MAX_FRAMES_IN_Q)
    {
        if(!m_used_sem.tryAcquire())
        {
            m_av_mutex.lock();
            if(m_stopPlaying)
            {
                m_av_mutex.unlock();
                return;
            }
            m_av_mutex.unlock();
            QApplication::processEvents();
            continue;
        }
        raw_frame = m_video_frames.remove();
        m_free_sem.release();
        int64_t frame_n = raw_frame->m_frame_n;
        playFrame(raw_frame);

        if(frame_n * m_videoPlayer.getFrameIntervalMSec()
                > m_play_time.elapsed())
        {
            break;
        }
    }
#endif//USE_MEM_Q

#ifdef USE_FILE_Q
    RawFrame raw_frame;
    while(true)
    {
        QString path = QString("%1/frame%2.raw")
                .arg(m_tmp_dir.path())
                .arg(m_last_buffered_frame, 5, 10, QChar('0'));
        if(!raw_frame.deserialize(path))
        {
            if(raw_frame.m_bits)
            {
                break;
            }
            return;
        }
        m_last_buffered_frame++;
        if(raw_frame.m_frame_n * m_videoPlayer.getFrameIntervalMSec() >=
                m_play_time.elapsed())
        {
            break;
        }
    }

    QImage image(raw_frame.m_width, raw_frame.m_height, QImage::Format_RGB32);
    if(!image.isNull())
    {
        memcpy(image.bits(), raw_frame.m_bits,
               raw_frame.m_width * raw_frame.m_height * 4);

        m_av_mutex.lock();
        if(m_stopPlaying)
        {
            m_av_mutex.unlock();
            return;
        }
        setPreviewImage(image);
        emit signalLoadedImageToPreview();
        emit currentPosition(frameToMilliseconds(raw_frame->m_frame_n));
        m_av_mutex.unlock();
    }
#endif //USE_FILE_Q

}

void PreviewArea::freeMovieBuffer()
{
    m_av_mutex.lock();
    m_stopPlaying = true;

    while(!m_video_frames.isEmpty())
    {
        m_used_sem.acquire();
        auto frame = m_video_frames.remove();
        delete frame;
    }
    m_av_mutex.unlock();
}

int PreviewArea::millisecondsToFrame(int milliseconds) const
{
    return milliseconds * m_videoPlayer.getFrameRate() / 1000;
}

int PreviewArea::frameToMilliseconds(int frame) const
{
    if(!m_videoPlayer.getFrameRate())
    {
        return 0;
    }
    return frame * 1000 / m_videoPlayer.getFrameRate();
}

void PreviewArea::volumeChanged(int percent)
{
    m_audioPlayer.setVolume(percent);
}

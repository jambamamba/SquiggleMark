#pragma once

#include <functional>
#include <QFuture>
#include <QGraphicsView>
#include <QList>
#include <QMutex>
#include <QSemaphore>
#include <QTemporaryDir>
#include <QTime>
#include <QTimer>

#include "../DS/circularbuffer.h"
#include "AudioPlayer.h"
#include "ImageHelper.h"
#include "MainWindowInterface.h"
#include "VideoPlayer.h"


class MoviePlayerWidget;
class PreviewArea : public QWidget
{
    Q_OBJECT
public:
    explicit PreviewArea(QWidget *parent = 0);
    void init(std::function<QString()>getImageFileToPreview,
              std::function<void()>onBgImapeSelected,
              ImageHelper * imageHelper, MoviePlayerWidget *moviePlayerWidget);
    void setPixmapSrc();
    QSize getOriginalImageSize() const;
    QSize getPreviewImageSize() const;
    void drawImage(const QSize &outputImageSize, const QSize &previewWindowSize, int rotationAngle);
    bool isPlayingMovie() const { return m_moviePlayerTimer && m_moviePlayerTimer->isActive(); }

signals:
    void signalLoadedImageToPreview();
    void currentPosition(int);
    void moviePlayerWidgetHide();
    void moviePlayerWidgetShow();
    void moviePlayerWidgetReset();
    void moviePlayerWidgetSetDuration(int);
    void moviePlayerStopTimer();

protected slots:
    void onUpdateMovieFrame();
    void playMovie();
    void seekMovie(int milliseconds);
    void stopMoivePlayerTimer();
    void volumeChanged(int percent);
public slots:
    void stopMovie();

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent (QResizeEvent *e);

private:
    QPixmap m_pixOriginalSizeImage;
    QPixmap m_pixPreviewSizeImage;
    QTimer *m_moviePlayerTimer;
    VideoPlayer m_videoPlayer;
    AudioPlayer m_audioPlayer;
    int m_seek_frame;
    bool m_stopPlaying;
    ImageHelper *m_imageHelper;
    QFuture<void> m_moviePlayerSession;
    QMutex m_av_mutex;
    QSemaphore m_free_sem;
    QSemaphore m_used_sem;
    CIRCULAR_Q<RawFrame*> m_video_frames;
    std::function<QString()>m_getImageFileToPreview;
    std::function<void()>m_onBgImapeSelected;
    QTemporaryDir m_tmp_dir;
    int m_last_buffered_frame;
    int m_first_buffered_frame;
    QTime m_play_time;

    void loadDefaultImage();
    void playMovieAsync(int startMilliseconds);
    bool addVideoFrame(AVFrame *frame);
    bool addAudioFrame(AVFrame *frame);
    void setPreviewImage(const QImage &image);
    void freeMovieBuffer();
    QImage getVideoFirstFrame(const QString &src);
    void scaleForPreview(const QSize &outputImageSize, const QSize &previewWindowSize, int rotationAngle);
    int millisecondsToFrame(int milliseconds) const;
    int frameToMilliseconds(int frame) const;
    void playFrame(RawFrame *raw_frame);
};

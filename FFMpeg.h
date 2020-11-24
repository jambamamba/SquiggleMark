#ifndef FFMPEG_H
#define FFMPEG_H

#include <QProcess>
#include <QRect>

class FFMpeg : public QObject
{
    Q_OBJECT
public:
    FFMpeg(QObject *controlsAreaWidget, int numFrames, bool rotate, int rotationAngle, bool scale, const QRect &crop_rect, int sx, int sy, int quality, const bool *stop);
    virtual ~FFMpeg();

    void waterMark(const QString &inMovie, const QString &outMovie, const QString &watermarkImg, int x, int y);

signals:
    void signalProgress(int);
    void signalEndProgress();

protected slots:
    void onFFmpegDone(int, QProcess::ExitStatus);
    void onFFmpegOutput();
    void onFFmpegError();

private:
    QProcess *m_p;
    int m_prevFrameNum;
    int m_numFrames;
    bool m_rotate;
    int m_rotationAngle;
    bool m_scale;
    QRect m_crop_rect;
    int m_sx;
    int m_sy;
    int m_quality;
    const bool *m_stop;

    void killIfStopped();
    void logFFmpegOutput(const QString &msg);
    QString transformVideo(const QString &exeFullPath, const QString &movIn, const QString &quality, const QProcessEnvironment &env, const QString &transformName, const QString &filter, const QString &movieOut);
    bool runFFmpeg(QProcess *process, const QString &exeFullPath, const QStringList &args, const QProcessEnvironment &env);
};

#endif // FFMPEG_H

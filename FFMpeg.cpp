#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QProcessEnvironment>

#include "FFMpeg.h"

extern "C" void mylog(const char *fmt, ...);

FFMpeg::FFMpeg(QObject *controlsAreaWidget, int numFrames, bool rotate, int rotationAngle, bool scale, const QRect &crop_rect, int sx, int sy, int quality, const bool *stop)
    : m_p(nullptr)
    , m_prevFrameNum(0)
    , m_numFrames(numFrames)
    , m_rotate(rotate)
    , m_rotationAngle(rotationAngle)
    , m_crop_rect(crop_rect)
    , m_scale(scale)
    , m_sx(sx)
    , m_sy(sy)
    , m_stop(stop)
    , m_quality(quality)
{
    connect(this, SIGNAL(signalProgress(int)), controlsAreaWidget, SIGNAL(signalProgress(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(signalEndProgress()), controlsAreaWidget, SIGNAL(signalEndProgress()), Qt::QueuedConnection);
}

FFMpeg::~FFMpeg()
{
}

void FFMpeg::killIfStopped()
{
    if(*m_stop)
    {
        m_p->kill();
        emit signalEndProgress();
    }
}
QString FFMpeg::transformVideo(const QString &exeFullPath, const QString &movIn, const QString &quality, const QProcessEnvironment &env, const QString &transformName, const QString &filter, const QString &movieOut)
{
    QFileInfo info(movieOut);
    QString outName = QDir::toNativeSeparators(
        info.absolutePath() + QString("/") +
        info.completeBaseName() + QString(".%1.").arg(transformName) + info.suffix()
                );
    QFile outFile(outName);
    outFile.remove();

    QStringList args;
    args << "-i" << movIn
         << "-vf" << filter
         << "-qscale:v" << quality
//         << "-acodec" << "copy"
         << outName;

    if(!runFFmpeg(m_p, exeFullPath, args, env))
    {
        return "";
    }
    return outName;
}

void FFMpeg::waterMark(const QString &movieIn, const QString &movieOut, const QString &watermarkImg, int x, int y)
{
    if(m_p) delete m_p;
    m_p = new QProcess;
    m_prevFrameNum = 0;

    connect(m_p, &QProcess::readyReadStandardError, this, &FFMpeg::onFFmpegError);
    connect(m_p, &QProcess::readyReadStandardOutput, this, &FFMpeg::onFFmpegOutput);
    connect(m_p, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onFFmpegDone(int, QProcess::ExitStatus)));

    QString path = QCoreApplication::applicationFilePath();
    QFileInfo info(path);
    QString exePath = info.absolutePath();
    exePath = QDir::toNativeSeparators(exePath);
#if defined(QT_DEBUG)
    exePath = "c:/work/libs/build/win64";
#endif
    QString exeFullPath = QString("%1/ffmpeg").arg(exePath);
#if defined(Win32) || defined(Win64)
    exeFullPath.append(".exe");
#endif
    exeFullPath = QDir::toNativeSeparators(exeFullPath);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("LD_LIBRARY_PATH", exePath);
    m_p->setProcessEnvironment(env);

    QStringList intermediateFiles;
    QString movIn(movieIn);
    QStringList args;
    QString quality = QString::number(m_quality);//1:best 31:worst

    if(m_crop_rect.width() > 1 && m_crop_rect.height() > 1)
    {
        QString crop = QString("crop=%1:%2:%3:%4").
                    arg(m_crop_rect.width()).
                    arg(m_crop_rect.height()).
                    arg(m_crop_rect.left()).
                    arg(m_crop_rect.top());
        movIn = QDir::toNativeSeparators(movIn);
        movIn = transformVideo(exeFullPath, movIn, quality, env, "crop", crop, movieOut);
        intermediateFiles << movIn;
        if(movIn.isEmpty())
        {
            goto Exit;
        }
    }

    if(m_rotate)
    {
        QString rotation = QString("rotate=%1").arg(
                    (double)(-m_rotationAngle * 3.14159265359f/180.0f));
        movIn = QDir::toNativeSeparators(movIn);
        movIn = transformVideo(exeFullPath, movIn, quality, env, "rotate", rotation, movieOut);
        intermediateFiles << movIn;
        if(movIn.isEmpty())
        {
            goto Exit;
        }
    }

    if(m_scale)
    {
        QString scale = QString("scale=%1:%2").arg(m_sx).arg(m_sy);
        movIn = QDir::toNativeSeparators(movIn);
        movIn = transformVideo(exeFullPath, movIn, quality, env, "scale", scale, movieOut);
        intermediateFiles << movIn;
        if(movIn.isEmpty())
        {
            goto Exit;
        }
    }

    if(!watermarkImg.size())
    {
        QFile outFile(movieOut);
        outFile.remove();

        QFile inFile(movIn);
        inFile.rename(movieOut);
    }
    else
    {
        QFile outFile(movieOut);
        outFile.remove();
        movIn = QDir::toNativeSeparators(movIn);

        args.clear();
        args << "-i" << movIn <<
                "-i" << watermarkImg <<
                "-filter_complex" << QString("overlay=%1:%2").arg(x).arg(y) <<
                "-qscale:v" << quality <<
//                "-acodec" << "copy" <<
                movieOut;

        if(!runFFmpeg(m_p, exeFullPath, args, env))
        {
            goto Exit;
        }
    }
Exit:
    for(int i=0 ; i<intermediateFiles.size(); ++i)
    {
        QFile intFile(intermediateFiles.at(i));
        intFile.remove();
    }
    delete m_p;
    m_p = nullptr;
}

bool FFMpeg::runFFmpeg(QProcess *process, const QString &exeFullPath, const QStringList &args, const QProcessEnvironment &env)
{
    process->start(exeFullPath, args);
    if(!process->waitForStarted(-1))
    {
        mylog("failed to start %s %s, path: %s",
              exeFullPath.toUtf8().data(),
              args.join(" ").toUtf8().data(),
              env.value("LD_LIBRARY_PATH").toUtf8().data());
        emit signalEndProgress();
        return false;
    }
    else
    {
        mylog("started %s %s", exeFullPath.toUtf8().data(), args.join(" ").toUtf8().data());
    }

    while(!process->waitForFinished(400))
    {
        killIfStopped();
        if(process->state() == QProcess::NotRunning)
        {
            break;
        }
    }
    return true;
}

void FFMpeg::onFFmpegOutput()
{
    QString out = m_p->readAllStandardOutput();
    if(out.count()) logFFmpegOutput(out);
}

void FFMpeg::onFFmpegError()
{
    QString err = m_p->readAllStandardError();
    if(err.count()) logFFmpegOutput(err);
}

void FFMpeg::logFFmpegOutput(const QString &msg)
{
    if(*m_stop)
    {
        return;
    }

    char buffer[1024] = {0};
    memcpy(buffer, msg.toUtf8().data(),
           msg.count() > 1023 ? 1023 : msg.count());
    mylog(buffer);

    QRegExp rx("frame=\\s*(\\d*).*");
    int pos = 0;
    while((pos = rx.indexIn(msg, pos)) != -1)
    {
        int frameNum = rx.cap(1).toInt();
        int framesProcessed = frameNum - m_prevFrameNum;
        mylog("processed %i frames", framesProcessed);
        emit signalProgress(framesProcessed);
        m_prevFrameNum = frameNum;

        pos += rx.matchedLength();
    }
}

void FFMpeg::onFFmpegDone(int exitCode, QProcess::ExitStatus exitStatus)
{
    int framesProcessed = m_numFrames>0 ? m_numFrames - m_prevFrameNum : m_prevFrameNum;
    emit signalProgress(framesProcessed);
    m_prevFrameNum = 0;
    mylog("ffmpeg %s with exit code: %i", exitStatus==QProcess::NormalExit ?
              "done" : "crashed", exitCode);
}

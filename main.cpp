#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QThreadPool>
#include <stdlib.h>
#include <string.h>
#include <cstdio>
#include <functional>

#include "MainWindow.h"

static MainWindow *mainWindow;

extern "C" {
#include <stdio.h>
#include <time.h>
void mylog(const char *fmt, ...)
{

    time_t t = time(0);
    char tbuffer [128];
    tm * timeinfo = localtime ( &t );
    strftime (tbuffer, 80,"%y-%m-%d %H:%M:%S", timeinfo);

    char buffer1[1024];
    char buffer2[1024+128];
    va_list args;
    va_start(args, fmt);
    vsprintf (buffer1, fmt, args);
    sprintf(buffer2, "%s - %s\n", tbuffer, buffer1);
    QString *msg = new QString(buffer2);
    qDebug() << *msg;
    if(mainWindow)
    {
        mainWindow->appendLogMessage(msg);
    }
    va_end(args);
}
}//extern "C"

static void initializeThreadPool()
{
    int num_threads = QThreadPool::globalInstance()->maxThreadCount();
    mylog("max threads in pool: %i", num_threads);
    int num_cores = QThread::idealThreadCount();
    int new_num_threads = 8;
    if(num_threads < num_cores)
    {
        new_num_threads = num_cores * 2;
    }
    if(new_num_threads <= 2)
    {
        new_num_threads = 4;
    }
    if(new_num_threads > num_threads)
    {
        QThreadPool::globalInstance()->setMaxThreadCount(new_num_threads);
    }

    mylog("num cores: %i, num threads: %i, new_num_threads: %i", num_cores, num_threads, new_num_threads);
}

static void initializeForDarwin(const char* path)
{
#if defined(Q_OS_DARWIN)
#if defined(NDEBUG)
    QDir dir(path);
    dir.cdUp();
    dir.cd("plugins");
    QCoreApplication::setLibraryPaths(QStringList(dir.absolutePath()));
    mylog("library paths: %s",
          QCoreApplication::libraryPaths().join(",").toUtf8().data());
    mylog("exe path: %s",
        QString(path).toUtf8().data());
#else
    system("ln -s /Users/OsleTek/work/libs/build/osx/bin/libcrypto.1.1.dylib /usr/local/lib/libcrypto.1.1.dylib");
    system("ln -s /Users/OsleTek/work/libs/build/osx/bin/libssl.1.1.dylib /usr/local/lib/libssl.1.1.dylib");
    system("ln -s /Users/OsleTek/work/libs/build/osx/bin/libfreetype.6.dylib  /usr/local/lib/libfreetype.6.dylib");
    system("ln -s /Users/OsleTek/work/libs/build/osx/bin/libavcodec.57.dylib /usr/local/lib/libavcodec.57.dylib");
    system("ln -s /Users/OsleTek/work/libs/build/osx/bin/libavdevice.57.dylib /usr/local/lib/libavdevice.57.dylib");
    system("ln -s /Users/OsleTek/work/libs/build/osx/bin/libavfilter.6.dylib /usr/local/lib/libavfilter.6.dylib");
    system("ln -s /Users/OsleTek/work/libs/build/osx/bin/libavformat.57.dylib /usr/local/lib/libavformat.57.dylib");
    system("ln -s /Users/OsleTek/work/libs/build/osx/bin/libavutil.55.dylib /usr/local/lib/libavutil.55.dylib");
    system("ln -s /Users/OsleTek/work/libs/build/osx/bin/libswresample.2.dylib /usr/local/lib/libswresample.2.dylib");
    system("ln -s /Users/OsleTek/work/libs/build/osx/bin/libswscale.4.dylib /usr/local/lib/libswscale.4.dylib");
    system("ln -s /Users/OsleTek/work/libs/build/osx/bin/libcurl.4.dylib /usr/local/lib/libcurl.4.dylib");
    system("ln -s /Users/OsleTek/work/libs/build/osx/bin/libvorbisenc.2.dylib /usr/local/lib/libvorbisenc.2.dylib");
    system("ln -s /Users/OsleTek/work/libs/build/osx/bin/libvorbis.0.dylib /usr/local/lib/libvorbis.0.dylib");
    system("ln -s /Users/OsleTek/work/libs/build/osx/bin/libogg.0.dylib /usr/local/lib/libogg.0.dylib");
    system("ln -s /Users/OsleTek/work/libs/build/osx/bin/libtheoraenc.1.dylib /usr/local/lib/libtheoraenc.1.dylib");
    system("ln -s /Users/OsleTek/work/libs/build/osx/bin/libtheoradec.1.dylib /usr/local/lib/libtheoradec.1.dylib");
    system("ln -s /Users/OsleTek/work/libs/build/osx/bin/libopenh264.2.dylib /usr/local/lib/libopenh264.2.dylib");
#endif
#endif
}

extern "C" int main(int argc, char *argv[])
{
    mylog("starting application in %s",
          argv[0] ? argv[0] : "");
    initializeForDarwin(argv[0]);
    initializeThreadPool();
    QApplication a(argc, argv);
    int ret = 0;
//    try {
        MainWindow w;
        mainWindow = &w;
        w.show();
        ret = a.exec();
//    } catch(const std::bad_function_call& e) {
//        mylog("caught excpetion in main: %s", e.what());
//    }
    return ret;
}

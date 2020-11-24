#include <QSlider>
#include <QToolButton>

#include "MoviePlayerWidget.h"
#include "ui_MoviePlayerWidget.h"

#include "VideoPlayer.h"

extern "C" void mylog(const char *fmt, ...);

namespace {
QString humanReadableTime(int ms)
{
    int days = ms / 1000 / 60 / 60 / 24;
    ms -= (days * 24 * 60 * 60 * 1000);

    int hours = ms / 1000 / 60 / 60;
    ms -= (hours * 60 * 60 * 1000);

    int mins = ms / 1000 / 60;
    ms -= (mins * 60 * 1000);

    int secs = ms / 1000;
    ms -= (secs * 1000);

    QString ret;
    if(days > 0)
    {
        ret.append(QString("%1 day%2 ").arg(days).arg(days > 1 ? "s" : ""));
    }
    if(hours > 0)
    {
        auto h = QString("%1:").arg(hours, 2, 'g', -1, '0');
        ret.append(h);
    }
    auto m = QString("%1:").arg(mins, 2, 'g', -1, '0');
    ret.append(m);
    auto s = QString("%1.").arg(secs, 2, 'g', -1, '0');
    ret.append(s);
    auto i = QString("%1").arg(ms, 3, 'g', -1, '0');
    ret.append(i);

    return ret;
}
}

MoviePlayerWidget::MoviePlayerWidget(QWidget *parent) :
    QWidget(parent),
    m_userMovingSlider(false),
    ui(new Ui::MoviePlayerWidget),
    m_maxDuration(0),
    m_playing(false)
{
    ui->setupUi(this);
    ui->volumeSlider->setMinimum(0);
    ui->volumeSlider->setMaximum(100);
    ui->volumeSlider->setValue(100);
    emit volumeChanged(100);
}

MoviePlayerWidget::~MoviePlayerWidget()
{
    delete ui;
}

void MoviePlayerWidget::reset()
{
    ui->playButton->setIcon(QIcon(":/resources/play-button.png"));
    ui->videoSlider->setTickPosition(QSlider::NoTicks);
}

void MoviePlayerWidget::setDuration(int milliseconds)
{
    ui->videoSlider->setMaximum(milliseconds);
    ui->videoTimeLabel->setText(humanReadableTime(milliseconds));
    ui->videoSlider->setValue(0);
    m_maxDuration = milliseconds;
}

void MoviePlayerWidget::on_playButton_clicked()
{
    m_playing ^= 1;
    ui->playButton->setIcon( m_playing ?
        QIcon(":/resources/pause-button.png") : QIcon(":/resources/play-button.png"));
    emit play();
}

void MoviePlayerWidget::setTimeLabel(int milliseconds)
{
    QString label = QString("%1/%2").arg(humanReadableTime(milliseconds)).arg(humanReadableTime(m_maxDuration));
    ui->videoTimeLabel->setText(label);
}

void MoviePlayerWidget::setVideoSliderValue(int milliseconds)
{
    setTimeLabel(milliseconds);
    ui->videoSlider->setValue(milliseconds);
}

void MoviePlayerWidget::on_videoSlider_sliderPressed()
{
    m_userMovingSlider = true;
    emit stop();
}

void MoviePlayerWidget::on_videoSlider_sliderReleased()
{
    m_playing = false;
    ui->playButton->setIcon(QIcon(":/resources/play-button.png"));
    m_userMovingSlider = false;
    int milliseconds = ui->videoSlider->value();
    emit seek(milliseconds);
}

void MoviePlayerWidget::on_videoSlider_valueChanged(int milliseconds)
{
    if(m_userMovingSlider)
    {
        setTimeLabel(milliseconds);
    }
}

void MoviePlayerWidget::on_volumeSlider_valueChanged(int percent)
{
    mylog("volume slider %i", percent);
    emit volumeChanged(percent);
}

void MoviePlayerWidget::setVolume(int percent)
{
    ui->volumeSlider->setValue(percent);
}

void MoviePlayerWidget::on_speakerButton_clicked()
{
    int percent = ui->speakerButton->isChecked() ? 0 : 100;
    ui->speakerButton->isChecked() ?
                ui->speakerButton->setIcon(QIcon(":/resources/speaker-button.png")) :
                ui->speakerButton->setIcon(QIcon(":/resources/speaker-button-mute.png"));
    ui->volumeSlider->setValue(percent);
    emit volumeChanged(percent);
}

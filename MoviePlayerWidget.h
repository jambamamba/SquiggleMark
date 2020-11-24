#ifndef MOVIEPLAYERWIDGET_H
#define MOVIEPLAYERWIDGET_H

#include <QWidget>

namespace Ui {
class MoviePlayerWidget;
}

class MoviePlayerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MoviePlayerWidget(QWidget *parent = 0);
    ~MoviePlayerWidget();

signals:
    void play();
    void stop();
    void seek(int);
    void volumeChanged(int);

private slots:
    void on_playButton_clicked();
    void on_videoSlider_valueChanged(int milliseconds);
    void setVideoSliderValue(int milliseconds);
    void on_videoSlider_sliderPressed();
    void on_videoSlider_sliderReleased();
    void on_volumeSlider_valueChanged(int percent);
    void on_speakerButton_clicked();

public slots:
    void setDuration(int milliseconds);
    void reset();
public:
    void setVolume(int);

private:
    Ui::MoviePlayerWidget *ui;
    bool m_userMovingSlider;
    int m_maxDuration;
    bool m_playing;
    unsigned long m_volume;

    void setTimeLabel(int milliseconds);
};

#endif // MOVIEPLAYERWIDGET_H

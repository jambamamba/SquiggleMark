#ifndef QUALITYSLIDER_H
#define QUALITYSLIDER_H

#include <QWidget>

namespace Ui {
class QualitySlider;
}

class QXmlStreamReader;
class QXmlStreamWriter;

class QualitySlider : public QWidget
{
    Q_OBJECT

public:
    explicit QualitySlider(const char* type, int min, int max, int value,
                           const QString &left_lable, const QString right_label, QWidget *parent = 0);
    ~QualitySlider();

    void saveSettings(QXmlStreamWriter &writer);
    void loadSettings(QXmlStreamReader  &reader);
    int getQuality() const;

private slots:
    void on_horizontalSlider_sliderMoved(int position);

private:
    Ui::QualitySlider *ui;
    QString m_type;
    int m_min;
    int m_max;

    void updateWidget(int value);
};

#endif // QUALITYSLIDER_H

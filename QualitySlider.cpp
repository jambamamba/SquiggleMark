#include <QXmlStreamReader>
#include <QXmlStreamWriter>


#include "QualitySlider.h"
#include "ui_QualitySlider.h"

QualitySlider::QualitySlider(const char *type, int min, int max, int value,
    const QString &left_lable, const QString right_label, QWidget *parent)
    : m_type(type)
    , m_min(min)
    , m_max(max)
    , QWidget(parent)
    , ui(new Ui::QualitySlider)
{
    ui->setupUi(this);

    ui->groupBox->setTitle(type);
    ui->horizontalSlider->setMinimum(min);
    ui->horizontalSlider->setMaximum(max);
    ui->labelLeft->setText(left_lable);
    ui->labelRight->setText(right_label);
    updateWidget(value);
}

QualitySlider::~QualitySlider()
{
    delete ui;
}

void QualitySlider::saveSettings(QXmlStreamWriter &writer)
{
    writer.writeStartElement(m_type);
    writer.writeAttribute("Quality", QString("%1").arg(ui->horizontalSlider->value()));
    writer.writeEndElement();
}

void QualitySlider::loadSettings(QXmlStreamReader  &reader)
{
    if(reader.name() == m_type)
    {
        foreach(const QXmlStreamAttribute &attribute, reader.attributes())
        {
            if(attribute.name() == "Quality")
            {
                updateWidget(attribute.value().toInt());
            }
        }
    }
}

void QualitySlider::updateWidget(int value)
{
    if(value >= m_min && value <= m_max)
    {
        ui->labelMiddle->setText(QString::number(value));
        ui->horizontalSlider->setValue(value);
    }
}

void QualitySlider::on_horizontalSlider_sliderMoved(int position)
{
    ui->labelMiddle->setText(QString::number(position));
}

int QualitySlider::getQuality() const
{
    return ui->horizontalSlider->value();
}

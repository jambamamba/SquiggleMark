#include "RotationWidget.h"
#include "ui_RotationWidget.h"

RotationWidget::RotationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RotationWidget)
{
    ui->setupUi(this);
    qRegisterMetaType<QTextCursor>("QTextCursor");
}

RotationWidget::~RotationWidget()
{
    delete ui;
}

int RotationWidget::value() const
{
    return ui->dial->value();
}

void RotationWidget::update(int value)
{
    ui->angleText->setPlainText( QString("%1").arg( value ));
    ui->dial->setSliderPosition( value );
}

void RotationWidget::on_dial_valueChanged(int value)
{
    ui->angleText->setPlainText(QString("%1").arg(value));
    emit signalRotate(value);
}

void RotationWidget::on_resetButton_clicked()
{
    ui->angleText->setPlainText("0");
    ui->dial->setSliderPosition(0);
}

void RotationWidget::on_angleText_textChanged()
{
    if (ui->angleText->toPlainText().length() > 4)
    {
        ui->angleText->setPlainText(ui->angleText->toPlainText().left(ui->angleText->toPlainText().length()-1));
        ui->angleText->moveCursor(QTextCursor::End);
    }
    int angle = ui->angleText->toPlainText().toInt();
    if( angle >= -180 && angle <= 180 )
    {
       ui->dial->setSliderPosition( angle );
       emit signalRotate(angle);
    }
}

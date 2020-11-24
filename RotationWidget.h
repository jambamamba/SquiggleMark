#ifndef ROTATIONWIDGET_H
#define ROTATIONWIDGET_H

#include <QWidget>

namespace Ui {
class RotationWidget;
}

class RotationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RotationWidget(QWidget *parent = 0);
    ~RotationWidget();

    int value() const;
    void update(int value);

signals:
    void signalRotate(int);

private slots:
    void on_dial_valueChanged(int value);
    void on_resetButton_clicked();
    void on_angleText_textChanged();

private:
    Ui::RotationWidget *ui;
};

#endif // ROTATIONWIDGET_H

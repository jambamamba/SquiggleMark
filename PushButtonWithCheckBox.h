#ifndef PUSHBUTTONWITHCHECKBOX_H
#define PUSHBUTTONWITHCHECKBOX_H

#include <QCheckBox>
#include <QPushButton>

class PushButtonWithCheckBox : public QPushButton
{
    Q_OBJECT
public:
    explicit PushButtonWithCheckBox(QWidget *parent = 0);
    virtual void paintEvent( QPaintEvent* event );
    QSize sizeHint () const { return QSize( 128, 128 ); }

signals:

public slots:

private:
    QCheckBox *m_checkBox;

};

#endif // PUSHBUTTONWITHCHECKBOX_H

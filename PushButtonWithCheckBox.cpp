#include <QPainter>
#include <QStyleOptionButton>

#include "PushButtonWithCheckBox.h"


PushButtonWithCheckBox::PushButtonWithCheckBox(QWidget *parent) :
    QPushButton(parent),
    m_checkBox(this)
{
}

void PushButtonWithCheckBox::paintEvent( QPaintEvent* event )
{
    Q_UNUSED( event )
    QPainter p( this );

    //  QStyleOption, and derived classes, describe the state of the widget.
    QStyleOptionButton butOpt;
    butOpt.initFrom( this ); // A convenience function that steals state
                             // from this widget.
    butOpt.state = QStyle::State_Enabled;
    butOpt.state |= down_ ? QStyle::State_Sunken : QStyle::State_Raised;

    //  Renders the widget onto the QPainter.
    style()->drawControl( QStyle::CE_PushButton, &butOpt, &p, this );

    //  pixelMetric() gives you style relevant dimensional data.
    int chkBoxWidth = style()->pixelMetric( QStyle::PM_CheckListButtonSize,
                                            &butOpt, this ) / 2;
    butOpt.rect.moveTo( ( rect().width() / 2 ) - chkBoxWidth, 0 );
    butOpt.state |= checked_ ? QStyle::State_On : QStyle::State_Off;
    style()->drawControl( QStyle::CE_CheckBox, &butOpt, &p, this );
}

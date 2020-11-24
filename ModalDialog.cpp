#include "ModalDialog.h"

#include <QDialog>
#include <QGridLayout>

ModalDialog::ModalDialog(QObject *parent) : QObject(parent)
{

}

QDialog *ModalDialog::newModalDialog(QWidget *widget, const QString &title)
{
    QDialog *dlg = new QDialog;
    QLayout *layout = new QGridLayout;
    layout->addWidget(widget);
    dlg->setWindowTitle(title);
    dlg->setLayout(layout);
    dlg->setModal(true);
    dlg->exec();
    return dlg;
}


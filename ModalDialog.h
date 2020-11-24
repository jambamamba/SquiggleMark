#pragma once

#include <QObject>
class QDialog;
class ModalDialog : public QObject
{
    Q_OBJECT
public:
    explicit ModalDialog(QObject *parent = nullptr);

    QDialog *newModalDialog(QWidget *widget, const QString &title);

signals:

public slots:
};

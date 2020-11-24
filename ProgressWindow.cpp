#include "ProgressWindow.h"
#include "ui_ProgressWindow.h"

#include <QDesktopServices>
#include <QFile>
#include <QDir>
#include <QFileInfo>

ProgressWindow::ProgressWindow(QWidget *parent) :
    QDialog(parent, Qt::Dialog),
    ui(new Ui::ProgressWindow)
{
    ui->setupUi(this);
}

ProgressWindow::~ProgressWindow()
{
    delete ui;
}

void ProgressWindow::initProgressWindow(int maximumValue)
{
    ui->progressBar->reset();
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(maximumValue);
    ui->progressMessage->setText("");
    ui->cancelButton->setText("Cancel");
    m_dirs.clear();
}

void ProgressWindow::setProgressBarValue(int value)
{
    ui->progressBar->setValue(value);
}

int ProgressWindow::getProgressBarValue()
{
    return ui->progressBar->value();
}

int ProgressWindow::getProgressBarMaximum()
{
    return ui->progressBar->maximum();
}

void ProgressWindow::updateProgressMessage(const QString &message)
{
    QFile file(message);
    if(file.exists())
    {
        QFileInfo fileInfo(message);
        QString dirPath (fileInfo.absoluteDir().absolutePath());
        if(!m_dirs.contains(dirPath))
        {
            m_dirs.append(dirPath);
        }
    }

    ui->progressMessage->append(message);
}

void ProgressWindow::on_cancelButton_clicked()
{
    reject();
}

void ProgressWindow::on_openOutputFolderButton_clicked()
{
    foreach(const QString dir, m_dirs)
    {
        QDesktopServices::openUrl(QUrl(dir));
    }
}

void ProgressWindow::on_doneProcessingFiles()
{
    ui->cancelButton->setText("Ok");
}

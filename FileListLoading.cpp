#include "FileListLoading.h"
#include "ui_FileListLoading.h"

FileListLoading::FileListLoading(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileListLoading)
{
    ui->setupUi(this);
}

FileListLoading::~FileListLoading()
{
    delete ui;
}

void FileListLoading::on_cancelButton_clicked()
{
    emit rejected();
}

void FileListLoading::onScanned(QString dir)
{
    ui->labelFilePath->setText(dir);
}

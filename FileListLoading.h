#ifndef FILELISTLOADING_H
#define FILELISTLOADING_H

#include <QDialog>

namespace Ui {
class FileListLoading;
}

class FileListLoading : public QDialog
{
    Q_OBJECT

public:
    explicit FileListLoading(QWidget *parent = 0);
    ~FileListLoading();

public slots:
    void onScanned(QString dir);
private slots:
    void on_cancelButton_clicked();

private:
    Ui::FileListLoading *ui;
};

#endif // FILELISTLOADING_H

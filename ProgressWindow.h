#ifndef PROGRESS_H
#define PROGRESS_H

#include <QDialog>
#include <QList>

namespace Ui {
class ProgressWindow;
}

class ProgressWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressWindow(QWidget *parent = 0);
    ~ProgressWindow();

    void initProgressWindow(int maximumValue);
    void setProgressBarValue(int value);
    int getProgressBarValue();
    int getProgressBarMaximum();
    void updateProgressMessage(const QString &message);
private slots:
    void on_cancelButton_clicked();
    void on_openOutputFolderButton_clicked();
public slots:
    void on_doneProcessingFiles();

protected:

signals:

private:
    Ui::ProgressWindow *ui;
    QList<QString> m_dirs;
};

#endif // PROGRESS_H

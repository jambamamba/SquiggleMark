#ifndef FILESELECTIONWIDGET_H
#define FILESELECTIONWIDGET_H

#include <functional>
#include <QFileDialog>
#include <QWidget>

#include "SupportedFileTypes.h"

namespace Ui {
class FileSelectionWidget;
}

class FileListView;
class FileSelectionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileSelectionWidget(QWidget *parent = 0);
    ~FileSelectionWidget();

    FileListView *listView();
    void init(SupportedFileTypes *supportedFileTypes, std::function<void()> onFileListSelectionChanged);
    void updateListViewCheckboxWidgets();
    void updateCheckboxes();
    void updateNumOfFilesWidget();
    void resetNumOfFilesWidget();
    void allowDrops(bool allow);
    void allowOperations(bool);
    void setTitle(const QString &fileSelectionAction);
    bool shouldRecursivelyScanDir();
    void hideWidgets();
    void addItemsFrom(FileListView* listView);
    void enableSimpleMode();

private slots:
    void on_checkAllFiles_clicked();
    void on_toggleAllFiles_clicked();
    void on_listView_clicked(const QModelIndex &index);
    void on_removeCheckedFiles_clicked();
    void on_selectFilesToWatermarkButton_clicked();
    void on_selectDirToWatermarkButton_clicked();
    void on_selectFilesToWatermark(QFileDialog &dialog);

    void on_nextButton_clicked();

private:
    bool m_simpleMode;
    std::function<void()>m_onFileListSelectionChanged;
    SupportedFileTypes *m_supportedFileTypes;
    Ui::FileSelectionWidget *ui;
};

#endif // FILESELECTIONWIDGET_H

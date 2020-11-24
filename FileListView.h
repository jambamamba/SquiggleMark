#ifndef FILELISTVIEW_H
#define FILELISTVIEW_H

#include <functional>
#include <QListView>
#include <QFutureWatcher>
#include <QStandardItemModel>

#include "SupportedFileTypes.h"
#include "MainWindowInterface.h"

class QDir;
class ImageHelper;
class QXmlStreamReader;
class QXmlStreamWriter;
class FileListLoading;

class FileListView : public QListView
{
    Q_OBJECT
public:
    explicit FileListView(QWidget *parent = 0);
    ~FileListView();
    void init(
            ImageHelper *imageHelper,
            SupportedFileTypes *supportedFileTypes,
            std::function<void ()> onFileListUpdated,
            std::function<void ()> onSaveSettings,
            std::function<bool()> shouldRecursivelyScanDir,
            std::function<void(const QString&)>onFileListSelectionChanged);
    void saveSettings(QXmlStreamWriter &writer);
    void loadSettings(QXmlStreamReader &reader);
    void clear();
    const QStandardItemModel &getModel() const;
    QStringList getCheckedFiles() const;
    int getCheckedFileCount(int *numFrames = nullptr) const;
    bool anyCheckedFiles() const;
    int getFileCount() const;
    QString getAt(const QModelIndex &index);
    QModelIndex getModelIndex(int index) const;
    QModelIndex getCurrentIndex() const;
    const QModelIndex getFirstSelectedItem() const { return selectedIndexes().first(); }
    void addImageFileToModel(const QString &fileName, bool recursive);
    QModelIndexList selectedIndexes () const { return QListView::selectedIndexes (); }
    void checkAll(bool check);
    void toggleAll();
    bool getCheckAll() { return m_checkAll; }
    bool getToggleAll() { return m_toggleAll; }
    void deleteCheckedItems();


protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dropEvent(QDropEvent *e);
    void keyPressEvent ( QKeyEvent * event );
    bool eventFilter(QObject *Object, QEvent *event);
    void mousePressEvent(QMouseEvent *event);
signals:
    void signalProgress(int);
    void scanning(QString dir);

public slots:
    void currentChanged ( const QModelIndex & current, const QModelIndex & previous );
    void itemChanged(QStandardItem* item);
    void finishedAddingFiles();
    void stopLoadingFiles();
    void onContextMenu(const QPoint&);
    void on_contextMenuActionCopyPath();
    void on_contextMenuActionCopyFileName();

private:
    QStandardItemModel m_model;
    ImageHelper *m_imageHelper;
    bool m_checkAll;
    bool m_toggleAll;
    QString m_rightClickedFile;
    QFuture<void> m_future_scanning_dirs;
    QFutureWatcher<void> m_watcher_scanning_dirs;
    bool m_stop_loading;
    FileListLoading *m_loading_dlg;
    SupportedFileTypes *m_supportedFileTypes;
    std::function<void ()> m_onFileListUpdated;
    std::function<void ()> m_onSaveSettings;
    std::function<bool()> m_shouldRecursivelyScan;
    std::function<void(const QString&)>m_onFileListSelectionChanged;

    void deleteSelectedItems();
    void addImagesFromDirectory(const QDir &dir);
    void handleKeyPressEvent(QKeyEvent *e);
    void addItemToModel(QString file, bool checked);
    void scanDirectory(QString fileName, bool recursive);
    void showLoadingDialog();
    void setupLoadingDialog();
    int getNumVideoFrames(int row) const;
};

#endif // FILELISTVIEW_H

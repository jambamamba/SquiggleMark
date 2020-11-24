#include <QClipboard>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QUrl>
#include <QStandardItem>
#include <QIcon>
#include <QApplication>
#include <QDir>
#include <QMenu>
#include <QMimeData>
#include <QXmlStreamAttributes>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QtConcurrent/QtConcurrent>


#include "FileListView.h"
#include "FileListLoading.h"
#include "ImageHelper.h"
#include "VideoPlayer.h"
#include "SupportedFileTypes.h"

extern "C" void mylog(const char *fmt, ...);

namespace {
const int ThumbnailHeight = 60;
}

bool isImageFile(const QString &fileName);

FileListView::FileListView(QWidget *parent)
    : QListView(parent)
    , m_model(this)
    , m_checkAll(true)
    , m_toggleAll(false)
    , m_stop_loading(false)
    , m_loading_dlg(nullptr)
    , m_supportedFileTypes(nullptr)
    , m_onFileListUpdated(nullptr)
    , m_onSaveSettings(nullptr)
    , m_onFileListSelectionChanged([](const QString&)
        {
    qDebug() << "in";
    })
{
    setAcceptDrops(true);
    setModel(&m_model);
    connect(&m_model,
                SIGNAL(itemChanged(QStandardItem*)),
                this,
                SLOT(itemChanged(QStandardItem*)));
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,
            SIGNAL(customContextMenuRequested(const QPoint&)),
            this,
            SLOT(onContextMenu(const QPoint&)));
    connect(&m_watcher_scanning_dirs, SIGNAL(finished()),
            this, SLOT(finishedAddingFiles()), Qt::QueuedConnection);

    setupLoadingDialog();
}

FileListView::~FileListView()
{
}

void FileListView::init(
        ImageHelper *imageHelper,
        SupportedFileTypes *supportedFileTypes,
        std::function<void ()> onFileListUpdated,
        std::function<void ()> onSaveSettings,
        std::function<bool()> shouldRecursivelyScanDir,
        std::function<void(const QString&)>onFileListSelectionChanged)
{
    m_onFileListUpdated = onFileListUpdated;
    m_onSaveSettings = onSaveSettings;
    m_imageHelper = imageHelper;
    m_supportedFileTypes = supportedFileTypes;
    m_shouldRecursivelyScan =  shouldRecursivelyScanDir;
    m_onFileListSelectionChanged = onFileListSelectionChanged;
}

const QStandardItemModel &FileListView::getModel() const
{
    return m_model;
}

QStringList FileListView::getCheckedFiles() const
{
    QStringList files;
    for(int row = 0; row < m_model.rowCount(); row++)
    {
        if(m_model.item(row, 0)->checkState() == Qt::Checked)
        {
            QString fileName = m_model.data(
                        m_model.index(row, 0), Qt::DisplayRole).toString();
            files.append(fileName);
        }
    }
    return files;
}

int FileListView::getNumVideoFrames(int row) const
{
    int numFrames = 0;
    QString fileName = m_model.data(
                m_model.index(row, 0), Qt::DisplayRole).toString();
    if(m_imageHelper->isVideoFile(fileName))
    {
        VideoPlayer videoPlayer;
        videoPlayer.open(fileName.toUtf8().data());
        numFrames = videoPlayer.getNumFrames();
    }
    return numFrames;
}

int FileListView::getCheckedFileCount(int *numFrames) const
{
    int files = 0;
    for(int row = 0; row < m_model.rowCount(); row++)
    {
        if(m_model.item(row, 0)->checkState() == Qt::Checked)
        {
            if(numFrames)
            {
                *numFrames += getNumVideoFrames(row);
            }
            files++;
        }
    }
    return files;
}

bool FileListView::anyCheckedFiles() const
{
    for(int row = 0; row < m_model.rowCount(); row++)
    {
        if(m_model.item(row, 0)->checkState() == Qt::Checked)
        {
            return true;
        }
    }
    return false;
}

int FileListView::getFileCount() const
{
    return m_model.rowCount();
}

QString FileListView::getAt(const QModelIndex &index)
{
    return m_model.data(index).toString();
}

QModelIndex FileListView::getModelIndex(int index) const
{
    return m_model.index(index, 0);
}

QModelIndex FileListView::getCurrentIndex() const
{
    return selectedIndexes().size() > 0 ? selectedIndexes().first() : QModelIndex();
}

void FileListView::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void FileListView::keyPressEvent ( QKeyEvent * event )
{
    handleKeyPressEvent(event);
}

void FileListView::dragMoveEvent(QDragMoveEvent *e)
{

}

void FileListView::addImagesFromDirectory(const QDir &dir)
{
    QStringList fileTypes = m_supportedFileTypes->getList();
    QStringList files = dir.entryList(fileTypes,
                QDir::Files | QDir::NoSymLinks);
    foreach(const QString &file, files)
    {
        QString fileName(QDir::toNativeSeparators(
                             dir.absolutePath().
                             append("/").
                             append(file)));
        if(!m_model.findItems(fileName).size())
        {
            addItemToModel(fileName, true);
        }
    }
}

void FileListView::scanDirectory(QString fileName, bool recursive)
{
    if(m_stop_loading)
    {
        return;
    }
    emit scanning(fileName);

    QDir dir(fileName);
    if(dir.exists())
    {
        addImagesFromDirectory(dir);
    }

    if(recursive)
    {
        QStringList fileTypes;
        fileTypes.append("*");
        QStringList dirs = dir.entryList(fileTypes,
                    QDir::Dirs | QDir::NoSymLinks );
        foreach(const QString &subdir, dirs)
        {
            if(m_stop_loading)
            {
                break;
            }
            if(subdir=="." || subdir=="..")
            {
                continue;
            }
            QString fileName(QDir::toNativeSeparators(
                                 dir.absolutePath().
                                 append("/").
                                 append(subdir)));
            scanDirectory(fileName, recursive);
        }
    }
}

void FileListView::setupLoadingDialog()
{
    m_loading_dlg = new FileListLoading;
    m_loading_dlg->setModal(false);
    connect(m_loading_dlg, SIGNAL(rejected()), this, SLOT(stopLoadingFiles()));
    connect(this, &FileListView::scanning, m_loading_dlg, &FileListLoading::onScanned);
}

void FileListView::showLoadingDialog()
{
    if(!m_loading_dlg->isVisible())
    {
        m_loading_dlg->show();
        m_loading_dlg->raise();
        m_loading_dlg->activateWindow();
    }
}

void FileListView::addImageFileToModel(const QString &fileName, bool recursive)
{
    if(!m_model.findItems(fileName).size())
    {
        if(m_imageHelper->isImageFile(fileName) ||
                m_imageHelper->isVideoFile(fileName))
        {
            addItemToModel(fileName, true);
        }
        else
        {
            m_stop_loading = false;

            m_future_scanning_dirs = QtConcurrent::run(this, &FileListView::scanDirectory, fileName, recursive);
            m_watcher_scanning_dirs.setFuture(m_future_scanning_dirs);

            showLoadingDialog();
        }
        if(!m_onFileListUpdated) { mylog("m_onFileListUpdated"); }
        m_onFileListUpdated();
    }
}

void FileListView::stopLoadingFiles()
{
    m_stop_loading = true;
    m_loading_dlg->hide();
    finishedAddingFiles();
}

void FileListView::finishedAddingFiles()
{
    if(!m_onFileListUpdated) { mylog("m_onFileListUpdated"); }
    m_onFileListUpdated();
    if(m_loading_dlg->isVisible())
    {
        m_loading_dlg->hide();
    }
}

void FileListView::dropEvent(QDropEvent *e)
{
    if(!m_shouldRecursivelyScan) { mylog("m_shouldRecursivelyScan"); }
    bool recursive = m_shouldRecursivelyScan();
    foreach (const QUrl &url, e->mimeData()->urls())
    {
        const QString &fileName = url.toLocalFile();
        addImageFileToModel(QDir::toNativeSeparators(fileName), recursive);
    }
}

void FileListView::clear()
{
    if(!m_onFileListUpdated) { mylog("m_onFileListUpdated"); }
    m_model.clear();
    m_onFileListUpdated();
}

void FileListView::deleteSelectedItems()
{
    foreach(const QModelIndex &index, selectedIndexes())
    {
        m_model.removeRow(index.row());
    }
    if(!m_onFileListUpdated) { mylog("m_onFileListUpdated"); }
    m_onFileListUpdated();
}

void FileListView::deleteCheckedItems()
{
    for(int i = 0; i<m_model.rowCount(); )
    {
        QModelIndex si = m_model.index(i, 0);
        QStandardItem *iz = m_model.itemFromIndex(si);

        if(iz->checkState() == Qt::Checked)
        {
            m_model.removeRow(si.row());
        }
        else
        {
            i++;
        }
    }
    if(!m_onFileListUpdated) { mylog("m_onFileListUpdated"); }
    m_onFileListUpdated();
}

void FileListView::handleKeyPressEvent(QKeyEvent *e)
{
    if(Qt::Key_Delete == e->key() && QApplication::keyboardModifiers() && Qt::ControlModifier)
    {
        deleteCheckedItems();
    }
    else if(Qt::Key_Delete == e->key())
    {
        deleteSelectedItems();
    }
    else
    {
        int current_index = getCurrentIndex().row();

        if(Qt::Key_Up == e->key())
        {
            QModelIndex si = m_model.index(
                        (current_index > 0) ? current_index - 1 :
                                              m_model.rowCount() - 1, 0);
            setCurrentIndex(si);
        }
        else if(Qt::Key_Down == e->key())
        {
            QModelIndex si = m_model.index(
                        (current_index < m_model.rowCount() - 1) ?
                            current_index + 1 : 0, 0);
            setCurrentIndex(si);
        }
        else if(Qt::Key_PageUp == e->key())
        {
            QModelIndex si = m_model.index(
                        (current_index > 4) ? current_index - 4 : 0, 0);
            setCurrentIndex(si);
        }
        else if(Qt::Key_PageDown == e->key())
        {
            QModelIndex si = m_model.index(
                        (current_index < m_model.rowCount() - 5) ?
                            current_index + 4 : m_model.rowCount() - 1, 0);
            setCurrentIndex(si);
        }
    }
}

bool FileListView::eventFilter(QObject *Object, QEvent *Event)
{
  if (Event->type() == QEvent::KeyPress)
  {
    QKeyEvent *KeyEvent = (QKeyEvent*)Event;

    handleKeyPressEvent(KeyEvent);
  }
  return QListView::eventFilter(Object, Event);
}

void FileListView::saveSettings(QXmlStreamWriter &writer)
{
    writer.writeStartElement("FilesToWatermark");
    writer.writeAttribute("CheckAll", QString("%1").arg((int)m_checkAll));
    writer.writeAttribute("ToggleAll", QString("%1").arg((int)m_toggleAll));
    for (int e = 0; e < m_model.rowCount(); ++e)
    {
        QModelIndex si = m_model.index(e, 0);
        QStandardItem *iz = m_model.itemFromIndex(si);

        writer.writeStartElement("FileName");
        writer.writeAttribute("Checked", iz->checkState() == Qt::Checked ?
                                      QString("%1").arg(1) : QString("%1").arg(0));
            writer.writeCharacters(iz->text());
        writer.writeEndElement();
    }
    writer.writeEndElement();
}

void FileListView::loadSettings(QXmlStreamReader  &reader)
{
    if(reader.name() == "FilesToWatermark")
    {
        foreach(const QXmlStreamAttribute &attribute, reader.attributes())
        {
            if(attribute.name() == "CheckAll")
            {
                m_checkAll = attribute.value().toInt() == 0 ? false : true;
            }
            else if(attribute.name() == "ToggleAll")
            {
                m_toggleAll = attribute.value().toInt() == 0 ? false : true;
            }
        }
    }
    else if(reader.name() == "FileName")
    {
        QXmlStreamAttributes attributes = reader.attributes();
        QString name = reader.readElementText();
        QFile file_(name);
        if(name.size() && file_.exists() && !m_model.findItems(file_.fileName()).size())
        {
            bool checked = true;
            foreach(const QXmlStreamAttribute &attribute, attributes)
            {
                if(attribute.name() == "Checked")
                {
                    checked = attribute.value().toInt() == 0 ? false : true;
                    break;
                }
            }
            //TODO: should be done async with progress bar
            addItemToModel(QDir::toNativeSeparators(file_.fileName()), checked);
        }
    }
}

void FileListView::addItemToModel(QString file, bool checked)
{
    QStandardItem *item = new QStandardItem(file);
    item->setCheckable(true);
    item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
    m_model.appendRow(item);
}

void FileListView::checkAll(bool check)
{
    for (int e = 0; e < m_model.rowCount(); ++e)
    {
        QModelIndex si = m_model.index(e, 0);
        QStandardItem *iz = m_model.itemFromIndex(si);

        iz->setCheckState(check ? Qt::Checked : Qt::Unchecked);
    }
    m_checkAll = check;
    if(!m_onSaveSettings) { mylog("m_onSaveSettings"); }
    m_onSaveSettings();
}

void FileListView::toggleAll()
{
    for (int e = 0; e < m_model.rowCount(); ++e)
    {
        QModelIndex si = m_model.index(e, 0);
        QStandardItem *iz = m_model.itemFromIndex(si);

        iz->setCheckState(iz->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);
    }
    m_toggleAll ^= 1;
    if(!m_onSaveSettings) { mylog("m_onSaveSettings"); }
    m_onSaveSettings();
}

void FileListView::currentChanged ( const QModelIndex & current, const QModelIndex & previous )
{
    if(!m_onFileListSelectionChanged) { mylog("m_onFileListSelectionChanged"); }
    m_onFileListSelectionChanged(getAt(current));
}

void FileListView::itemChanged(QStandardItem* item)
{
    m_onFileListUpdated();
}

void FileListView::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        emit customContextMenuRequested(event->pos());
    }
    else
    {
        QListView::mousePressEvent(event);
    }
}

void FileListView::onContextMenu(const QPoint& pos)
{
    QModelIndex index = indexAt(mapTo(this, pos));
    if(index.isValid())
    {
        QMenu *menu = new QMenu(this);
        menu->addAction("Copy Path", this,
                        SLOT(on_contextMenuActionCopyPath()));
        menu->addAction("Copy File Name", this,
                        SLOT(on_contextMenuActionCopyFileName()));

        m_rightClickedFile = m_model.data(index, Qt::DisplayRole).toString();
        QAction* selectedItem = menu->exec( mapToGlobal(pos) );
        if (!selectedItem)
        {
            m_rightClickedFile.clear();
        }

        delete menu;
    }
}

void FileListView::on_contextMenuActionCopyPath()
{
    QFileInfo f(m_rightClickedFile);
    mylog("%s", f.absoluteFilePath().toUtf8().data());
    QApplication::clipboard()->setText ( f.absoluteFilePath() );
}

void FileListView::on_contextMenuActionCopyFileName()
{
    QFileInfo f(m_rightClickedFile);
    mylog("%s", f.fileName().toUtf8().data());
    QApplication::clipboard()->setText ( f.fileName() );
}

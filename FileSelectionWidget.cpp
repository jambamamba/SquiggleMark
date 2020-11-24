#include "FileSelectionWidget.h"
#include "SupportedFileTypes.h"
#include "ui_FileSelectionWidget.h"

#include "FileListView.h"

extern "C" void mylog(const char *fmt, ...);

FileSelectionWidget::FileSelectionWidget(QWidget *parent)
    : QWidget(parent)
    , m_simpleMode(false)
    , m_supportedFileTypes(nullptr)
    , m_onFileListSelectionChanged(nullptr)
    , ui(new Ui::FileSelectionWidget)
{
    ui->setupUi(this);
    ui->nextButton->hide();
}

FileSelectionWidget::~FileSelectionWidget()
{
    delete ui;
}

FileListView *FileSelectionWidget::listView()
{
    return ui->listView;
}

void FileSelectionWidget::enableSimpleMode()
{
    m_simpleMode = true;
}

void FileSelectionWidget::updateListViewCheckboxWidgets()
{
    int numFiles = ui->listView->getFileCount();
    if(numFiles > 0)
    {
        if(!m_simpleMode)
        {
            ui->toggleAllFiles->show();
            ui->checkAllFiles->show();
            ui->removeCheckedFiles->show();
        }
        ui->clearFileListButton->show();
    }
    else
    {
        ui->toggleAllFiles->hide();
        ui->checkAllFiles->hide();
        ui->removeCheckedFiles->hide();
        ui->clearFileListButton->hide();
    }
}

void FileSelectionWidget::updateCheckboxes()
{
    ui->checkAllFiles->setChecked(ui->listView->getCheckAll());
    ui->toggleAllFiles->setChecked(ui->listView->getToggleAll());
}

void FileSelectionWidget::allowDrops(bool allow)
{
    ui->listView->setAcceptDrops(allow);
}

void FileSelectionWidget::allowOperations(bool allow)
{
    ui->clearFileListButton->setEnabled(allow);
    ui->removeCheckedFiles->setEnabled(allow);
}

void FileSelectionWidget::updateNumOfFilesWidget()
{
    int numFiles = ui->listView->getFileCount();
    int numCheckedFiles = ui->listView->getCheckedFileCount();
    ui->numberOfFiles->setText(QString("%1/%2 files").arg(numCheckedFiles).arg(numFiles));
    ui->nextButton->setEnabled(true);
}

void FileSelectionWidget::resetNumOfFilesWidget()
{
    ui->numberOfFiles->setText("");
    ui->nextButton->setEnabled(false);
}

void FileSelectionWidget::setTitle(const QString &fileSelectionAction)
{
    ui->fileSelectionGroupBox->setTitle(fileSelectionAction);
}

void FileSelectionWidget::on_checkAllFiles_clicked()
{
    ui->listView->checkAll( ui->checkAllFiles->isChecked() );
}

void FileSelectionWidget::on_toggleAllFiles_clicked()
{
    ui->listView->toggleAll();
}

void FileSelectionWidget::init(
        SupportedFileTypes *supportedFileTypes,
        std::function<void()> onFileListSelectionChanged)
{
    m_supportedFileTypes = supportedFileTypes;
    m_onFileListSelectionChanged = onFileListSelectionChanged;
}

void FileSelectionWidget::on_listView_clicked(const QModelIndex &index)
{
    if(!m_onFileListSelectionChanged) { mylog("m_onFileListSelectionChanged"); }
    m_onFileListSelectionChanged();
}

void FileSelectionWidget::on_removeCheckedFiles_clicked()
{
    ui->listView->deleteCheckedItems();
}

bool FileSelectionWidget::shouldRecursivelyScanDir()
{
    return ui->checkBoxRecursive->checkState() ==  Qt::Checked;
}

void FileSelectionWidget::addItemsFrom(FileListView* listView)
{
    for(int i=0; i < listView->model()->rowCount(); ++i)
    {
        QModelIndex index = listView->model()->index(i, 0);
        QString file = listView->model()->data(index).toString();
        QStandardItem *item = new QStandardItem(file);
        item->setCheckable(true);
        QStandardItem *iz = ((QStandardItemModel *)listView->model())->itemFromIndex(index);
        item->setCheckState(iz->checkState());
        ((QStandardItemModel *)ui->listView->model())->appendRow(item);
    }
}

void FileSelectionWidget::hideWidgets()
{
    ui->checkBoxRecursive->hide();
    ui->checkAllFiles->hide();
    ui->toggleAllFiles->hide();
    ui->removeCheckedFiles->hide();
    ui->clearFileListButton->hide();

    ui->nextButton->show();
    ui->nextButton->setEnabled(false);
}

void FileSelectionWidget::on_selectFilesToWatermark(QFileDialog &dialog)
{
    QString filetypes = m_supportedFileTypes->getString();
    dialog.setNameFilter(tr(QString("Images (%1);;Any (*.*)").arg(filetypes).toUtf8().data()));
    dialog.setViewMode(QFileDialog::Detail);

    QStringList fileNames;
    if (dialog.exec())
    {
        fileNames = dialog.selectedFiles();
    }

    foreach(QString fileName, fileNames)
    {
        ui->listView->addImageFileToModel(
                    QDir::toNativeSeparators(fileName), shouldRecursivelyScanDir());
    }
}

void FileSelectionWidget::on_selectFilesToWatermarkButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    //dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    on_selectFilesToWatermark(dialog);
}

void FileSelectionWidget::on_selectDirToWatermarkButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    //dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    on_selectFilesToWatermark(dialog);
}


void FileSelectionWidget::on_nextButton_clicked()
{
    ((QDialog*)parent())->close();
}

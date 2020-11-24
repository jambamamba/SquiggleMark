#include <gd.h>

#include <QCloseEvent>
#include <QCoreApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QScrollBar>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QMutexLocker>
#include <QProgressBar>
#include <QSplitter>
#include <QtConcurrent/QtConcurrent>
#include <QTimer>
#include <QTreeWidget>
#include <QTreeView>

#include "ControlsAreaWidget.h"
#include "FileListView.h"
#include "FileSelectionWidget.h"
#include "FontHelper.h"
#include "ImageHelper.h"
#include "LicenseWidget.h"
#include "MainSplitter.h"
#include "MainWindow.h"
#include "PreviewArea.h"
#include "SupportedFileTypes.h"
#include "PreviewAreaWidget.h"
#include "UpdaterWidget.h"
#include "WaterMark.h"
#include "ModalDialog.h"

#include "ui_MainWindow.h"

#define DEFINE_SPRITEMILL false

using namespace std;

static const char LOG_PATH[] = "log.txt";
extern "C" void mylog(const char *fmt, ...);

namespace
{
static const QString APP_NAME("SquiggleMark");
static const int APP_ID = 2184;//2185 for SpriteMill
static const int APP_VERSION = 270;


QString getDataDirectory()
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    QString path = QString(dataDir).append("/").append(APP_NAME).append("/").append(APP_NAME).append(".xml");
    QFile file(path);
    if(!file.exists())
    {
        QDir().mkpath(dataDir.append("/").append(APP_NAME));
    }
    return path;
}

QString getLicenseWidgetLabel()
{
    return QString("You are using the demo version.\n\nThe processed files will have the %1 logo on them. \nThe purchased version does not add the %1 logo.\n\nIf you like %1, please support us by purchasing a license at:").
        arg(DEFINE_SPRITEMILL ? "SpriteMill" : "SquiggleMark");
}

}//namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_stop(false)
    , m_watermark(new Watermark)
    , m_imageHelper(new ImageHelper(APP_NAME, &m_supportedFileTypes))
    , m_fontHelper(new FontHelper)
    , m_updater_widget(new UpdaterWidget(APP_NAME, APP_ID, APP_VERSION, this))
    , m_guid(QUuid::createUuid())
    , m_controlsAreaWidget(new ControlsAreaWidget(this))
    , m_previewArea(new PreviewArea(this))
    , m_previewAreaWidget(new PreviewAreaWidget(this))
    , m_splitter(nullptr)
    , m_dialog(new ModalDialog(this))
{
    ui->setupUi(this);

    connect(this, SIGNAL(signalAppendLogMessage(const QString*)),
                           this, SLOT(onAppendLogMessage(const QString*)), Qt::QueuedConnection);

    loadFonts();
    loadSettings();
    initializeLicenseWidget();
    setupApplicationType();
    loadSplitterView();
    m_updater_widget->checkForNewVersion(m_guid);

    connect(this, SIGNAL(signalProgress(const QString)),
             this, SLOT( updateProgress(const QString)), Qt::QueuedConnection);
    emit signalProgress("");

    QMetaObject::invokeMethod(m_controlsAreaWidget, "onUpdateWatermarkingWidgets");

    ui->statusBar->setMaximumHeight(0);
    resetProgressBar();

    adjustSize();

}

MainWindow::~MainWindow()
{
    onSaveSettings();
    delete ui;
}

LicenseWidget *MainWindow::newLicenseWidget()
{
    auto widget = new LicenseWidget(APP_NAME, APP_ID, APP_VERSION, m_guid,
                                         QString("http://www.osletek.com/?a=order&&fileid=%1&&guid=%2")
                                         .arg(APP_ID).arg(m_guid.toString()),
                                         this);
    widget->checkLicense(getLicenseWidgetLabel(),
                                   [this](int licenseType){
        m_licenseType = (LicenseType)licenseType;
    });
    return widget;
}

void MainWindow::initializeLicenseWidget()
{
    m_license_widget = newLicenseWidget();
    m_controlsAreaWidget->setupLicenseWidget(m_license_widget);
}

void MainWindow::resetProgressBar()
{
    ui->progressBar->reset();
    ui->progressBar->setMinimum(0);
}

void MainWindow::endProgress()
{
    m_numFilesToProcess = 0;
    m_numFilesProcessed = 0;
}

void MainWindow::updateProgress(int increment)
{
    m_numFilesProcessed += increment;
    int progress = m_numFilesToProcess > 0 ? (m_numFilesProcessed*100)/m_numFilesToProcess : 100;
    mylog("progress %i%%: %i of %i", progress, m_numFilesProcessed, m_numFilesToProcess);
    ui->progressBar->setValue(progress);
}

void MainWindow::updateProgress(const QString savedFile)
{
    bool done = ui->progressBar->value() >= ui->progressBar->maximum() ||
            savedFile.size() == 0;
    qDebug() << "done " << done << ui->progressBar->value() << savedFile.size();
    if(done)
    {
        m_controlsAreaWidget->updateWidgetsAfterProcessingFiles();
        ui->progressBar->setValue(ui->progressBar->maximum());
        m_stop = false;
    }
    if(savedFile.size())
    {
        updateProgress(1);
        QFile file(savedFile);
        if(file.exists())
        {
            QFileInfo fileInfo(savedFile);
            QString dirPath(fileInfo.absoluteDir().absolutePath());
            m_controlsAreaWidget->addToProcessedDirectories(dirPath);
            m_controlsAreaWidget->enableViewFilesButton(savedFile);
        }
        else
        {
            m_controlsAreaWidget->enableViewFilesButton(QString());
        }
    }
}


void MainWindow::resetProgress()
{
    emit signalProgress("");
}

void MainWindow::initProgressBar(int numFilesToProcess)
{
    m_stop = false;
    m_numFilesProcessed = 0;
    m_numFilesToProcess = numFilesToProcess;

    ui->progressBar->setMaximum( 100 );
    ui->progressBar->setValue(0);
}

void MainWindow::stop()
{
    m_stop = true;
    m_controlsAreaWidget->stopRendering();
    m_previewArea->stopMovie();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(m_controlsAreaWidget->isRendering() || m_previewArea->isPlayingMovie())
    {
        event->ignore();
        stop();
        QTimer::singleShot (500, this, SLOT(close()));
    }
    else
    {
        event->accept();
    }
}

bool MainWindow::close()
{
    if(m_controlsAreaWidget->isRendering() || m_previewArea->isPlayingMovie())
    {
        stop();
        QTimer::singleShot (500, this, SLOT(close()));
        return false;
    }
    else
    {
        return QMainWindow::close();
    }
}

void MainWindow::onFileListUpdated()
{
    QMetaObject::invokeMethod(m_controlsAreaWidget, "onUpdateWatermarkingWidgets");
    m_controlsAreaWidget->updateFileSelectionWidgets();
    m_controlsAreaWidget->selectFileIfNoneSelected();
    onSaveSettings();
    resetProgressBar();
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    updatePreview();
}

void MainWindow::onBgImapeSelected()
{
    emit signalBgImageSelected(m_previewArea->getPreviewImageSize(),
                         m_previewArea->getOriginalImageSize());
}

void MainWindow::setImageFileToPreview(QString imageFile)
{
    m_mutex.lock();
    m_imageFileToPreview = imageFile;
    m_mutex.unlock();
}

QString MainWindow::getImageFileToPreview()
{
    QString imageFile;
    m_mutex.lock();
    imageFile = m_imageFileToPreview;
    m_imageFileToPreview.clear();
    m_mutex.unlock();
    return imageFile;
}

void MainWindow::loadSettings()
{
    QFile file(getDataDirectory());
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        mylog("could not open file %s for loading settings", file.fileName().toUtf8().data());
        return;
    }
    QXmlStreamReader reader(&file);

    while (!reader.atEnd())
    {
        reader.readNext();
        if(reader.isStartElement())
        {
            m_watermark->loadSettings(reader);
            m_controlsAreaWidget->loadSettings(reader);
            m_previewAreaWidget->loadSettings(reader);
            if(reader.name() == "GUID")
            {
                m_guid = QUuid(reader.readElementText());
            }
            else if(reader.name() == "AdditionalFontsDirectory")
            {
                m_additionalFontsDir = reader.readElementText();
            }
            else if(reader.name() == "FontColor")
            {
                m_fontHelper->setFontColor(reader.readElementText().toInt());
            }
            else if(reader.name() == "AutomaticCheckForUpdates")
            {
                m_updater_widget->setCheckboxForCheckingUpdates(
                            reader.readElementText().toInt());
            }
            else if(reader.name() == "Font")
            {
                foreach(const QXmlStreamAttribute &attribute, reader.attributes())
                {
                    if(attribute.name() == "PointSize")
                    {
                        m_fontHelper->setFontSizeInPoints(attribute.value().toDouble());
                    }
                    else if(attribute.name() == "Family")
                    {
                        m_fontHelper->setFontFamily(attribute.value().toString());
                    }
                    else if(attribute.name() == "Style")
                    {
                        m_fontHelper->setFontStyle((FONTHELPER_STYLE_FLAG) attribute.value().toInt());
                    }
                    else if(attribute.name() == "Decoration")
                    {
                        m_fontHelper->setUnderlined(attribute.value().toInt() == 1);
                    }
                }
            }
        }
    }
    file.close();
}

void MainWindow::onSaveSettings()
{
    QFile file(getDataDirectory());
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        mylog("could not open file %s for saving settings", file.fileName().toUtf8().data());
        return;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("SquiggleMarkSettings");

    writer.writeTextElement("GUID", m_guid.toString());
    writer.writeTextElement("AdditionalFontsDirectory", m_additionalFontsDir);
    writer.writeTextElement("FontColor", QString("%1").arg(m_fontHelper->getFontColor()));
    writer.writeTextElement("AutomaticCheckForUpdates",
                            QString("%1").arg(m_updater_widget->getCheckboxForCheckingUpdates()));
    m_controlsAreaWidget->saveSettings(writer);
    m_previewAreaWidget->saveSettings(writer);

    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();
}

void MainWindow::onFileListSelectionChanged(const QString &imageFile)
{
    if(imageFile.size())
    {
       setImageFileToPreview(imageFile);

        if(!m_futureFileSelecetionChanged.isRunning())
        {
            m_futureFileSelecetionChanged =
                    QtConcurrent::run(m_previewArea, &PreviewArea::setPixmapSrc);
        }
    }
}

//void MainWindow::updateResizingForiOSWidgets()
//{
//    ui->generateiOSImagesPushButton->setEnabled( ui->iosDeviceImages->getCheckedDevicesCount() > 0 );
//    ui->generateiOSIconsPushButton->setEnabled( ui->iosDeviceIcons->getCheckedDevicesCount() > 0 );
//    ui->generateiTunesScreenShotsPushButton->setEnabled( ui->iTunesScreenShots->getCheckedDevicesCount() > 0 );
//    onSaveSettings();
//}

void MainWindow::setupApplicationType()
{
    if(DEFINE_SPRITEMILL)
    {
        m_controlsAreaWidget->disableWatermarkingFeature();
    }
    else
    {
        QString title = QString("SquiggleMark %1 - A watermarking application").
                arg(QString::number(APP_VERSION/100.0f, '0', 2));
        setWindowTitle(title);
        m_controlsAreaWidget->enableWatermarkingFeature(title);
    }
}

void MainWindow::onLicenseValidated(int licenseType)
{
    m_licenseType = (LicenseType)licenseType;
    if(m_licenseType == LicenseType_SquiggleMarkPhoto)
    {
        m_supportedFileTypes.dontSupportVideo();
    }
}

void MainWindow::writemylog(const QString *msg)
{

    m_controlsAreaWidget->logMessage(msg);

    delete msg;
}

void MainWindow::clearmylog()
{
    QFile logFile(LOG_PATH);
    logFile.remove();
}

void MainWindow::onAppendLogMessage(const QString *msg)
{
    writemylog(msg);
}

void MainWindow::appendLogMessage(const QString *msg)
{
    QString path(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    path = path.append(QDir::separator()).append(APP_NAME);
    if(!QFile(path).exists())
    {
        QDir().mkpath(path);
    }
    path.append(QDir::separator()).append(LOG_PATH);
    FILE* fp=fopen(path.toUtf8().data(), "a+t");
    if(fp)
    {
        fwrite(msg->toUtf8().data(), msg->count(), 1, fp);
        fclose(fp);
    }
    emit signalAppendLogMessage(msg);
}

void MainWindow::onShowWatermarkingWidgets()
{
    m_previewAreaWidget->showWatermarkingWidgets();
    ui->progressBar->show();
}

void MainWindow::loadSplitterView()
{
    m_watermark = createNewLayer(m_previewArea);
    m_controlsAreaWidget->init(
                DEFINE_SPRITEMILL,
                m_watermark,
                m_previewArea,
                m_imageHelper,
                &m_supportedFileTypes,
                m_fontHelper,
                m_updater_widget,
                [this](){return m_stop;},
                [this](){return m_previewArea->getOriginalImageSize();},
                [this](){onShowWatermarkingWidgets();},
                [this](){return m_licenseType; },
                [this](const QString &imageFile){onFileListSelectionChanged(imageFile);},
                [this](){onFileListUpdated();},
                [this](){onSaveSettings();}
    );
    m_previewAreaWidget->init(
                    m_watermark,
                    m_previewArea,
                    m_imageHelper,
                    [this](){return getImageFileToPreview();},
                    [this](){onBgImapeSelected();},
                    [this](){updatePreview();}
    );

    setupSplitter();

    connect(m_controlsAreaWidget, &ControlsAreaWidget::updatePreview,
           this, &MainWindow::updatePreview);
    connect(m_controlsAreaWidget, &ControlsAreaWidget::initProgressBar,
           this, &MainWindow::initProgressBar);
    connect(m_controlsAreaWidget, &ControlsAreaWidget::resetProgress,
           this, &MainWindow::resetProgress);
    connect(m_controlsAreaWidget, SIGNAL(signalProgress(int)),
           this, SLOT(updateProgress(int)), Qt::QueuedConnection);
    connect(m_controlsAreaWidget, SIGNAL(signalEndProgress()),
            this, SLOT(endProgress()), Qt::QueuedConnection);
    connect(m_controlsAreaWidget, SIGNAL(showProgress(QString)),
           this, SLOT(updateProgress(QString)), Qt::QueuedConnection);
    connect(m_controlsAreaWidget, SIGNAL(clearmylog()),
           this, SLOT(clearmylog()));
    connect(m_controlsAreaWidget, SIGNAL(signalShowLicenseDialog()),
           this, SLOT(showLicenseDialog()));
    connect(m_controlsAreaWidget, SIGNAL(updatePreviewImageDimensions()),
            m_previewAreaWidget, SLOT(updatePreviewImageDimensions()));
    connect(m_controlsAreaWidget, SIGNAL(scrollToPreview(WaterPlacement)),
            m_previewAreaWidget, SLOT(scrollToPreview(WaterPlacement)));
    connect(this, SIGNAL(signalBgImageSelected(const QSize&, const QSize&)),
            m_controlsAreaWidget, SLOT(onBgImageSelected(const QSize&, const QSize&)));
}

void MainWindow::setupSplitter()
{
    m_splitter = new MainSplitter(Qt::Horizontal, this);
    ui->splitterHorizontalLayout->addWidget(m_splitter);
    m_splitter->addWidget(m_previewAreaWidget);
    m_splitter->addWidget(m_controlsAreaWidget);
    m_splitter->show();

    connect(m_splitter, &MainSplitter::splitterMoved, this,
            &MainWindow::onSplitterMoved);

    int width = rect().width();
    QList<int> sizes;
    sizes << width*.9f << width *.1f;
    m_splitter->setSizes(sizes);
}

void MainWindow::onSplitterMoved(int pos, int index)
{
//    mylog("splitter moved %i, %i", pos, index);
    updatePreview();
}

void MainWindow::showLicenseDialog()
{
    delete m_dialog->newModalDialog(newLicenseWidget(), "Demo");
}

Watermark *MainWindow::createNewLayer(PreviewArea *previewArea)
{
    m_watermark->init(m_imageHelper, m_fontHelper, [this](){
        m_controlsAreaWidget->onWatermarkSelected();
    });
    m_watermark->setParent(previewArea);
    m_watermark->setWatermarkPlacement(
                m_controlsAreaWidget->getWatermarkPlacement());

    return m_watermark;
}

void MainWindow::updatePreview()
{
    ImageScaling scaleForUser = m_controlsAreaWidget->getImageScaling();
    bool scaleForPreview = m_previewAreaWidget->adjustToFitPreview();
    QSize viewPortSize = m_previewAreaWidget->maximumViewportSize();
    m_watermark->setScaleToFit(scaleForPreview || scaleForUser);
    QSize outputImageSize = m_controlsAreaWidget->getSizeSetByUser(QSize());
    m_previewArea->drawImage(
                outputImageSize,
                scaleForPreview ? viewPortSize : QSize(),
                m_controlsAreaWidget->rotateImageValue()
    );
}

bool MainWindow::loadFonts()
{
    if(!m_fontHelper->loadFonts(m_additionalFontsDir))
    {
        QString msg("Could not find fonts. You can specify your fonts folder in the SquiggleMark.xml file and restart the program");
        mylog(msg.toUtf8().data());
        QMessageBox::information(this, "No Fonts!", msg);
        return false;
    }
    return true;
}

#include <QColorDialog>
#include <QDesktopServices>
#include <QtConcurrent/QtConcurrent>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "ControlsAreaWidget.h"
#include "ui_ControlsAreaWidget.h"

#include "FileListView.h"
#include "FileSelectionWidget.h"
#include "FontHelper.h"
#include "ImageHelper.h"
#include "LicenseWidget.h"
#include "PreviewArea.h"
#include "QualitySlider.h"
#include "ReferenceImageSizeHelp.h"
#include "UpdaterWidget.h"
#include "WaterMark.h"
#include "ModalDialog.h"

extern "C" void mylog(const char *fmt, ...);

ControlsAreaWidget::ControlsAreaWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ControlsAreaWidget)
    , m_autoScroll(false)
    , m_imageHelper(nullptr)
    , m_fontHelper(nullptr)
    , m_isRendering(false)
    , m_firstTimeUserFileSelectionWidget(nullptr)
    , m_stop(nullptr)
    , m_getPreviewedImageSize(nullptr)
    , m_onShowWatermarkingWidgets(nullptr)
    , m_onFileListSelectionChanged(nullptr)
    , m_onFileListUpdated(nullptr)
    , m_onSaveSettings(nullptr)
    , m_qualitySliderJpg(new QualitySlider("JPG", 0, 100, 90, "Poor quality\nSmaller file", "Best quality\nLarger file", this))
    , m_qualitySliderPng(new QualitySlider("PNG", 0, 9, 7, "Fastest\nLarger file", "Slowest\nSmaller file", this))
    , m_qualitySliderVideo(new QualitySlider("Video", 1, 31, 7, "Best quality\nLarger file", "Poor quality\nSmaller file", this))
    , m_trialimg(nullptr)
    , m_watermark(nullptr)
    , m_dialog(new ModalDialog(this))
{
    ui->setupUi(this);
}

ControlsAreaWidget::~ControlsAreaWidget()
{
    delete ui;
}

void ControlsAreaWidget::init(
        bool spritemill,
        Watermark* watermark,
        PreviewArea* previewArea,
        ImageHelper *imageHelper,
        SupportedFileTypes *supportedFileTypes,
        FontHelper *fontHelper,
        UpdaterWidget *updaterWidget,
        std::function<bool()>stop,
        std::function<QSize()>getPreviewedImageSize,
        std::function<void()>onShowWatermarkingWidgets,
        std::function<LicenseType()>getLicenceType,
        std::function<void(const QString &imageFile)>onFileListSelectionChanged,
        std::function<void()>onFileListUpdated,
        std::function<void()>onSaveSettings
        )
{
    m_stop = stop;

    m_watermark = watermark;
    m_autoScroll = ui->autoScrollCheckBox->isChecked();
    m_imageHelper = imageHelper;
    m_imageHelper->setQualityGetters(
                [this]()->int { return jpegQuality(); },
                [this]()->int { return pngQuality(); },
                [this]()->int { return videoQuality(); }
    );
    m_supportedFileTypes = supportedFileTypes;
    m_fontHelper = fontHelper;
    m_getPreviewedImageSize = getPreviewedImageSize;
    m_onShowWatermarkingWidgets = onShowWatermarkingWidgets;
    m_onFileListSelectionChanged = onFileListSelectionChanged;
    m_onFileListUpdated = onFileListUpdated;
    m_onSaveSettings = onSaveSettings;
    m_getLicenceType = getLicenceType;
    connect(ui->rotateWatermark, SIGNAL(signalRotate(int)), this, SLOT(on_rotateWatermark(int)));
    connect(ui->rotateImage, SIGNAL(signalRotate(int)), this, SLOT(on_rotateImage(int)));
    connect(ui->tabWidgetFunctions, SIGNAL(currentChanged(int)), this, SLOT(tabSelected()));
    connect(ui->fileSelectionWidget->listView(), &FileListView::signalProgress,
            this, &ControlsAreaWidget::signalProgress, Qt::QueuedConnection);
    connect(m_watermark, SIGNAL(cropRectUpdated(const QRect&)),
            this, SLOT(on_cropRectUpdated(const QRect&)));
    connect(previewArea, SIGNAL(signalLoadedImageToPreview()), this,
             SLOT( updateWatermark()));
    connect(this, SIGNAL(signalProgressMessage(const QString*)),
             this, SLOT( on_appendProgressMessage(const QString*)), Qt::QueuedConnection);

    ui->verticalLayoutForUpdater->addWidget(updaterWidget);
    setupQualityWidgets();
    loadTrialImage(spritemill);
    updateFontWidgetsGroupBoxes();
    updateResizingWidgets();
    initFileSelectionWidget(ui->fileSelectionWidget,
                            m_onFileListSelectionChanged,
                            m_onFileListUpdated,
                            m_onSaveSettings);
    updateWatermarkText();
    updateFileListWidgets();
    m_watermark->loadWatermarkDefault();
    ui->fileSelectionWidget->updateListViewCheckboxWidgets();

    updateFontWidgets();
    updateFontStyle();
    updateWatermarkRotationWidget();
    updateAboutPageWidgets();
    selectFileIfNoneSelected();
    loadLicenseTerms();

    if(m_getLicenceType() == LicenseType_SquiggleMarkPhoto ||
            m_getLicenceType() == LicenseType_SpriteMill)
    {
        ui->groupBox_OutputVideoFormat->hide();
    }

//    resize(0, 0);
//    QApplication::processEvents();
}

void ControlsAreaWidget::updateWidgetsAfterProcessingFiles()
{
    ui->applyWatermarkButton->setText("Apply");
    ui->generateiOSImagesPushButton->setText("Generate");
    ui->generateiOSIconsPushButton->setText("Generate");
    ui->generateiTunesScreenShotsPushButton->setText("Generate");
    ui->iosDeviceIcons->enableListItems(true);
    ui->iTunesScreenShots->enableListItems(true);
    m_watermark->setAcceptDrops(true);
    ui->fileSelectionWidget->allowDrops(true);
    ui->fileSelectionWidget->allowOperations(true);
    m_isRendering = false;
}

void ControlsAreaWidget::on_appendProgressMessage(const QString* msg)
{
    ui->progressMessage->append(*msg);
    delete msg;
}

void ControlsAreaWidget::on_autoScrollCheckBox_clicked()
{
    m_autoScroll = ui->autoScrollCheckBox->isChecked();
}

void ControlsAreaWidget::on_radioButtonTopLeft_clicked()
{
    updateWatermark();
}

void ControlsAreaWidget::on_radioButtonTopRight_clicked()
{
    updateWatermark();
}

void ControlsAreaWidget::on_radioButtonBottomLeft_clicked()
{
    updateWatermark();
}

void ControlsAreaWidget::on_radioButtonBottomRight_clicked()
{
    updateWatermark();
}

void ControlsAreaWidget::on_radioButtonCentered_clicked()
{
    updateWatermark();
}

void ControlsAreaWidget::on_radioButtonCenterLeft_clicked()
{
    updateWatermark();
}

void ControlsAreaWidget::on_radioButtonCenterTop_clicked()
{
    updateWatermark();
}

void ControlsAreaWidget::on_radioButtonCenterBottom_clicked()
{
    updateWatermark();
}

void ControlsAreaWidget::on_radioButtonCenterRight_clicked()
{
    updateWatermark();
}

void ControlsAreaWidget::on_rotateImage(int)
{
     emit updatePreview(getImageScaling());
}

void ControlsAreaWidget::on_reloadWatermarkFile_clicked()
{
    if(ui->watermarkFile->text().size())
    {
        setWatermarkImage(ui->watermarkFile->text());
        onWatermarkSelected();
    }
}

void ControlsAreaWidget::on_destinationFolderPushButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setNameFilter(tr("Any (*.*)"));
    dialog.setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if (dialog.exec())
    {
        fileNames = dialog.selectedFiles();
    }
    foreach(QString fileName, fileNames)
    {
        ui->destinationFolderLineEdit->
                setText(QDir::toNativeSeparators(fileName));
        break;
    }
}

void ControlsAreaWidget::on_watermarkFileDialogButton_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    //dialog.setOption(QFileDialog::DontUseNativeDialog, true);
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
        setWatermarkImage(fileName);
        break;
    }
}

void ControlsAreaWidget::on_transparencySlider_valueChanged(int value)
{
    ui->transparencyLevel->setText( QString("%1%").arg(value));
    m_watermark->setOpactiyPercent(value);
    if(!m_onSaveSettings) { mylog("m_onSaveSettings"); }
    m_onSaveSettings();
}

void ControlsAreaWidget::on_watermarkText_textChanged()
{
    //changeToLayer2();
    updateFontWidgetsGroupBoxes();
    m_watermark->onTypedWatermark( ui->watermarkText->toPlainText() );
}

void ControlsAreaWidget::on_increaseFontSizeButton_clicked()
{
    if(m_fontHelper->incrementFontSize())
    {
        m_watermark->drawWatermarkText();
        ui->fontSizePointsSpinBox->setValue(m_fontHelper->getFontSizeInPoints());
    }
}

void ControlsAreaWidget::on_reduceFontSizeButton_clicked()
{
    if(m_fontHelper->decrementFontSize())
    {
        m_watermark->drawWatermarkText();
        ui->fontSizePointsSpinBox->setValue(m_fontHelper->getFontSizeInPoints());
    }
}

void ControlsAreaWidget::on_fontSizePointsSpinBox_valueChanged(int arg1)
{
    if(m_watermark->getWatermarkText().size() > 0 && m_fontHelper->setFontSizeInPoints(arg1))
    {
        m_watermark->drawWatermarkText();
    }
}

void ControlsAreaWidget::on_colorPickerToolButton_clicked()
{
    QColorDialog dlg;
    connect(&dlg, SIGNAL(currentColorChanged(QColor)), this, SLOT(colorPickerCurrentColorChanged(QColor)));
    QRgb color = m_fontHelper->getFontColor();
    dlg.setCurrentColor(color);
    dlg.exec();
    if(QDialog::Accepted == dlg.result())
    {
        colorPickerCurrentColorChanged(dlg.currentColor());
    }
    else
    {
        colorPickerCurrentColorChanged(QColor(color));
    }
}


void ControlsAreaWidget::colorPickerCurrentColorChanged(QColor color)
{
    m_fontHelper->setFontColor(color.rgb());
    m_watermark->drawWatermarkText();
}

void ControlsAreaWidget::on_boldFontTypeButton_clicked()
{
    ui->boldFontTypeButton->isChecked() ?
        m_fontHelper->addFontStyle(FONTHELPER_STYLE_FLAG_BOLD) :
        m_fontHelper->removeFontStyle(FONTHELPER_STYLE_FLAG_BOLD);
    m_watermark->drawWatermarkText();
}

void ControlsAreaWidget::on_italicsFontTypeButton_clicked()
{
    ui->italicsFontTypeButton->isChecked() ?
        m_fontHelper->addFontStyle(FONTHELPER_STYLE_FLAG_ITALIC) :
        m_fontHelper->removeFontStyle(FONTHELPER_STYLE_FLAG_ITALIC);
    m_watermark->drawWatermarkText();
}

void ControlsAreaWidget::on_underlineFontTypeButton_clicked()
{
    m_fontHelper->setUnderlined(ui->underlineFontTypeButton->isChecked());
    m_watermark->drawWatermarkText();
}

void ControlsAreaWidget::on_gotoSquiggleMarkPushButton_clicked()
{
    QDesktopServices::openUrl(QUrl(ui->gotoSquiggleMarkPushButton->text()));
}

void ControlsAreaWidget::on_gotoSquiggleMarkPushButtonWithImage_clicked()
{
    on_gotoSquiggleMarkPushButton_clicked();
}

void ControlsAreaWidget::on_gotoLeLesBackyardPushButton_clicked()
{
    QDesktopServices::openUrl(QUrl(ui->gotoLeLesBackyardPushButton->text()));
}

void ControlsAreaWidget::on_copyrightToolButton_clicked()
{
    ui->watermarkText->insertPlainText("©");
}

void ControlsAreaWidget::on_registeredToolButton_clicked()
{
    ui->watermarkText->insertPlainText("®");
}

void ControlsAreaWidget::on_trademarkToolButton_clicked()
{
    ui->watermarkText->insertPlainText("™");
}

void ControlsAreaWidget::on_viewFilesButton_clicked()
{
    foreach(const QString dir, m_processedDirectories)
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
    }
}

void ControlsAreaWidget::on_widthCheckBox_clicked()
{
    updateResizingWidgets();
}

void ControlsAreaWidget::on_heightCheckBox_clicked()
{
    updateResizingWidgets();
}

void ControlsAreaWidget::on_outputImageWidth_textChanged(const QString &arg1)
{
    updateResizingWidgets();
}

void ControlsAreaWidget::on_outputImageHeight_textChanged(const QString &arg1)
{
    updateResizingWidgets();
}

void ControlsAreaWidget::on_generateiTunesScreenShotsPushButton_clicked()
{
    if(m_isRendering)
    {
        stopRendering();
    }
    else
    {
        if(!startMakeiTunesScreenShotImages())
        {
            emit resetProgress();
        }
    }
}

void ControlsAreaWidget::on_generateiOSIconsPushButton_clicked()
{
    if(m_isRendering)
    {
        stopRendering();
    }
    else
    {
        if(!startResizeIconsForIOS())
        {
            emit resetProgress();
        }
    }
}

void ControlsAreaWidget::on_generateiOSImagesPushButton_clicked()
{
    if(m_isRendering)
    {
        stopRendering();
    }
    else
    {
        if(!startResizeImagesForIOS())
        {
            emit resetProgress();
        }
    }
}

void ControlsAreaWidget::on_generateMacOSIconsPushButton_clicked()
{
    if(m_isRendering)
    {
        stopRendering();
    }
    else
    {
        if(!startResizeIconsForMacOS())
        {
            emit resetProgress();
        }
    }
}

void ControlsAreaWidget::on_applyWatermarkButton_clicked()
{
    if(m_isRendering)
    {
        stopRendering();
    }
    else
    {
        if(!startApplyingWatermark())
        {
            resetProgress();
        }
    }
}

void ControlsAreaWidget::on_tileCheckBox_clicked()
{
    m_watermark->tileWatermark( ui->tileCheckBox->isChecked() );
    ui->positionWatermarkGroupBox->setEnabled( !ui->tileCheckBox->isChecked() );
    ui->tileSlider->setEnabled( ui->tileCheckBox->isChecked() );
}

void ControlsAreaWidget::on_tileSlider_valueChanged(int value)
{
    m_watermark->setTileMarginPercent( value );
}

void ControlsAreaWidget::on_clearLogPushButton_clicked()
{
    ui->logMessage->clear();
    emit clearmylog();
}

void ControlsAreaWidget::updateWatermarkRotationWidget()
{
    ui->rotateWatermark->update(-m_watermark->getDegreeRotation());
}

void ControlsAreaWidget::updateWatermarkText()
{
    QString watermarkText = m_watermark->getWatermarkText();
    ui->watermarkText->setPlainText(watermarkText);
    QMetaObject::invokeMethod(this, "onWatermarkSelected");
}

void ControlsAreaWidget::onUpdateWatermarkingWidgets()
{
    QString watermarkFile = m_watermark->getWatermarkFileName();
    int numFiles = ui->fileSelectionWidget->listView()->getFileCount();
    int numCheckedFiles = ui->fileSelectionWidget->listView()->getCheckedFileCount();

    if(numFiles > 0)
    {
        ui->applyWatermarkButton->setEnabled( numCheckedFiles > 0 );
        ui->generateiOSImagesPushButton->setEnabled(numCheckedFiles > 0);
        ui->generateiOSIconsPushButton->setEnabled(numCheckedFiles > 0);
        ui->generateiTunesScreenShotsPushButton->setEnabled(numCheckedFiles > 0);
        ui->fileSelectionWidget->allowOperations(true);
        ui->fileSelectionWidget->updateNumOfFilesWidget();
        ui->iosDeviceIcons->enableListItems(numCheckedFiles > 0);
        ui->iTunesScreenShots->enableListItems(numCheckedFiles > 0);
        showWatermarkingWidgets(true);
    }
    else
    {
        ui->applyWatermarkButton->setEnabled( false );
        ui->generateiOSImagesPushButton->setEnabled( false );
        ui->generateiOSIconsPushButton->setEnabled( false );
        ui->generateiTunesScreenShotsPushButton->setEnabled( false );
        ui->iosDeviceIcons->enableListItems(false);
        ui->iTunesScreenShots->enableListItems(false);
        ui->fileSelectionWidget->allowOperations(false);
        ui->fileSelectionWidget->resetNumOfFilesWidget();
        showWatermarkingWidgets( watermarkFile.size() > 0 );
    }

    if(m_firstTimeUserFileSelectionWidget)
    {
        m_firstTimeUserFileSelectionWidget->allowOperations(
                    m_firstTimeUserFileSelectionWidget->listView()->getFileCount() > 0);
        m_firstTimeUserFileSelectionWidget->updateNumOfFilesWidget();
    }

    ui->transparencyLevel->setText(
                QString("%1%").arg(100-m_watermark->getOpacityPercent()));
    ui->transparencySlider->setValue(
                100-m_watermark->getOpacityPercent());

    ui->tileCheckBox->setChecked( m_watermark->isTilingEnabled() );
    on_tileCheckBox_clicked();
}

void ControlsAreaWidget::on_rotateWatermark(int value)
{
    m_watermark->loadWatermarkRotated(-value);
    if(!m_onSaveSettings) { mylog("m_onSaveSettings"); }
    m_onSaveSettings();
}

void ControlsAreaWidget::setWatermarkImage(const QString &fileName)
{
    if(!fileName.size())
    {
        ui->watermarkImagePreview->setPixmap(QPixmap());
        ui->watermarkFile->setText("");
        m_watermark->loadWatermarkFromFile("");
        return;
    }
    QString watermarkFile = QDir::toNativeSeparators(fileName);
    QImage watermark;
    watermark.load(watermarkFile);
    if(watermark.isNull())
    {
        watermarkFile = "";
    }
    else
    {
        QSize watermarkSize(watermark.width(), watermark.height());
        QSize previewSize = ui->watermarkImagePreview->size();
        if(watermarkSize.width() > previewSize.width() ||
                watermarkSize.height() > previewSize.height())
        {
            watermark = watermark.scaled(previewSize, Qt::KeepAspectRatio);
            ui->watermarkImagePreview->setPixmap(QPixmap::fromImage(watermark));
        }
        else
        {
            ui->watermarkImagePreview->setPixmap(QPixmap::fromImage(watermark));
        }
    }
    ui->watermarkFile->setText(fileName);
    m_watermark->loadWatermarkFromFile(watermarkFile);
    if(!m_onSaveSettings) { mylog("m_onSaveSettings"); }
    m_onSaveSettings();
}

void ControlsAreaWidget::onWatermarkSelected()
{
    QMetaObject::invokeMethod(this, "onUpdateWatermarkingWidgets");
    ui->transparencySlider->setEnabled(true);
    if(m_autoScroll)
    {
        emit scrollToPreview( getWatermarkPlacement() );
    }
    ui->watermarkPositionLabel->setText(QString("%1, %2").
                                        arg(m_watermark->getWatermarkPositionX()).
                                        arg(m_watermark->getWatermarkPositionY()));
}

void ControlsAreaWidget::updateFontWidgetsGroupBoxes()
{
    static bool firstRun = true;
    bool show = ui->watermarkText->toPlainText().size() > 0;
    if(show)
    {
      ui->fontGroupBox->show();
      ui->styleGroupBox->show();
      ui->specialCharactersGroupBox->show();
      ui->watermarkTextLabel->setStyleSheet("");
    }
    else if(firstRun)
    {
        ui->fontGroupBox->hide();
        ui->styleGroupBox->hide();
        ui->specialCharactersGroupBox->hide();
        ui->watermarkTextLabel->setStyleSheet("QLabel {color: red; font-weight:bold;}");
    }
    firstRun = false;
}

void ControlsAreaWidget::updateResizingWidgets()
{
    if(!m_getPreviewedImageSize)
    {
        return;//not init'ed yet
    }
    ImageScaling scaling = getImageScaling();
    switch(scaling)
    {
    case IMG_SCALING_FIXED_WIDTH_AND_HEIGHT:
        ui->outputImageWidth->setEnabled( true );
        ui->outputImageHeight->setEnabled( true );
        break;
    case IMG_SCALING_FIXED_WIDTH:
        ui->outputImageWidth->setEnabled( true );
        ui->outputImageHeight->setEnabled( false );
        if(!m_getPreviewedImageSize) { mylog("m_getPreviewedImageSize"); }
        if(m_getPreviewedImageSize().width() > 0)
        {
            ui->outputImageHeight->setText(QString("%1").arg(
            (m_getPreviewedImageSize().height() * ui->outputImageWidth->text().toInt()) /
                    m_getPreviewedImageSize().width()));
        }
        break;
    case IMG_SCALING_FIXED_HEIGHT:
        ui->outputImageWidth->setEnabled( false );
        ui->outputImageHeight->setEnabled( true );
        if(!m_getPreviewedImageSize) { mylog("m_getPreviewedImageSize"); }
        if(m_getPreviewedImageSize().height() > 0)
        {
            ui->outputImageWidth->setText(QString("%1").arg(
            (m_getPreviewedImageSize().width() * ui->outputImageHeight->text().toInt()) /
                    m_getPreviewedImageSize().height()));
        }
        break;
    case IMG_SCALING_NONE:
        ui->outputImageWidth->setEnabled( false );
        ui->outputImageHeight->setEnabled( false );
        if(!m_getPreviewedImageSize) { mylog("m_getPreviewedImageSize"); }
        ui->outputImageWidth->setText(QString("%1").arg(m_getPreviewedImageSize().width()));
        ui->outputImageHeight->setText(QString("%1").arg(m_getPreviewedImageSize().height()));
        break;
    default:
        break;
    }
    emit updatePreview(getImageScaling());
    m_onSaveSettings();
}

void ControlsAreaWidget::stopRendering()
{
    m_imageHelper->stop();
    ui->progressMessage->append(QString("Cancelling..."));
}

void ControlsAreaWidget::updateWidgetsBeforeProcessingFiles(enum Operation operation)
{
    m_isRendering = true;
    ui->applyWatermarkButton->setText("Cancel");
    ui->generateiOSImagesPushButton->setText("Cancel");
    ui->generateiOSIconsPushButton->setText("Cancel");
    ui->generateiTunesScreenShotsPushButton->setText("Cancel");
    ui->iosDeviceIcons->enableListItems(false);
    ui->iTunesScreenShots->enableListItems(false);
    m_watermark->setAcceptDrops(false);
    ui->fileSelectionWidget->allowDrops(false);
    ui->fileSelectionWidget->allowOperations(false);
    ui->viewFilesButton->setEnabled(false);
    m_processedDirectories.clear();
    initProgress(operation);
}


bool ControlsAreaWidget::startMakeiTunesScreenShotImages()
{
    if(!ui->fileSelectionWidget->listView()->anyCheckedFiles())
        return false;
    updateWidgetsBeforeProcessingFiles(OPERATION_RESIZE_FOR_ITUNES_SCREEN_SHOTS);
    QtConcurrent::run(this, &ControlsAreaWidget::resizeForiTunesScreenShotImages);
    return true;
}

bool ControlsAreaWidget::startResizeIconsForIOS()
{
    if(!ui->fileSelectionWidget->listView()->anyCheckedFiles())
        return false;
    updateWidgetsBeforeProcessingFiles(OPERATION_RESIZE_FOR_IOS_ICONS);
    QtConcurrent::run(this, &ControlsAreaWidget::resizeForIOSIcons);
    return true;
}

bool ControlsAreaWidget::startResizeImagesForIOS()
{
    if(!ui->fileSelectionWidget->listView()->anyCheckedFiles())
        return false;
    int referenceImageSize = ui->referenceImageSize->text().toInt();
    if(referenceImageSize<=0)
    {
        showReferenceImageSizeHelpDialog();
        return false;
    }
    updateWidgetsBeforeProcessingFiles(OPERATION_RESIZE_FOR_IOS_IMAGES);
    QtConcurrent::run(this, &ControlsAreaWidget::resizeForIOSImages);
    return true;
}

bool ControlsAreaWidget::startResizeIconsForMacOS()
{
    if(!ui->fileSelectionWidget->listView()->anyCheckedFiles())
        return false;
    updateWidgetsBeforeProcessingFiles(OPERATION_RESIZE_FOR_MACOS_ICONS);
    QtConcurrent::run(this, &ControlsAreaWidget::resizeForMacOSIcons);
    return true;
}

bool ControlsAreaWidget::startResizing()
{
    if(!ui->fileSelectionWidget->listView()->anyCheckedFiles())
        return false;
    updateWidgetsBeforeProcessingFiles(OPERATION_RESIZING_IMAGES);
    QtConcurrent::run(this, &ControlsAreaWidget::resizeImages);
    return true;
}

bool ControlsAreaWidget::startRotating()
{
    if(!ui->fileSelectionWidget->listView()->anyCheckedFiles())
        return false;
    updateWidgetsBeforeProcessingFiles(OPERATION_ROTATING_IMAGES);
    QtConcurrent::run(this, &ControlsAreaWidget::resizeImages);
    return true;
}

bool ControlsAreaWidget::startApplyingWatermark()
{
    const QString watermark = m_watermark->getWatermarkFileName();
    if(!ui->fileSelectionWidget->listView()->anyCheckedFiles())
        return false;
    updateWidgetsBeforeProcessingFiles(OPERATION_WATERMARKING_IMAGES);
        QtConcurrent::run(this, &ControlsAreaWidget::applyWatermark);
    return true;
}

ImageScaling ControlsAreaWidget::getImageScaling() const
{
    if( ui->widthCheckBox->isChecked() && ui->heightCheckBox->isChecked() )
    {
        return IMG_SCALING_FIXED_WIDTH_AND_HEIGHT;
    }
    else if( ui->widthCheckBox->isChecked() )
    {
        return IMG_SCALING_FIXED_WIDTH;
    }
    else if( ui->heightCheckBox->isChecked() )
    {
        return IMG_SCALING_FIXED_HEIGHT;
    }
    else
    {
        return IMG_SCALING_NONE;
    }
}

QSize ControlsAreaWidget::getSizeSetByUser(const QSize defaultSize) const
{
    ImageScaling scaleForUser = getImageScaling();
    return (scaleForUser != IMG_SCALING_NONE) ?
        QSize(ui->outputImageWidth->text().toInt(),
              ui->outputImageHeight->text().toInt()) :
        defaultSize;
}

void ControlsAreaWidget::on_referenceImageSizeHelpButton_clicked()
{
    showReferenceImageSizeHelpDialog();
}

void ControlsAreaWidget::showReferenceImageSizeHelpDialog()
{
    delete m_dialog->newModalDialog(new ReferenceImageSizeHelp(this), "Resize Help");
}

void ControlsAreaWidget::showWatermarkingWidgets(bool visible)
{
    if(!m_onShowWatermarkingWidgets) { mylog("m_onShowWatermarkingWidgets"); }
    m_onShowWatermarkingWidgets();

    static bool first = true;
    if(!visible && first)
    {
        first = false;
        m_firstTimeUserFileSelectionWidget = new FileSelectionWidget(this);
        qDebug() << "m_firstTimeUserFileSelectionWidget" << (void*)m_firstTimeUserFileSelectionWidget;
        if(!m_onFileListSelectionChanged) { mylog("m_onFileListSelectionChanged"); }
        if(!m_onFileListUpdated) { mylog("m_onFileListUpdated"); }
        if(!m_onSaveSettings) { mylog("m_onSaveSettings"); }
        initFileSelectionWidget(m_firstTimeUserFileSelectionWidget,
                                [this](const QString &imageFile){m_onFileListSelectionChanged(imageFile);},
                                [this](){m_onFileListUpdated();},
                                [this](){m_onSaveSettings();}
        );
        m_firstTimeUserFileSelectionWidget->hideWidgets();
        m_firstTimeUserFileSelectionWidget->enableSimpleMode();
        auto dlg = m_dialog->newModalDialog(m_firstTimeUserFileSelectionWidget, "Files");
        ui->fileSelectionWidget->addItemsFrom(m_firstTimeUserFileSelectionWidget->listView());
        ui->tabWidgetFunctions->setCurrentWidget(ui->tabWatermark);
        delete dlg;
    }
}

void ControlsAreaWidget::initFileSelectionWidget(
        FileSelectionWidget *fileSelectionWidget,
        std::function<void(const QString &imageFile)>onFileListSelectionChanged,
        std::function<void()>onFileListUpdated,
        std::function<void()>onSaveSettings)
{
    fileSelectionWidget->init(
                m_supportedFileTypes,
                [this,onFileListSelectionChanged](){
                    QModelIndex index(ui->fileSelectionWidget->listView()->getCurrentIndex());
                    QString imageFile = ui->fileSelectionWidget->listView()->getAt(index);
                    onFileListSelectionChanged(imageFile);
    });
    fileSelectionWidget->listView()->init(
                m_imageHelper,
                m_supportedFileTypes,
                onFileListUpdated,
                onSaveSettings,
                [fileSelectionWidget](){
                    return fileSelectionWidget->shouldRecursivelyScanDir();
                },
                onFileListSelectionChanged);
}

WaterPlacement ControlsAreaWidget::getWatermarkPlacement() const
{
    WaterPlacement watermarkPlacement;
    if(ui->freeHandDrag->isChecked())
    {
        watermarkPlacement = WaterPlacment_Custom;
    }
    else
    {
        if(ui->radioButtonTopLeft->isChecked())
        {
            watermarkPlacement = WaterPlacement_TopLeft;
        }
        else if(ui->radioButtonTopRight->isChecked())
        {
            watermarkPlacement = WaterPlacement_TopRight;
        }
        else if(ui->radioButtonBottomLeft->isChecked())
        {
            watermarkPlacement = WaterPlacement_BottomLeft;
        }
        else if(ui->radioButtonBottomRight->isChecked())
        {
            watermarkPlacement = WaterPlacement_BottomRight;
        }
        else if(ui->radioButtonCentered->isChecked())
        {
            watermarkPlacement = WaterPlacement_Centered;
        }
        else if(ui->radioButtonCenterLeft->isChecked())
        {
            watermarkPlacement = WaterPlacement_CenterLeft;
        }
        else if(ui->radioButtonCenterTop->isChecked())
        {
            watermarkPlacement = WaterPlacement_CenterTop;
        }
        else if(ui->radioButtonCenterBottom->isChecked())
        {
            watermarkPlacement = WaterPlacement_CenterBottom;
        }
        else if(ui->radioButtonCenterRight->isChecked())
        {
            watermarkPlacement = WaterPlacement_CenterRight;
        }

    }
    return watermarkPlacement;
}

void ControlsAreaWidget::logMessage(const QString *msg)
{
    if(ui && ui->logMessage)
    {
        ui->logMessage->append(*msg);
    }
}

void ControlsAreaWidget::tabSelected()
{
    if(ui->tabWidgetFunctions->currentIndex()==
            ui->tabWidgetFunctions->indexOf(ui->tabCropRect))
    {
        ui->freeHandDrag->setChecked(false);
        m_watermark->setMode(Watermark::WATERMARKING_MODE_CROP_RECT);
    }
    else
    {
        m_watermark->setMode(Watermark::WATERMARKING_MODE_NONE);
    }
}


void ControlsAreaWidget::updateFileListWidgets()
{
    ui->fileSelectionWidget->updateCheckboxes();
}


void ControlsAreaWidget::updateFontWidgets()
{
    removeUnsupportedFonts();
    for(int i = 0; i < ui->fontComboBox->count(); i++)
    {
        if(ui->fontComboBox->itemText(i) == m_fontHelper->getFontFamily())
        {
            ui->fontComboBox->setCurrentIndex(i);
            break;
        }
    }
    connect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), this, SLOT(on_setCurrentFont(QFont)));
    if(!m_fontHelper->getFontFamily().size())
    {
        setFont(ui->fontComboBox->currentFont());
    }
    ui->fontSizePointsSpinBox->setValue(m_fontHelper->getFontSizeInPoints());

    ui->boldFontTypeButton->setEnabled(m_fontHelper->isFontStyleAvailable(FONTHELPER_STYLE_FLAG_BOLD));
    ui->italicsFontTypeButton->setEnabled(m_fontHelper->isFontStyleAvailable(FONTHELPER_STYLE_FLAG_ITALIC));

    ui->boldFontTypeButton->setChecked((FONTHELPER_STYLE_FLAG_BOLD & m_fontHelper->getFontStyle()) == FONTHELPER_STYLE_FLAG_BOLD);
    ui->italicsFontTypeButton->setChecked((FONTHELPER_STYLE_FLAG_ITALIC & m_fontHelper->getFontStyle()) == FONTHELPER_STYLE_FLAG_ITALIC);
    ui->underlineFontTypeButton->setChecked(m_fontHelper->isUnderlined());
}

void ControlsAreaWidget::updateFontStyle()
{
    if(!m_fontHelper->isFontStyleAvailable(FONTHELPER_STYLE_FLAG_BOLD))
    {
        m_fontHelper->removeFontStyle(FONTHELPER_STYLE_FLAG_BOLD);
        ui->boldFontTypeButton->setEnabled(false);
        if(ui->boldFontTypeButton->isChecked())
        {
            ui->boldFontTypeButton->setChecked(false);
        }
    }
    else
    {
        ui->boldFontTypeButton->setEnabled(true);
    }
    if(!m_fontHelper->isFontStyleAvailable(FONTHELPER_STYLE_FLAG_ITALIC))
    {
        m_fontHelper->removeFontStyle(FONTHELPER_STYLE_FLAG_ITALIC);
        ui->italicsFontTypeButton->setEnabled(false);
        if(ui->italicsFontTypeButton->isChecked())
        {
            ui->italicsFontTypeButton->setChecked(false);
        }
    }
    else
    {
        ui->italicsFontTypeButton->setEnabled(true);
    }
    if(!m_fontHelper->isFontStyleAvailable(FONTHELPER_STYLE_FLAG_BOLD) &&
            !m_fontHelper->isFontStyleAvailable(FONTHELPER_STYLE_FLAG_ITALIC))
    {
        m_fontHelper->setFontStyle(FONTHELPER_STYLE_FLAG_REGULAR);
    }
}

void ControlsAreaWidget::on_setCurrentFont(const QFont &font)
{
    if(setFont(font))
    {
        updateFontStyle();
        m_watermark->drawWatermarkText();
    }
}

void ControlsAreaWidget::removeUnsupportedFonts()
{
    for(int i = 0; i< ui->fontComboBox->count(); i++)
    {
        QString family(ui->fontComboBox->itemText(i));
        if(!m_fontHelper->getFonts().contains(family))
        {
            ui->fontComboBox->removeItem(i);
            i--;
        }
    }
}

bool ControlsAreaWidget::setFont(QFont font) const
{
    if(!m_fontHelper->getFonts().contains(font.family()))
    {
        mylog("unrecognized font: %s", font.family().toUtf8().data());
        return false;
    }

    m_fontHelper->setFontFamily(font.family());

    return true;
}

void ControlsAreaWidget::updateAboutPageWidgets()
{
    makePushButtonIntoWebLink(ui->gotoSquiggleMarkPushButton);
    makePushButtonIntoWebLink(ui->gotoLeLesBackyardPushButton);
    makePushButtonIntoWebLink(ui->pushButtonEmail1);
}

void ControlsAreaWidget::makePushButtonIntoWebLink(QWidget *button)
{
    button->setStyleSheet("QPushButton {color: blue; text-decoration:underline}");
    button->setCursor(Qt::PointingHandCursor);
}

void ControlsAreaWidget::setupQualityWidgets()
{
    ui->verticalLayoutForQuality->addWidget(m_qualitySliderJpg);
    ui->verticalLayoutForQuality->addWidget(m_qualitySliderPng);
    ui->verticalLayoutForQuality->addWidget(m_qualitySliderVideo);
}

void ControlsAreaWidget::enableWatermarkingFeature(const QString &title)
{
    ui->tabWidgetFunctions->removeTab(
                ui->tabWidgetFunctions->indexOf(ui->tabiOSAppImages) );
    ui->tabWidgetFunctions->removeTab(
                ui->tabWidgetFunctions->indexOf(ui->tabiOSAppIcons) );
    ui->tabWidgetFunctions->removeTab(
                ui->tabWidgetFunctions->indexOf(ui->tabiTunesScreenShots) );
    ui->tabWidgetFunctions->removeTab(
                ui->tabWidgetFunctions->indexOf(ui->tabMacOSAppIcons) );

    ui->aboutAppLabel->setText(title);
}

void ControlsAreaWidget::disableWatermarkingFeature()
{
    const QString title("SpriteMill - Sprite Generator for iOS Screens");
    const QString fileSelectionAction("What images do you want to use?");
    const QString appHomePageUrl("http://www.osletek.com/spritemill");

    ui->applyWatermarkButton->hide();
    ui->tabWidgetFunctions->removeTab( ui->tabWidgetFunctions->indexOf(ui->tabWatermark) );
    ui->tabWidgetFunctions->removeTab( ui->tabWidgetFunctions->indexOf(ui->tabImageSize) );
    ui->tabWidgetFunctions->removeTab( ui->tabWidgetFunctions->indexOf(ui->tabRotateImages) );
    ui->tabWidgetFunctions->removeTab( ui->tabWidgetFunctions->indexOf(ui->tabCropRect) );
    setWindowTitle(title);
    ui->aboutAppLabel->setText(title);
    ui->fileSelectionWidget->setTitle(fileSelectionAction);
    ui->gotoSquiggleMarkPushButton->setText(appHomePageUrl);
//    m_watermark->setParent(NULL);
    ui->gotoSquiggleMarkPushButtonWithImage->setIcon( QIcon(":/resources/windmill.png") );
}

int ControlsAreaWidget::jpegQuality() const
{
    return m_qualitySliderJpg->getQuality();
}

int ControlsAreaWidget::pngQuality() const
{
    return m_qualitySliderPng->getQuality();
}

int ControlsAreaWidget::videoQuality() const
{
    return m_qualitySliderVideo->getQuality();
}

void ControlsAreaWidget::selectFileIfNoneSelected()
{
    QModelIndex index(ui->fileSelectionWidget->listView()->getCurrentIndex());
    if(!index.isValid())
    {
        ui->fileSelectionWidget->listView()->setCurrentIndex(
                ui->fileSelectionWidget->listView()->getModelIndex(0));
    }
}

void ControlsAreaWidget::updateFileSelectionWidgets()
{
    updateFileSelectionWidget(ui->fileSelectionWidget);
    updateFileSelectionWidget(m_firstTimeUserFileSelectionWidget);
}

void ControlsAreaWidget::updateFileSelectionWidget(FileSelectionWidget* fileSelectionWidget)
{
    if(fileSelectionWidget)
    {
        fileSelectionWidget->updateListViewCheckboxWidgets();
        fileSelectionWidget->updateNumOfFilesWidget();
    }
}

void ControlsAreaWidget::addToProcessedDirectories(const QString& dirPath)
{
    if(!m_processedDirectories.contains(dirPath))
    {
        m_processedDirectories.append(dirPath);
    }
}

void ControlsAreaWidget::enableViewFilesButton(const QString &savedFile)
{
    ui->viewFilesButton->setEnabled(true);
    if(savedFile.size())
    {
        emit signalProgressMessage(new QString((QString("finished %1").arg(QDir::toNativeSeparators(savedFile)))));
    }
}


gdImagePtr ControlsAreaWidget::getWatermarkImage(const QString &watermarkFile) const
{
    gdImagePtr watermarkerImg =  m_imageHelper->getGdImageFromPath(
                watermarkFile.toUtf8().data());
    if(!watermarkerImg)
    {
        return nullptr;
    }

    gdImageSaveAlpha(watermarkerImg, 1);
    gdImageAlphaBlending(watermarkerImg, 0);

    int opacityPercent = m_watermark->getOpacityPercent();
    if(opacityPercent < 100)
    {
        watermarkerImg = m_imageHelper->applyTransparencyToImage(watermarkerImg, opacityPercent);
    }

    int degreeRotation = -ui->rotateWatermark->value();
    if(degreeRotation !=  0)
    {
        watermarkerImg = m_imageHelper->getGdRotatedImage(watermarkerImg, degreeRotation);
    }
    return watermarkerImg;
}

void ControlsAreaWidget::setupLicenseWidget(LicenseWidget *licenseWidget)
{
    ui->horizontalLayoutForLicense->addWidget(licenseWidget);
}

void ControlsAreaWidget::resizeAndWatermark(QList<WatermarkOptions*> watermarkOptions, QList<Device> devices)
{
    if(m_getLicenceType() == LicenseType_Invalid)
    {
        emit signalShowLicenseDialog();
    }
    m_imageHelper->start();
    QList<QFuture<QString> > futures;
    QStringList files = ui->fileSelectionWidget->listView()->getCheckedFiles();
    foreach(const QString &fileName, files)
    {
        if(m_stop())
        {
            break;
        }

        for(int i = 0; i<devices.size(); i++)
        {
            emit signalProgressMessage(new QString(QString("processing %1").arg(QDir::toNativeSeparators(fileName))));
            futures.append(m_imageHelper->resizeAndWatermarkAsync(
                               this,
                               fileName,
                               ui->destinationFolderLineEdit->text(),
                               watermarkOptions,
                               devices[i]));
        }
    }

    while(futures.count() > 0)
    {
        updateProgress(futures);
    }

    deleteGdWatermarkImages(watermarkOptions);
    emit resetProgress();
    m_imageHelper->stop();
}

bool ControlsAreaWidget::resizeImages()
{
    mylog("start resizeImages");
    OutputFormat format = getOutputFormat();
    QList<WatermarkOptions*> list;
    WatermarkOptions *watermarkOptions = new WatermarkOptions(
                m_getLicenceType(), m_trialimg,
                format.m_img_format, format.m_video_format);
    list.append(watermarkOptions);
    QList<Device> devices;
    devices.append(Device( getImageScaling(),
                           -ui->rotateImage->value(),
                           m_crop_rect,
                           ui->outputImageWidth->text().toInt(),
                           ui->outputImageHeight->text().toInt(),
                           1,
                        "", "", IMG_ACTION_KEEP_TRANSPARENCY));
    resizeAndWatermark(list, devices);
    mylog("leave resizeImages");
    return true;
}

bool ControlsAreaWidget::applyWatermark()
{
    mylog("start applyWatermark");
    QList<Device> devices;
    devices.append(Device( getImageScaling(),
                           -ui->rotateImage->value(),
                           m_crop_rect,
                           ui->outputImageWidth->text().toInt(),
                           ui->outputImageHeight->text().toInt(),
                           1,
                           "", "", IMG_ACTION_KEEP_TRANSPARENCY));
    resizeAndWatermark(getWatermarkOptions(), devices);

    mylog("leave applyWatermark");
    return true;
}

bool ControlsAreaWidget::resizeForiTunesScreenShotImages()
{
    mylog("start resizeForiTunesScreenShotImages");
    OutputFormat format = getOutputFormat();
    QList<WatermarkOptions*> list;
    WatermarkOptions *watermarkOptions = new WatermarkOptions(
                m_getLicenceType(), m_trialimg,
                format.m_img_format, format.m_video_format);
    list.append(watermarkOptions);
    resizeAndWatermark(list, ui->iTunesScreenShots->getIOSDevices());
    mylog("leave resizeForiTunesScreenShotImages");
    return true;
}

bool ControlsAreaWidget::resizeForIOSIcons()
{
    mylog("start resizeForIOSIcons");
    OutputFormat format = getOutputFormat();
    QList<WatermarkOptions*> list;
    WatermarkOptions *watermarkOptions = new WatermarkOptions(
                m_getLicenceType(), m_trialimg,
                format.m_img_format, format.m_video_format);
    list.append(watermarkOptions);
    resizeAndWatermark(list, ui->iosDeviceIcons->getIOSDevices());
    mylog("leave resizeForIOSIcons");
    return true;
}

bool ControlsAreaWidget::resizeForIOSImages()
{
//    convertToWebP();
//    return true;
    mylog("start resizeForIOSImages");
    OutputFormat format = getOutputFormat();
    QList<WatermarkOptions*> list;
    WatermarkOptions *watermarkOptions = new WatermarkOptions(
                m_getLicenceType(), m_trialimg,
                format.m_img_format, format.m_video_format);
    list.append(watermarkOptions);
    int referenceImageSize = ui->referenceImageSize->text().toInt();
    if(referenceImageSize > 0)
    {
        IOSDeviceProperties iosDeviceProperties(
                    referenceImageSize,
                    ui->radioButtonReferenceImageWidth->isChecked() ?
                        IMG_SCALE_WIDTH_AND_CROP_HEIGHT :
                        IMG_SCALE_HEIGHT_AND_CROP_WIDTH,
                    ui->trueColor->isChecked(),
                    false);
        QList<Device> devices = ui->iosDeviceImages->getIOSDevices(
                    &iosDeviceProperties);
        resizeAndWatermark(list, devices);
        mylog("leave resizeForIOSImages");
        return true;
    }
    emit resetProgress();
    return false;
}

bool ControlsAreaWidget::resizeForMacOSIcons()
{
    mylog("start resizeForMacOSIcons");
    OutputFormat format = getOutputFormat();
    QList<WatermarkOptions*> list;
    WatermarkOptions *watermarkOptions = new WatermarkOptions(
                m_getLicenceType(), m_trialimg,
                format.m_img_format, format.m_video_format);
    list.append(watermarkOptions);
    resizeAndWatermark(list, ui->macosDeviceIcons->getIOSDevices());
    mylog("leave resizeForMacOSIcons");
    return true;
}

QList<WatermarkOptions*> ControlsAreaWidget::getWatermarkOptions()
{
    QList<WatermarkOptions *> list;
    OutputFormat format = getOutputFormat();
    list.append(
        new WatermarkOptions(
            m_getLicenceType(),
            m_trialimg,
            format.m_img_format, format.m_video_format,
            getWatermarkImage(m_watermark->getWatermarkFileName()),//TODO combine watermark and trial images into one
            m_watermark->getWatermarkPlacement(),
            m_watermark->getOpacityPercent(),
            m_watermark->getMarginX(false),
            m_watermark->getMarginY(false),
            m_watermark->isTilingEnabled(),
            m_watermark->getTileMarginPercent())
        );
    return list;
}

void ControlsAreaWidget::on_freeHandDrag_clicked()
{
    if(ui->freeHandDrag->isChecked())
    {
        m_watermark->setMode( Watermark::WATERMARKING_MODE_DRAG );
        updateWatermark();
        ui->autoScrollCheckBox->setChecked(false);
        m_autoScroll = false;
        enableWatermarkPositioningRadioButtons(false);
    }
    else
    {
        m_watermark->setMode( Watermark::WATERMARKING_MODE_NONE );
        enableWatermarkPositioningRadioButtons(true);
    }
}

void ControlsAreaWidget::on_pushButtonEmail1_clicked()
{
     QDesktopServices::openUrl(QUrl(ui->pushButtonEmail1->text()));
}

void ControlsAreaWidget::updateProgress(QList<QFuture<QString> > &futures)
{
    for(int i = 0; i < futures.count(); )
    {
        const QFuture<QString> &future = futures.at(i);
        if(future.isResultReadyAt(i))
        {
            if(!m_stop())
            {
                QString savedFile = future.result();
                emit showProgress(savedFile);
            }
            futures.removeAt(i);
            continue;
        }
        ++i;
    }
}

void ControlsAreaWidget::deleteGdWatermarkImages(QList<WatermarkOptions*> watermarkOptions)
{
    foreach(const WatermarkOptions *options, watermarkOptions)
    {
        if(options->m_watermarkimg)
        {
            gdImageDestroy(options->m_watermarkimg);
            delete options;
        }
    }
}

void ControlsAreaWidget::loadTrialImage(bool spritemill)
{
    m_trialimg = m_imageHelper->getGdImageFromQImage( QImage(
        spritemill ? ":/resources/spritemill-watermark.png" : ":/resources/squigglemark-watermark.png") );
}

void ControlsAreaWidget::on_cropRectUpdated(const QRect &crop_rect)
{
    ui->lineEditCropTop->setText(QString("%1").arg(crop_rect.top()));
    ui->lineEditCropLeft->setText(QString("%1").arg(crop_rect.left()));
    ui->lineEditCropBottom->setText(QString("%1").arg(crop_rect.bottom()));
    ui->lineEditCropRight->setText(QString("%1").arg(crop_rect.right()));
    ui->labelCropLeftTopRightBottom->setText(QString("(%1,%2) x (%3,%4) -> (%5,%6)").
                             arg(crop_rect.left()).
                             arg(crop_rect.top()).
                             arg(crop_rect.right()).
                             arg(crop_rect.bottom()).
                             arg(crop_rect.width()).
                             arg(crop_rect.height()));
    m_crop_rect = crop_rect;
}

void ControlsAreaWidget::enableWatermarkPositioningRadioButtons( bool enabled )
{
    ui->radioButtonTopLeft->setEnabled(enabled);
    ui->radioButtonTopRight->setEnabled(enabled);
    ui->radioButtonBottomLeft->setEnabled(enabled);
    ui->radioButtonBottomRight->setEnabled(enabled);
    ui->radioButtonCentered->setEnabled(enabled);
    ui->radioButtonCenterLeft->setEnabled(enabled);
    ui->radioButtonCenterTop->setEnabled(enabled);
    ui->radioButtonCenterRight->setEnabled(enabled);
    ui->radioButtonCenterBottom->setEnabled(enabled);
}

int ControlsAreaWidget::rotateImageValue() const
{
    return ui->rotateImage->value();
}

void ControlsAreaWidget::initProgress(Operation operation)
{
    int numFilesToProcess = 0;
    switch(operation)
    {
    case OPERATION_RESIZING_IMAGES:
    case OPERATION_ROTATING_IMAGES:
    case OPERATION_WATERMARKING_IMAGES:
    {
        int numFrames = 0;
        numFilesToProcess += ui->fileSelectionWidget->listView()->getCheckedFileCount(&numFrames);
        numFilesToProcess += numFrames;
        if(getImageScaling() != IMG_SCALING_NONE)
        {
            numFilesToProcess += numFrames;
        }
        if(ui->rotateImage->value() != 0)
        {
            numFilesToProcess += numFrames;
        }
        break;
    }
    case OPERATION_RESIZE_FOR_ITUNES_SCREEN_SHOTS:
        numFilesToProcess = ui->fileSelectionWidget->listView()->getCheckedFileCount() *
                ui->iTunesScreenShots->getCheckedDevicesCount();
        break;
    case OPERATION_RESIZE_FOR_IOS_ICONS:
        numFilesToProcess = ui->fileSelectionWidget->listView()->getCheckedFileCount() *
                ui->iosDeviceIcons->getCheckedDevicesCount();
        break;
    case OPERATION_RESIZE_FOR_IOS_IMAGES:
        numFilesToProcess = ui->fileSelectionWidget->listView()->getCheckedFileCount() *
                ui->iosDeviceImages->getCheckedDevicesCount();
        break;
    case OPERATION_RESIZE_FOR_MACOS_ICONS:
        numFilesToProcess = ui->fileSelectionWidget->listView()->getCheckedFileCount() *
                ui->macosDeviceIcons->getCheckedDevicesCount();
        break;
    default:
        numFilesToProcess = 1;
    }

    ui->logMessage->clear();
    emit initProgressBar(numFilesToProcess);
}

void ControlsAreaWidget::updateWatermark()
{
    WaterPlacement placement = getWatermarkPlacement();
    m_watermark->setWatermarkPlacement(placement);
    ui->watermarkPositionLabel->setText(QString("%1, %2").
                                        arg(m_watermark->getWatermarkPositionX()).
                                        arg(m_watermark->getWatermarkPositionY()));
    emit updatePreview(getImageScaling());
    emit scrollToPreview(placement);
    updateResizingWidgets();

    emit updatePreviewImageDimensions();
}

void ControlsAreaWidget::loadSettings(QXmlStreamReader  &reader)
{
    loadResizeSettings(reader);
    loadOutputFormatSettings(reader);
    ui->fileSelectionWidget->listView()->loadSettings(reader);
    m_qualitySliderJpg->loadSettings(reader);
    m_qualitySliderPng->loadSettings(reader);
    m_qualitySliderVideo->loadSettings(reader);
//xxx
//    ui->iosDeviceImages->loadIOSDevices(reader);
//    ui->iosDeviceIcons->loadIOSDevices(reader);
//    ui->iTunesScreenShots->loadIOSDevices(reader);

    if(reader.name() == "DestinationDirectory")
    {
        ui->destinationFolderLineEdit->setText( reader.readElementText() );
    }
}

void ControlsAreaWidget::loadOutputFormatSettings(QXmlStreamReader &reader)
{
    if(reader.name() == "OutputImage");
    {
        foreach(const QXmlStreamAttribute &attribute, reader.attributes())
        {
            if(attribute.name() == "Format")
            {
                if(attribute.value() == "original")
                {
                    ui->radioButtonOriginalImageFormat->setChecked(true);
                }
                if(attribute.value() == "jpg")
                {
                    ui->radioButtonJpg->setChecked(true);
                }
                if(attribute.value() == "png")
                {
                    ui->radioButtonPng->setChecked(true);
                }
            }
        }
    }

    if(reader.name() == "OutputVideo");
    {
        foreach(const QXmlStreamAttribute &attribute, reader.attributes())
        {
            if(attribute.name() == "Format")
            {
                if(attribute.value() == "original")
                {
                    ui->radioButtonOriginalImageFormat->setChecked(true);
                }
                if(attribute.value() == "mp4")
                {
                    ui->radioButtonMp4->setChecked(true);
                }
                if(attribute.value() == "ogg")
                {
                    ui->radioButtonOgg->setChecked(true);
                }
            }
        }
    }
}

void ControlsAreaWidget::loadResizeSettings(QXmlStreamReader  &reader)
{
    if(reader.name() == "Resize")
    {
        foreach(const QXmlStreamAttribute &attribute, reader.attributes())
        {
            if(attribute.name() == "FixedWidth")
            {
                ui->widthCheckBox->setChecked(attribute.value().toInt() == 0 ? false : true);
            }
            else if(attribute.name() == "FixedHeigth")
            {
                ui->heightCheckBox->setChecked(attribute.value().toInt() == 0 ? false : true);
            }
            else if(attribute.name() == "Width")
            {
                ui->outputImageWidth->setText(attribute.value().toString());
            }
            else if(attribute.name() == "Height")
            {
                ui->outputImageHeight->setText(attribute.value().toString());
            }
            else if(attribute.name() == "RotationAngle")
            {
                ui->rotateImage->update(attribute.value().toString().toInt());
            }
            else if(attribute.name() == "ReferenceImageSize")
            {
                ui->referenceImageSize->setText(attribute.value().toString());
            }
            else if(attribute.name() == "TrueColor")
            {
                ui->trueColor->setChecked(attribute.value().toInt() == 0 ? false : true);
            }
        }
    }
}

void ControlsAreaWidget::saveSettings(QXmlStreamWriter &writer)
{
    writer.writeTextElement("DestinationDirectory", ui->destinationFolderLineEdit->text());
    ui->fileSelectionWidget->listView()->saveSettings(writer);
    m_qualitySliderJpg->saveSettings(writer);
    m_qualitySliderPng->saveSettings(writer);
    m_qualitySliderVideo->saveSettings(writer);
    m_watermark->saveSettings(writer);
    ui->iosDeviceImages->saveIOSDevices(writer);
    ui->iosDeviceIcons->saveIOSDevices(writer);
    ui->iTunesScreenShots->saveIOSDevices(writer);
    saveResizeSettings(writer);
    saveOutputFormatSettings(writer);
}

ControlsAreaWidget::OutputFormat ControlsAreaWidget::getOutputFormat() const
{
    OutputFormat format;

    if(ui->radioButtonOriginalImageFormat->isChecked())
    {
        format.m_img_format_string = "original";
        format.m_img_format = OutputImageFormat_Original;
    }
    else if(ui->radioButtonJpg->isChecked())
    {
        format.m_img_format_string = "jpg";
        format.m_img_format = OutputImageFormat_Jpg;
    }
    else if(ui->radioButtonPng->isChecked())
    {
        format.m_img_format_string = "png";
        format.m_img_format = OutputImageFormat_Png;
    }

    if(ui->radioButtonOriginalVideoFormat->isChecked())
    {
        format.m_video_format_string = "original";
        format.m_video_format = OutputVideoFormat_Original;
    }
    else if(ui->radioButtonMp4->isChecked())
    {
        format.m_video_format_string = "mp4";
        format.m_video_format = OutputVideoFormat_Mp4;
    }
    else if(ui->radioButtonOgg->isChecked())
    {
        format.m_video_format_string = "ogg";
        format.m_video_format = OutputVideoFormat_Ogg;
    }

    return format;
}

void ControlsAreaWidget::saveOutputFormatSettings(QXmlStreamWriter &writer)
{
    writer.writeStartElement("OutputImage");
        writer.writeAttribute("Format", getOutputFormat().m_img_format_string);
    writer.writeEndElement();

    writer.writeStartElement("OutputVideo");
        writer.writeAttribute("Format", getOutputFormat().m_video_format_string);
    writer.writeEndElement();
}

void ControlsAreaWidget::saveResizeSettings(QXmlStreamWriter &writer)
{
    writer.writeStartElement("Resize");
        writer.writeAttribute("FixedWidth", ui->widthCheckBox->isChecked() ?
                                  QString("%1").arg(1) : QString("%1").arg(0));
        writer.writeAttribute("FixedHeight", ui->heightCheckBox->isChecked() ?
                                  QString("%1").arg(1) : QString("%1").arg(0));
        writer.writeAttribute("Width", ui->outputImageWidth->text());
        writer.writeAttribute("Height", ui->outputImageHeight->text());
        writer.writeAttribute("RotationAngle", QString("%1").arg(ui->rotateImage->value()));
        writer.writeAttribute("ReferenceImageSize", ui->referenceImageSize->text());
        writer.writeAttribute("TrueColor", ui->trueColor->isChecked() ?
                                  QString("%1").arg(1) : QString("%1").arg(0));
    writer.writeEndElement();
}

void ControlsAreaWidget::loadLicenseTerms()
{
    ui->labelOsLeTekLicense->setText(loadTextFileFromResources("license.txt"));
    ui->label3rdPartyLicenses->setText(loadTextFileFromResources("license3.txt"));
}

QString ControlsAreaWidget::loadTextFileFromResources(const char* filename)
{
    QString path = QString(":/resources/%1").arg(filename);
    QResource r(path);
    QString text = QString(
                QByteArray( reinterpret_cast< const char* >( r.data() ), r.size() ) );
    return text;
}

QString ControlsAreaWidget::getSelectedFileForPreview(const QModelIndex &index) const
{
    return ui->fileSelectionWidget->listView()->getAt(index);
}

void ControlsAreaWidget::onBgImageSelected(const QSize& previewImageSize, const QSize& originalImageSize)
{
    m_watermark->setBgImageSize(
                previewImageSize,
                getSizeSetByUser(originalImageSize));
}

void ControlsAreaWidget::on_clearFileListButton_clicked()
{
    QMetaObject::invokeMethod(this, "onUpdateWatermarkingWidgets");
    ui->fileSelectionWidget->listView()->clear();
    ui->fileSelectionWidget->updateNumOfFilesWidget();
    ui->fileSelectionWidget->updateListViewCheckboxWidgets();
}


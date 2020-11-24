#ifndef CONTROLSAREAWIDGET_H
#define CONTROLSAREAWIDGET_H

#include <gd.h>
#include <functional>

#include <QFuture>
#include <QWidget>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "WatermarkPlacement.h"

class QXmlStreamReader;
class QXmlStreamWriter;

namespace Ui {
class ControlsAreaWidget;
}

class FileSelectionWidget;
class FontHelper;
class ImageHelper;
class LicenseWidget;
class PreviewArea;
class QualitySlider;
class SupportedFileTypes;
class UpdaterWidget;
class Watermark;
class ModalDialog;
class ControlsAreaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ControlsAreaWidget(QWidget *parent = 0);
    ~ControlsAreaWidget();

    void init(
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
            );
    void selectFileIfNoneSelected();
    void addToProcessedDirectories(const QString& dirPath);
    void enableViewFilesButton(const QString &savedFile);
    gdImagePtr getWatermarkImage(const QString &watermarkFile) const;
    bool isRendering() const { return m_isRendering; }
    void setRendering() { m_isRendering = true; }
    void updateFileSelectionWidgets();
    WaterPlacement getWatermarkPlacement() const;
    void logMessage(const QString *msg);
    void onWatermarkSelected();
    void setupLicenseWidget(LicenseWidget *licenseWidget);
    ImageScaling getImageScaling() const;
    int jpegQuality() const;
    int pngQuality() const;
    int videoQuality() const;
    void enableWatermarkingFeature(const QString &title);
    void disableWatermarkingFeature();
    void stopRendering();
    void loadSettings(QXmlStreamReader  &xml);
    void saveSettings(QXmlStreamWriter &xml);
    QString getSelectedFileForPreview(const QModelIndex &index) const;
    QSize getSizeSetByUser(const QSize defaultSize) const;
    int rotateImageValue() const;
    void updateWidgetsAfterProcessingFiles();

public slots:
    void onBgImageSelected(const QSize&, const QSize&);
    void onUpdateWatermarkingWidgets();

private slots:
    void on_applyWatermarkButton_clicked();
    void on_radioButtonBottomLeft_clicked();
    void on_radioButtonTopRight_clicked();
    void on_radioButtonTopLeft_clicked();
    void on_radioButtonCentered_clicked();
    void on_radioButtonBottomRight_clicked();
    void on_watermarkFileDialogButton_clicked();
    void on_transparencySlider_valueChanged(int value);
    void on_increaseFontSizeButton_clicked();
    void on_reduceFontSizeButton_clicked();
    void on_fontSizePointsSpinBox_valueChanged(int arg1);
    void on_colorPickerToolButton_clicked();
    void on_boldFontTypeButton_clicked();
    void on_italicsFontTypeButton_clicked();
    void on_gotoSquiggleMarkPushButtonWithImage_clicked();
    void on_gotoSquiggleMarkPushButton_clicked();
    void on_gotoLeLesBackyardPushButton_clicked();
    void on_copyrightToolButton_clicked();
    void on_registeredToolButton_clicked();
    void on_trademarkToolButton_clicked();
    void on_underlineFontTypeButton_clicked();
    void on_viewFilesButton_clicked();
    void on_autoScrollCheckBox_clicked();
    void on_outputImageWidth_textChanged(const QString &arg1);
    void on_outputImageHeight_textChanged(const QString &arg1);
    void on_appendProgressMessage(const QString* msg);
    void on_generateiOSImagesPushButton_clicked();
    void on_generateMacOSIconsPushButton_clicked();
    void on_clearLogPushButton_clicked();
    void on_widthCheckBox_clicked();
    void on_heightCheckBox_clicked();
    void on_freeHandDrag_clicked();
    void on_reloadWatermarkFile_clicked();
    void on_tileCheckBox_clicked();
    void on_tileSlider_valueChanged(int value);
    void on_destinationFolderPushButton_clicked();
    void on_watermarkText_textChanged();
    void on_generateiOSIconsPushButton_clicked();
    void on_generateiTunesScreenShotsPushButton_clicked();
    void on_referenceImageSizeHelpButton_clicked();
    void on_rotateWatermark(int);
    void on_rotateImage(int);
    void on_pushButtonEmail1_clicked();
    void on_cropRectUpdated(const QRect &crop_rect);
    void on_clearFileListButton_clicked();
    void colorPickerCurrentColorChanged(QColor);
    void updateWatermark();
    void updateWatermarkText();
    void tabSelected();
    void updateFileListWidgets();
    void on_setCurrentFont(const QFont &font);
    void updateProgress(QList<QFuture<QString> > &futures);

    void on_radioButtonCenterLeft_clicked();

    void on_radioButtonCenterTop_clicked();

    void on_radioButtonCenterBottom_clicked();

    void on_radioButtonCenterRight_clicked();

signals:
    void signalAppendLogMessage(const QString*);
    void updatePreview(ImageScaling);
    void initProgressBar(int numFilesToProcess);
    void resetProgress();
    void scrollToPreview(WaterPlacement placement);
    void clearmylog();
    void signalShowLicenseDialog();
    void signalProgressMessage(const QString*);
    void showProgress(QString);
    void updatePreviewImageDimensions();
    void signalProgress(int);
    void signalEndProgress();

private:
    Ui::ControlsAreaWidget *ui;
    Watermark *m_watermark;
    bool m_autoScroll;
    ImageHelper *m_imageHelper;
    SupportedFileTypes *m_supportedFileTypes;
    FontHelper *m_fontHelper;
    QList<QString> m_processedDirectories;
    bool m_isRendering;
    QualitySlider *m_qualitySliderJpg;
    QualitySlider *m_qualitySliderPng;
    QualitySlider *m_qualitySliderVideo;
    FileSelectionWidget *m_firstTimeUserFileSelectionWidget;
    gdImagePtr m_trialimg;
    QRect m_crop_rect;
    ModalDialog *m_dialog;
    void enableWatermarkPositioningRadioButtons(bool enabled);
    std::function<bool()>m_stop;
    std::function<QSize()>m_getPreviewedImageSize;
    std::function<void()>m_onShowWatermarkingWidgets;
    std::function<LicenseType()>m_getLicenceType;
    std::function<void(const QString &imageFile)>m_onFileListSelectionChanged;
    std::function<void()>m_onFileListUpdated;
    std::function<void()>m_onSaveSettings;

    struct OutputFormat
    {
        OutputImageFormat m_img_format;
        QString m_img_format_string;
        OutputVideoFormat m_video_format;
        QString m_video_format_string;
    };

    void setWatermarkImage(const QString &fileName);
    void updateFontWidgetsGroupBoxes();
    void updateResizingWidgets();
    void updateWidgetsBeforeProcessingFiles(Operation operation);
    bool startApplyingWatermark();
    bool startResizeIconsForIOS();
    bool startResizeImagesForIOS();
    bool startResizeIconsForMacOS();
    bool startMakeiTunesScreenShotImages();
    bool startResizing();
    bool startRotating();
    void showReferenceImageSizeHelpDialog();
    void showWatermarkingWidgets(bool visible);
    void updateWatermarkRotationWidget();
    void initFileSelectionWidget(
            FileSelectionWidget *fileSelectionWidget,
            std::function<void(const QString &imageFile)>onFileListSelectionChanged,
            std::function<void()>onFileListUpdated,
            std::function<void()>onSaveSettings);
    void updateFontWidgets();
    void updateFontStyle();
    void removeUnsupportedFonts();
    bool setFont(QFont font) const;
    void updateAboutPageWidgets();
    void makePushButtonIntoWebLink(QWidget *button);
    void setupQualityWidgets();
    void updateFileSelectionWidget(FileSelectionWidget* fileSelectionWidget);
    bool applyWatermark();
    bool resizeImages();
    bool resizeForIOSIcons();
    bool resizeForIOSImages();
    bool resizeForMacOSIcons();
    bool resizeForiTunesScreenShotImages();
    void resizeAndWatermark(QList<WatermarkOptions*> options, QList<Device> devices);
    QList<WatermarkOptions*> getWatermarkOptions();
    void deleteGdWatermarkImages(QList<WatermarkOptions*> watermarkOptions);
    void loadTrialImage(bool spritemill);
    void initProgress(Operation operation);
    void loadResizeSettings(QXmlStreamReader  &reader);
    void loadOutputFormatSettings(QXmlStreamReader &reader);
    void saveResizeSettings(QXmlStreamWriter &writer);
    void saveOutputFormatSettings(QXmlStreamWriter &writer);
    OutputFormat getOutputFormat() const;
    void loadLicenseTerms();
    QString loadTextFileFromResources(const char* filename);
};
Q_DECLARE_METATYPE(ImageScaling)
Q_DECLARE_METATYPE(Operation)
Q_DECLARE_METATYPE(WaterPlacement)

#endif // CONTROLSAREAWIDGET_H

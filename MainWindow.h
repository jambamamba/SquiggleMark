#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gd.h>

#include <QFuture>
#include <QUuid>
#include <QMainWindow>
#include <QMap>
#include <QMutex>

#include "License.h"
#include "MainWindowInterface.h"
#include "SupportedFileTypes.h"
#include "WatermarkPlacement.h"

namespace Ui {
class MainWindow;
}

class QAbstractButton;
class QModelIndex;
class QProgressBar;
class QMutexLocker;
class QFileDialog;
class QSplitter;

class FileSelectionWidget;
class ImageHelper;
class PreviewArea;
class LicenseWidget;
class Watermark;
class FontHelper;
class QXmlStreamWriter;
class QXmlStreamReader;
class UpdaterWidget;
class ControlsAreaWidget;
class PreviewAreaWidget;
class MainSplitter;
class LicenseWidget;
class ModalDialog;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void appendLogMessage(const QString *msg);

protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent (QResizeEvent *);

signals:
    void signalProgress();
    void signalProgress(QString);
    void signalAppendLogMessage(const QString*);
    void signalBgImageSelected(const QSize&, const QSize&);

private:
    Ui::MainWindow *ui;
    bool m_stop;
    Watermark *m_watermark;
    QMutex m_mutex;
    QString m_imageFileToPreview;
    ImageHelper *m_imageHelper;
    FontHelper *m_fontHelper;
    PreviewArea *m_previewArea;
    QString m_additionalFontsDir;
    int m_numFilesToProcess;
    int m_numFilesProcessed;
    UpdaterWidget *m_updater_widget;
    SupportedFileTypes m_supportedFileTypes;
    LicenseType m_licenseType;
    QFuture<void> m_futureFileSelecetionChanged;
    QString m_exePath;
    QUuid m_guid;
    MainSplitter *m_splitter;
    ControlsAreaWidget *m_controlsAreaWidget;
    PreviewAreaWidget *m_previewAreaWidget;
    LicenseWidget *m_license_widget;
    ModalDialog *m_dialog;

    void onFileListUpdated();
    virtual void onBgImapeSelected();
    virtual void onLicenseValidated(int licenseType);
    void onFileListSelectionChanged(const QString &imageFile);
    void loadSplitterView();
    void onShowWatermarkingWidgets();
    void loadSettings();
//    void setWatermarkPlacement(WaterPlacement placement);
    bool shouldStopWatermarking() const { return m_stop; }
    void resetProgressBar();
    void setImageFileToPreview(QString imageFile);
    QString getImageFileToPreview();
//    void updateResizingForiOSWidgets();
    void setupApplicationType();
    void initializeLicenseWidget();
    void stop();
    void writemylog(const QString *msg);
    Watermark* createNewLayer(PreviewArea* previewArea);
    bool loadFonts();
    void setupSplitter();
    LicenseWidget *newLicenseWidget();

public slots:
    void updateProgress(int);
    void endProgress();

private slots:
    bool close();
    void updateProgress(const QString savedFile);
//    bool convertToWebP();
    void initProgressBar(int numFilesToProcess);
    void resetProgress();
    void onSaveSettings();
    void onAppendLogMessage(const QString *msg);
    void updatePreview();
    void clearmylog();
    void onSplitterMoved(int pos, int index);
    void showLicenseDialog();
};

#endif // MAINWINDOW_H

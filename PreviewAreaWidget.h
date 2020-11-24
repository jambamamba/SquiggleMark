#ifndef PREVIEWAREAWIDGET_H
#define PREVIEWAREAWIDGET_H

#include <functional>

#include <QWidget>

#include "WatermarkPlacement.h"

class QXmlStreamReader;
class QXmlStreamWriter;

namespace Ui {
class PreviewAreaWidget;
}

class ImageHelper;
class PreviewArea;
class Watermark;
class PreviewAreaWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewAreaWidget(QWidget *parent = 0);
    ~PreviewAreaWidget();

    void init(Watermark* watermark,
              PreviewArea *previewArea,
              ImageHelper *imageHelper,
              std::function<QString()>getImageFileToPreview,
              std::function<void()>onBgImapeSelected,
              std::function<void()>onUpdatePreview);
    void showWatermarkingWidgets();
    bool adjustToFitPreview() const;
    QSize maximumViewportSize() const;
    void loadSettings(QXmlStreamReader  &reader);
    void saveSettings(QXmlStreamWriter  &writer);

public slots:
    void updatePreviewImageDimensions();
    void scrollToPreview(WaterPlacement placement);

private:
    Ui::PreviewAreaWidget *ui;
    Watermark* m_watermark;
    PreviewArea *m_previewArea;
    std::function<void()>m_onUpdatePreview;

private slots:
    void on_adjustToFitPreview_clicked();

};

#endif // PREVIEWAREAWIDGET_H

#include <QScrollBar>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "ImageHelper.h"
#include "PreviewArea.h"
#include "PreviewAreaWidget.h"
#include "WaterMark.h"
#include "ui_PreviewAreaWidget.h"

extern "C" void mylog(const char *fmt, ...);

PreviewAreaWidget::PreviewAreaWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PreviewAreaWidget)
    , m_watermark(nullptr)
    , m_previewArea(nullptr)
    , m_onUpdatePreview(nullptr)
{
    ui->setupUi(this);
}

void PreviewAreaWidget::init(Watermark* watermark,
                             PreviewArea *previewArea,
                             ImageHelper *imageHelper,
                             std::function<QString()>getImageFileToPreview,
                             std::function<void()>onBgImapeSelected,
                             std::function<void()>onUpdatePreview)
{
    m_watermark = watermark;
    m_previewArea = previewArea;
    m_onUpdatePreview = onUpdatePreview;

    m_previewArea->init(
                getImageFileToPreview,
                onBgImapeSelected,
                imageHelper, ui->moviePlayerWidget);

    ui->previewWidget->setWidget(m_previewArea);
}

PreviewAreaWidget::~PreviewAreaWidget()
{
    delete ui;
}

void PreviewAreaWidget::on_adjustToFitPreview_clicked()
{
    if(!m_onUpdatePreview) { mylog("m_onUpdatePreview"); }
    m_onUpdatePreview();
}

bool PreviewAreaWidget::adjustToFitPreview() const
{
    return ui->adjustToFitPreview->isChecked();
}

QSize PreviewAreaWidget::maximumViewportSize() const
{
    QSize sz = ui->previewWidget->maximumViewportSize();
    return sz;
}

void PreviewAreaWidget::updatePreviewImageDimensions()
{
    QSize imageSize = m_previewArea->getOriginalImageSize();
    ui->previewImageDimensions->setText(QString("%1 x %2").arg(imageSize.width()).arg(imageSize.height()));
}

void PreviewAreaWidget::loadSettings(QXmlStreamReader  &reader)
{
    if(reader.name() == "FitInPreviewWindow")
    {
        ui->adjustToFitPreview->setCheckState(
                    reader.readElementText().toInt() == 1 ? Qt::Checked : Qt::Unchecked);
    }
}

void PreviewAreaWidget::saveSettings(QXmlStreamWriter  &writer)
{
    writer.writeTextElement("FitInPreviewWindow", ui->adjustToFitPreview->checkState() == Qt::Checked ? "1" : "0");
}

void PreviewAreaWidget::showWatermarkingWidgets()
{
//    ui->rightPane->show();
    ui->previewGroupBox->show();
    ui->adjustToFitPreview->show();
    ui->previewWidget->show();
}

void PreviewAreaWidget::scrollToPreview(WaterPlacement placement)
{
    if(!m_previewArea->isPlayingMovie())
    {
        switch(placement)
        {
        case WaterPlacement_TopLeft:
            ui->previewWidget->horizontalScrollBar()->setValue(0);
            ui->previewWidget->verticalScrollBar()->setValue(0);
            break;
        case WaterPlacement_TopRight:
            ui->previewWidget->horizontalScrollBar()->setValue(m_previewArea->width());
            ui->previewWidget->verticalScrollBar()->setValue(0);
            break;
        case WaterPlacement_BottomLeft:
            ui->previewWidget->horizontalScrollBar()->setValue(0);
            ui->previewWidget->verticalScrollBar()->setValue(m_previewArea->height());
            break;
        case WaterPlacement_BottomRight:
            ui->previewWidget->horizontalScrollBar()->setValue(m_previewArea->width());
            ui->previewWidget->verticalScrollBar()->setValue(m_previewArea->height());
            break;
        case WaterPlacement_Centered:
            ui->previewWidget->horizontalScrollBar()->setValue(m_previewArea->width()/2 - 50);
            ui->previewWidget->verticalScrollBar()->setValue(m_previewArea->height()/2 - 50);
            break;
        case WaterPlacment_Custom:
            ui->previewWidget->horizontalScrollBar()->setValue(m_watermark->getMarginX());
            ui->previewWidget->verticalScrollBar()->setValue(m_watermark->getMarginY());
            break;
        case WaterPlacement_CenterLeft:
            ui->previewWidget->horizontalScrollBar()->setValue(0);
            ui->previewWidget->verticalScrollBar()->setValue(m_previewArea->height()/2 - 50);
            break;
        case WaterPlacement_CenterTop:
            ui->previewWidget->horizontalScrollBar()->setValue(m_previewArea->width()/2 - 50);
            ui->previewWidget->verticalScrollBar()->setValue(0);
            break;
        case WaterPlacement_CenterBottom:
            ui->previewWidget->horizontalScrollBar()->setValue(m_previewArea->width()/2 - 50);
            ui->previewWidget->verticalScrollBar()->setValue(m_previewArea->height());
            break;
        case WaterPlacement_CenterRight:
            ui->previewWidget->horizontalScrollBar()->setValue(m_previewArea->width());
            ui->previewWidget->verticalScrollBar()->setValue(m_previewArea->height()/2 - 50);
            break;
        }
    }
}

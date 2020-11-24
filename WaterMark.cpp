#include <QDir>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QUrl>
#include <QPainter>
#include <QMimeData>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QWidget>

#include "gd.h"
#include "FontHelper.h"
#include "WaterMark.h"
#include "ImageHelper.h"

extern "C" void mylog(const char *fmt, ...);

Watermark::Watermark(QWidget *parent)
    : QWidget(parent)
    , m_placement(WaterPlacement_BottomRight)
    , m_imageHelper(NULL)
    , m_watermarkFile()
    , m_opacity_percent(100)
    , m_bgImageSize(width(), height())
    , m_scaleToFit(false)
    , m_marginX(20)
    , m_marginY(20)
    , m_mouseOffsetFromTopLeftOfWatermarkX(0)
    , m_mouseOffsetFromTopLeftOfWatermarkY(0)
    , m_watermarkPositionX(0)
    , m_watermarkPositionY(0)
    , m_degreeRotation(0)
    , m_mode(WATERMARKING_MODE_NONE)
    , m_mouseDown(false)
    , m_tileWatermark(false)
    , m_tileMarginAsPercentageOfWatermarkSide(100)
{
    setAcceptDrops(true);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

Watermark::Watermark(const Watermark &eq)
{
    m_watermarkFile = eq.m_watermarkFile;
    m_watermarkText = eq.m_watermarkText;
    m_placement = eq.m_placement;
    m_imageHelper = eq.m_imageHelper;
    m_fontHelper = eq.m_fontHelper;
    m_bgViewSize = eq.m_bgViewSize;
    m_bgImageSize = eq.m_bgImageSize;
    m_mode = eq.m_mode;
    m_mousePositionX = eq.m_mousePositionX;
    m_mousePositionY = eq.m_mousePositionY;
    m_mouseOffsetFromTopLeftOfWatermarkX = eq.m_mouseOffsetFromTopLeftOfWatermarkX;
    m_mouseOffsetFromTopLeftOfWatermarkY = eq.m_mouseOffsetFromTopLeftOfWatermarkY;
    m_watermarkPositionX = eq.m_watermarkPositionX;
    m_watermarkPositionY = eq.m_watermarkPositionY;
    m_opacity_percent = eq.m_opacity_percent;
    m_scaleToFit = eq.m_scaleToFit;
    m_marginX = eq.m_marginX;
    m_marginY = eq.m_marginY;
    m_degreeRotation = eq.m_degreeRotation;
    m_mouseDown = eq.m_mouseDown;
    m_tileWatermark = eq.m_tileWatermark;
    m_tileMarginAsPercentageOfWatermarkSide = eq.m_tileMarginAsPercentageOfWatermarkSide;
    setParent( eq.parentWidget() );
}

const Watermark& Watermark::operator=(const Watermark &eq)
{
    loadFrom(eq);
    return *this;
}

void Watermark::loadFrom(const Watermark& eq)
{
    m_watermarkFile = eq.m_watermarkFile;
    m_watermarkText = eq.m_watermarkText;
    m_placement = eq.m_placement;
    m_imageHelper = eq.m_imageHelper;
    m_fontHelper = eq.m_fontHelper;
    m_bgViewSize = eq.m_bgViewSize;
    m_bgImageSize = eq.m_bgImageSize;
    m_mode = eq.m_mode;
    m_opacity_percent = eq.m_opacity_percent;
    m_scaleToFit = eq.m_scaleToFit;
    m_marginX = eq.m_marginX;
    m_marginY = eq.m_marginY;
    m_degreeRotation = eq.m_degreeRotation;
    m_mouseDown = eq.m_mouseDown;
    m_tileWatermark = eq.m_tileWatermark;
    m_tileMarginAsPercentageOfWatermarkSide = eq.m_tileMarginAsPercentageOfWatermarkSide;
//    setParent( eq.parentWidget() );
}

void Watermark::init(
        ImageHelper * imageHelper,
        FontHelper * fontHelper,
        std::function<void ()> onWatermarkSelected)
{
    m_onWatermarkSelected = onWatermarkSelected;
    m_imageHelper = imageHelper;
    m_fontHelper = fontHelper;
}

void Watermark::saveSettings(QXmlStreamWriter &writer)
{
    writer.writeStartElement("Watermark");
        writer.writeTextElement("File", m_watermarkFile);
        writer.writeTextElement("Text", m_watermarkText);
        writer.writeStartElement("Font");
            writer.writeAttribute("Family", m_fontHelper->getFontFamily());
            writer.writeAttribute("PointSize", QString("%1").arg(m_fontHelper->getFontSizeInPoints()));
            writer.writeAttribute("Style", QString("%1").arg(m_fontHelper->getFontStyle()));
            writer.writeAttribute("Decoration", QString("%1").arg(m_fontHelper->isUnderlined()));
        writer.writeEndElement();
        writer.writeTextElement("Opacity", QString("%1").arg(m_opacity_percent));
        writer.writeTextElement("Angle", QString("%1").arg(m_degreeRotation));
        writer.writeStartElement("Tile");
            writer.writeAttribute("Enabled", QString("%1").arg(m_tileWatermark));
            writer.writeAttribute("Margin", QString("%1").arg(m_tileMarginAsPercentageOfWatermarkSide));
        writer.writeEndElement();
    writer.writeEndElement();
}

void Watermark::loadSettings(QXmlStreamReader  &xml)
{
    if(xml.name() == "File")
    {
        QString fileName = xml.readElementText();
        QFile file_(fileName);
        if(file_.exists())
        {
            m_watermarkFile = fileName;
        }
    }
    else if(xml.name() == "Text")
    {
        m_watermarkText = xml.readElementText();
    }
    else if(xml.name() == "Opacity")
    {
        m_opacity_percent = xml.readElementText().toInt();
    }
    else if(xml.name() == "Angle")
    {
        m_degreeRotation = xml.readElementText().toInt();
    }
    else if(xml.name() == "Tile")
    {
        foreach(const QXmlStreamAttribute &attribute, xml.attributes())
        {
            if(attribute.name() == "Enabled")
            {
                m_tileWatermark = attribute.value().toInt();
            }
            else if(attribute.name() == "Margin")
            {
                m_tileMarginAsPercentageOfWatermarkSide = attribute.value().toInt();
                if(m_tileMarginAsPercentageOfWatermarkSide < 51 || m_tileMarginAsPercentageOfWatermarkSide > 500)
                {
                    m_tileMarginAsPercentageOfWatermarkSide = 100;
                }
            }
        }
    }
}

void Watermark::setScaleToFit(bool fit)
{
    m_scaleToFit = fit;
    cancelCropRect();
}

const QString &Watermark::getWatermarkFileName() const
{
    return m_watermarkFile;
}

const QString &Watermark::getWatermarkText() const
{
    return m_watermarkText;
}

int Watermark::getMarginX(bool whenScaledToFit) const
{
    if(whenScaledToFit)
    {
        if(m_placement == WaterPlacment_Custom)
        {
            return m_mousePositionX - m_mouseOffsetFromTopLeftOfWatermarkX;
        }
        else
        {
            return m_bgImageSize.width() > 0 ?
                        (m_marginX * m_bgViewSize.width()) / m_bgImageSize.width() : 0;
        }
    }
    else
    {
        if(m_placement == WaterPlacment_Custom)
        {
            return m_bgViewSize.width() > 0 ?
                        ((m_mousePositionX - m_mouseOffsetFromTopLeftOfWatermarkX) * m_bgImageSize.width()) /
                    m_bgViewSize.width() : 0;
        }
        else
        {
            return m_marginX;
        }
    }
}

int Watermark::getMarginY(bool whenScaledToFit) const
{
    if(whenScaledToFit)
    {
        if(m_placement == WaterPlacment_Custom)
        {
            return m_mousePositionY - m_mouseOffsetFromTopLeftOfWatermarkY;
        }
        else
        {
            return m_bgImageSize.height() > 0 ?
                        (m_marginY * m_bgViewSize.height()) / m_bgImageSize.height() : 0;
        }
    }
    else
    {
        if(m_placement == WaterPlacment_Custom)
        {
            return m_bgViewSize.height() > 0 ?
                        ((m_mousePositionY - m_mouseOffsetFromTopLeftOfWatermarkY) * m_bgImageSize.height()) /
                    m_bgViewSize.height() : 0;
        }
        else
        {
            return m_marginY;
        }
    }
}

void Watermark::scaleForPreview()
{
    if(m_scaleToFit &&
            !m_bgViewSize.isNull() &&
            m_bgViewSize.isValid() &&
            m_bgImageSize.width() > 0 &&
            m_bgImageSize.height() > 0)
    {
        int newWidth = (m_pixWatermark.width() * m_bgViewSize.width()) / m_bgImageSize.width();
        int newHeight = (m_pixWatermark.height() * m_bgViewSize.height()) / m_bgImageSize.height();
        m_pixWatermarkScaledForBg = m_pixWatermark.
                scaled(QSize(newWidth, newHeight), Qt::KeepAspectRatio);
        setMinimumSize(m_bgViewSize);
    }
    else
    {
        m_pixWatermarkScaledForBg = m_pixWatermark;
        setMinimumSize(m_bgImageSize);
    }
}

void Watermark::drawRectBorder(QPainter &painter, const QRect &dstRect)
{
    painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    QBrush brush(Qt::Dense2Pattern);
    brush.setColor(QColor(0xff, 0xff, 0xff));
    QPen pen(brush, 3, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    painter.setPen(pen);
    painter.drawRect(dstRect);
}

void Watermark::tileWatermark(bool enable)
{
    m_tileWatermark = enable;
    update();
}

void Watermark::setTileMarginPercent(int percent)
{
    m_tileMarginAsPercentageOfWatermarkSide = percent;
    update();
}

void Watermark::paintTiles(QPainter &painter)
{
    int dw = (m_pixWatermarkScaledForBg.width()*m_tileMarginAsPercentageOfWatermarkSide)/100;
    int dh = (m_pixWatermarkScaledForBg.height()*m_tileMarginAsPercentageOfWatermarkSide)/100;
    for(int x=-dw; x<m_bgImageSize.width(); x+=dw)
    {
        for(int y=-dh; y<m_bgImageSize.height(); y+=dh)
        {
            QRect dstRect = QRect(x, y, m_pixWatermarkScaledForBg.width(), m_pixWatermarkScaledForBg.height());
            painter.drawPixmap(dstRect, m_pixWatermarkScaledForBg);
        }
    }
}

void Watermark::paintEvent(QPaintEvent *e)
{
    if(m_pixWatermarkScaledForBg.isNull())
    {
        return;
    }

    QPainter painter;
    painter.begin(this);
    m_watermarkPositionX = 0;
    m_watermarkPositionY = 0;
    m_imageHelper->getWatermarkPosition(m_watermarkPositionX, m_watermarkPositionY,
                              m_scaleToFit ? m_bgViewSize.width() : m_bgImageSize.width(),
                              m_scaleToFit ? m_bgViewSize.height() : m_bgImageSize.height(),
                              m_pixWatermarkScaledForBg.width(), m_pixWatermarkScaledForBg.height(),
                              getMarginX(), getMarginY(),
                              m_placement);
    painter.setOpacity(m_opacity_percent / 100.0f);

    if(m_tileWatermark)
    {
        paintTiles(painter);
    }
    else
    {
        QRect dstRect = QRect(m_watermarkPositionX, m_watermarkPositionY, m_pixWatermarkScaledForBg.width(), m_pixWatermarkScaledForBg.height());
        painter.drawPixmap(dstRect, m_pixWatermarkScaledForBg);

        if(m_mode == WATERMARKING_MODE_DRAG)
        {
            drawRectBorder(painter, dstRect);
        }
        if(m_crop_rect.width() > 1 && m_crop_rect.height() > 1)
        {
            drawRectBorder(painter, m_crop_rect);
        }
    }

    painter.end();
}

void Watermark::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
}

void Watermark::dragMoveEvent(QDragMoveEvent *e)
{

}

void Watermark::onWatermarkChanged()
{
    scaleForPreview();
    update();
}

void Watermark::loadWatermarkDefault()
{
    loadWatermark(m_watermarkFile, m_degreeRotation);
}

void Watermark::loadWatermarkFromFile(QString watermarkFile)
{
    loadWatermark(watermarkFile, m_degreeRotation);
}

void Watermark::loadWatermarkRotated(int degreeRotation)
{
    loadWatermark(m_watermarkFile, degreeRotation);
}

void Watermark::loadWatermark(const QString &watermarkFile, int degreeRotation)
{
    if(watermarkFile.size())
    {
        QString fileName = QDir::toNativeSeparators(watermarkFile);
        if(m_imageHelper->isImageFile(fileName))
        {
            m_pixWatermark.load(fileName);
            if(!m_pixWatermark.isNull())
            {
                if(degreeRotation != 0)
                {
                    //this gives better anti-aliasing than qt method
                    gdImagePtr img = m_imageHelper->getGdImageFromPath(fileName.toUtf8().data(), degreeRotation);
                    if(img)
                    {
                        m_pixWatermark = m_imageHelper->getQPixmapFromGdImage(img);
                        gdFree(img);
                    }
                    //m_pixWatermark = m_pixWatermark.transformed(QTransform().rotate(degreeRotation));
                }
                m_pixWatermarkScaledForBg  = m_pixWatermark;
                m_watermarkFile = fileName;
                m_degreeRotation = degreeRotation;
                onWatermarkChanged();
            }
        }
    }
}

void Watermark::dropEvent(QDropEvent *e)
{
    const QUrl url = e->mimeData()->urls().last();
    QString watermarkFile = url.toLocalFile();
    loadWatermarkFromFile(watermarkFile);
}

void Watermark::setWatermarkPlacement(WaterPlacement placement)
{
    m_placement = placement;
}

void Watermark::setBgImageSize(const QSize &viewSize, const QSize &imageSize)
{
    m_bgViewSize = viewSize;
    m_bgImageSize = imageSize;
    scaleForPreview();
    update();
}

void Watermark::setOpactiyPercent(int transparencyPercent)
{
    m_opacity_percent = 100 - transparencyPercent;
    update();
}

int Watermark::getOpacityPercent() const
{
    return m_opacity_percent;
}

void Watermark::drawWatermarkText()
{
    QString watermarkFile = m_imageHelper->createWatermarkFileFromTextUsingFreeTypeFont(
        m_watermarkText, m_fontHelper);
    loadWatermarkFromFile(watermarkFile);
}

void Watermark::onTypedWatermark(const QString text)
{
    m_watermarkText = text;
    drawWatermarkText();
}

bool Watermark::isMouseInsideWatermarkRect(const QPoint mousePos) const
{
    QRect watermarkRect(m_watermarkPositionX, m_watermarkPositionY,
                        m_pixWatermarkScaledForBg.width(),
                        m_pixWatermarkScaledForBg.height());
    return (watermarkRect.contains(mousePos));
}

void Watermark::dragStartStop(QPoint pos)
{
    if(m_mode==WATERMARKING_MODE_CROP_RECT)
    {
        m_crop_rect.setTopLeft(pos);
        m_crop_rect.setBottomRight(pos);
        setCursor(Qt::CrossCursor);
        update();
    }
    else if(m_mode==WATERMARKING_MODE_DRAG &&
            isMouseInsideWatermarkRect(pos))
    {
        m_placement = WaterPlacment_Custom;
        m_mouseOffsetFromTopLeftOfWatermarkX = pos.rx() - m_watermarkPositionX;
        m_mouseOffsetFromTopLeftOfWatermarkY = pos.ry() - m_watermarkPositionY;
        setCursor(Qt::SizeAllCursor);
        drag(pos);
        update();
    }
}

void Watermark::drag(QPoint pos)
{
    if(!m_onWatermarkSelected) { mylog("m_onWatermarkSelected"); }
    if( m_placement == WaterPlacment_Custom &&
            (m_mode == WATERMARKING_MODE_DRAG) &&
            isMouseInsideWatermarkRect(pos) )
    {
        m_mousePositionX = pos.rx();
        m_mousePositionY = pos.ry();
        onWatermarkChanged();
        m_onWatermarkSelected();
        setCursor(Qt::OpenHandCursor);
    }
    else if(m_mode == WATERMARKING_MODE_CROP_RECT)
    {
        m_crop_rect.setBottomRight(pos);
        emit cropRectUpdated(scaledToFitCropRect());
        setCursor(Qt::CrossCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
}

void Watermark::mousePressEvent(QMouseEvent *e)
{
    switch(e->button())
    {
    case Qt::LeftButton:
        m_mouseDown = true;
        dragStartStop(e->pos());
        break;
    case Qt::RightButton:
        cancelCropRect();
    default:
        break;
    }
    onWatermarkChanged();
}

void Watermark::cancelCropRect()
{
    m_crop_rect.setLeft(0);
    m_crop_rect.setRight(0);
    m_crop_rect.setTop(0);
    m_crop_rect.setBottom(0);
    emit cropRectUpdated(scaledToFitCropRect());
}

QRect Watermark::scaledToFitCropRect()
{
    QRect rect;
    if(m_scaleToFit)
    {
        rect.setLeft( m_crop_rect.left() * m_bgImageSize.width() / m_bgViewSize.width());
        rect.setRight( m_crop_rect.right() * m_bgImageSize.width() / m_bgViewSize.width());
        rect.setTop( m_crop_rect.top() * m_bgImageSize.height() / m_bgViewSize.height());
        rect.setBottom( m_crop_rect.bottom() * m_bgImageSize.height() / m_bgViewSize.height());
    }
    else
    {
        rect = m_crop_rect;
    }
    return rect;
}

void Watermark::mouseReleaseEvent(QMouseEvent *e)
{
    switch(e->button())
    {
    case Qt::LeftButton:
        m_mouseDown = false;
        if(m_mode==WATERMARKING_MODE_DRAG &&  isMouseInsideWatermarkRect(e->pos()))
        {
            dragStartStop(getCenterOfWatermark());
        }
        break;
    default:
        break;
    }
}

void Watermark::mouseMoveEvent(QMouseEvent *e)
{
    if(!m_mouseDown)
    {
        return;
    }
    switch(m_mode)
    {
    case WATERMARKING_MODE_DRAG:
    case WATERMARKING_MODE_CROP_RECT:
        drag(e->pos());
        onWatermarkChanged();
        break;
    }
}

void Watermark::setMode(WATERMARKING_MODE mode)
{
    m_mode = mode;

    if(m_mode==WATERMARKING_MODE_DRAG &&
            isMouseInsideWatermarkRect( getCenterOfWatermark() ))
    {
        dragStartStop( getCenterOfWatermark() );
    }
    else if(m_mode==WATERMARKING_MODE_CROP_RECT)
    {
        setCursor(Qt::CrossCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
}

QPoint Watermark::getCenterOfWatermark() const
{
    return     QPoint (m_watermarkPositionX + m_pixWatermarkScaledForBg.width()/2,
                       m_watermarkPositionY + m_pixWatermarkScaledForBg.height()/2);
}

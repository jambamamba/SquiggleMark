#ifndef WATERMARK_H
#define WATERMARK_H

#include <functional>
#include <QWidget>

#include "MainWindowInterface.h"
#include "WatermarkPlacement.h"

class ImageHelper;
class FontHelper;
class QXmlStreamReader;
class QXmlStreamWriter;

class Watermark : public QWidget
{

    Q_OBJECT
public:
    enum WATERMARKING_MODE
    {
        WATERMARKING_MODE_NONE,
        WATERMARKING_MODE_DRAG,
        WATERMARKING_MODE_SCALE,
        WATERMARKING_MODE_ROTATE,
        WATERMARKING_MODE_CROP_RECT,
        WATERMARKING_NUM_MODES,
    };
    explicit Watermark(QWidget *parent = 0);
    Watermark(const Watermark& eq);
    const Watermark& operator=(const Watermark &eq);

    void loadFrom(const Watermark &eq);
    const QString &getWatermarkFileName() const;
    const QString &getWatermarkText() const;
    void init(ImageHelper * imageHelper, FontHelper * fontHelper, std::function<void ()> onWatermarkSelected);
    void saveSettings(QXmlStreamWriter &writer);
    void loadSettings(QXmlStreamReader  &xml);

    int getDegreeRotation() const { return m_degreeRotation; }
    void setWatermarkPlacement(WaterPlacement placement);
    WaterPlacement getWatermarkPlacement() const { return m_placement; }
    void scaleForPreview();
    void loadWatermarkDefault();
    void loadWatermarkFromFile(QString watermarkFile);
    void loadWatermarkRotated(int degreeRotation);
    void setOpactiyPercent(int transparencyPercent);
    int getOpacityPercent() const;
    void onTypedWatermark(const QString text);
    void setBgImageSize(const QSize &viewSize, const QSize &imageSize);
    void setScaleToFit(bool fit);
    int getMarginX(bool whenScaledToFit) const;
    int getMarginY(bool whenScaledToFit) const;
    int getMarginX() const { return getMarginX(m_scaleToFit); }
    int getMarginY() const { return getMarginY(m_scaleToFit); }
    int getWatermarkPositionX() const { return m_watermarkPositionX; }
    int getWatermarkPositionY() const { return m_watermarkPositionY; }
    void drawWatermarkText();
    void onWatermarkChanged();
    bool isDragging() const { return m_mode == WATERMARKING_MODE_DRAG; }
    void setMode(WATERMARKING_MODE mode);
    void tileWatermark(bool enable);
    void setTileMarginPercent(int percent);
    bool isTilingEnabled() const { return m_tileWatermark; }
    int getTileMarginPercent() const { return m_tileMarginAsPercentageOfWatermarkSide; }
signals:
    void cropRectUpdated(const QRect&);

public slots:

protected:
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);
    void dragMoveEvent(QDragMoveEvent *e);
    void dropEvent(QDropEvent *e);
    void paintEvent(QPaintEvent *e);

private:
    QPixmap m_pixWatermark;
    QPixmap m_pixWatermarkScaledForBg;
    QString m_watermarkFile;
    QString m_watermarkText;
    WaterPlacement m_placement;
    ImageHelper *m_imageHelper;
    FontHelper *m_fontHelper;
    QSize m_bgViewSize;
    QSize m_bgImageSize;
    int m_mode;
    int m_mousePositionX;
    int m_mousePositionY;
    int m_mouseOffsetFromTopLeftOfWatermarkX;
    int m_mouseOffsetFromTopLeftOfWatermarkY;
    int m_watermarkPositionX;
    int m_watermarkPositionY;
    int m_opacity_percent;
    bool m_scaleToFit;
    int m_marginX;
    int m_marginY;
    int m_degreeRotation;
    bool m_mouseDown;
    bool m_tileWatermark;
    int m_tileMarginAsPercentageOfWatermarkSide;
    QRect m_crop_rect;
    std::function<void ()> m_onWatermarkSelected;

    bool isMouseInsideWatermarkRect(const QPoint mousePos) const;
    void drawRectBorder(QPainter &painter, const QRect &dstRect);
    void loadWatermark(const QString &watermarkFile, int degreeRotation);
    void drag(QPoint pos);
    void dragStartStop(QPoint pos);
    QPoint getCenterOfWatermark() const;
    void paintTiles(QPainter &painter);
    QRect scaledToFitCropRect();
    void cancelCropRect();
};

#endif // WATERMARK_H

#ifndef IMAGEHELPER_H
#define IMAGEHELPER_H

#include <functional>
#include <gd.h>
#include <QFuture>
#include <QString>
//#include "../libs/libwebp/src/webp/encode.h"

#include "SupportedFileTypes.h"
#include "WatermarkPlacement.h"
#include "MainWindowInterface.h"

class FontHelper;
class QFileInfo;

class ImageHelper
{
public:
    enum IMG_TYPE
    {
        IMG_TYPE_UNKNOWN,
        IMG_TYPE_JPG,
        IMG_TYPE_PNG,
        IMG_TYPE_WEBP
    };
    static const char SaveFileName[];

    ImageHelper(const QString &app_name, SupportedFileTypes *supportedFileTypes);
    virtual ~ImageHelper() {}

    bool isPng(const char* fileName) const;
    bool isJpeg(const char* fileName) const;
    bool isSvg(const char* fileName) const;
    bool isImageFile(const QString &fileName) const;
    bool isVideoFile(const QString &fileName) const;
    void gdImageWrite(gdImagePtr dst, FILE* fpOut, IMG_TYPE imgType) const;
    void getWatermarkPosition(int &dstx, int &dsty,
                              int bgImageWidth, int bgImageHeight,
                              int watermarkWidth, int watermarkHeight,
                              int marginX, int marginY,
                              const WaterPlacement watermarkPlacement) const;
    QList<QFuture<QString> > resizeAndWatermarkAsync(QObject *mainWnd, const QString fileName, const QString outputDir, QList<WatermarkOptions*> watermarkOptions, Device device);
    QString createWatermarkFileFromTextUsingFreeTypeFont(const QString& text, const FontHelper *fontHelper) const;
    gdImagePtr applyTransparencyToImage(gdImagePtr img, int opacityPercent) const;
    QImage getQImageFromSvg(const QString &src) const;
    gdImagePtr getGdImageFromQImage(const QImage &qimg) const;
    QPixmap getQPixmapFromGdImage(const gdImagePtr img) const;
    gdImagePtr getGdImageFromPath(const char* path, int degreeRotation = 0) const;
    gdImagePtr getGdRotatedImage(gdImagePtr img, int degreeRotation) const;
    void start();
    void stop();
    void setQualityGetters(std::function<int()>getJpegQuality,
                           std::function<int()> getPngQuality,
                           std::function<int()> getVideoQuality);

private:
    bool readFileHeader(const char* filename, char* header, int headerSize, QString &err) const;
    bool saveImgToFile(const gdImagePtr img, const QString &fileName, IMG_TYPE imgType) const;
    QString saveImgToFile(const gdImagePtr img, const QString &fileName) const;
    gdImagePtr createTrueColorImageWithTransparentBackground(int width, int height) const;
    bool getImageRectFromText(const QString& text, char *fontname, const double ptsize, const double angle, int brect[]) const;
    void drawUnderline(gdImagePtr img, int height, int underlineRectHeight, int color, int *brect) const;
    gdImagePtr resizeAndWatermarkImage(const gdImagePtr img, const WatermarkOptions *options) const;
    gdImagePtr resizeImage(gdImagePtr img, const Device &device) const;
    gdImagePtr rotateImage(Device device, const QString inFile, gdImagePtr img) const;
    gdImagePtr cropImage(const QRect &crop_rect, const QString inFile, gdImagePtr img) const;
    QString resizeAndWatermark(QObject *mainWnd, const QString inFile,
                              const QString outFile,
                            QList<WatermarkOptions*> watermarkOptions, Device device) const;
    QString resizeAndWatermarkImage(QObject *mainWnd, const QString inFile,
                            const QString outFile,
                          QList<WatermarkOptions*> watermarkOptions, Device device) const;
    QString resizeAndWatermarkVideo(QObject *mainWnd, const QString inFile,
                            const QString outFile,
                          QList<WatermarkOptions*> watermarkOptions, Device device) const;
    QString getOutFilePathAndMakeDir(const QFileInfo &fileInfo, QString outputDir, const Device &device, const WatermarkOptions *options) const;
    QSize getImageSizeForDevice(gdImagePtr img, const Device &device) const;
    gdImagePtr gdPngFromSvg(const char* path) const;
    Device getDeviceWithCorrectWidthHeight(ImageScaling scaling, const Device &device, const QString &imgFile) const;
    WaterPlacement getTrialImagePlacement(const WaterPlacement watermarkPlacement) const;
    gdImagePtr gdGetCroppedImage(gdImagePtr src, const Device &device, bool keepTransparency) const;
    gdImagePtr createGdImage(int width, int height, bool keepTransparency) const;
    gdImagePtr gdGetScaledImage(gdImagePtr src, int width, int height, bool keepTransparency, bool resizeCanvasOnly) const;
    gdImagePtr applyTrialImageWatermark(const WatermarkOptions *options, const gdImagePtr img) const;
    QString getTmpFileName() const;
    gdImagePtr applyWatermark(QList<WatermarkOptions*> watermarkOptions, gdImagePtr img) const;
    bool hasWatermarkToApply(QList<WatermarkOptions *> watermarkOptions, gdImagePtr img) const;
//    void convertToWebP(const QString inFile, const QString outFile) const;
    static void gdError(int errcode, const char *errmsg, va_list);
    QString layerFilePath(const QString &file) const;

    std::function<int()>m_getJpegQuality;
    std::function<int()>m_getPngQuality;
    std::function<int()>m_getVideoQuality;
    SupportedFileTypes *m_supportedFileTypes;
    bool m_stop;
    const QString m_app_name;
};

#endif // IMAGEHELPER_H

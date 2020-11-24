#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

///#include <dirent.h>
extern "C"{
#include <gdfontg.h>
#include <gd.h>
}
#include <QFile>
#include <QFuture>
#include <QImage>
#include <QPainter>
#include <QStringList>
#include <QtConcurrent/QtConcurrent>
#include <QtSvg/QSvgRenderer>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include "FontHelper.h"
#include "FFMpeg.h"
#include "ImageHelper.h"
#include "VideoPlayer.h"
#include "SupportedFileTypes.h"

extern "C" void mylog(const char *fmt, ...);

namespace {
    const int WebPQuality = 40;// %
    const int LOWER_LEFT_X = 0;
    const int LOWER_LEFT_Y = 1;
    const int LOWER_RIGHT_X = 2;
    const int LOWER_RIGHT_Y = 3;
    const int UPPER_RIGHT_X = 4;
    const int UPPER_RIGHT_Y = 5;
    const int UPPER_LEFT_X = 6;
    const int UPPER_LEFT_Y = 7;

}//namespace

const char ImageHelper::SaveFileName[]="~TextMark.png";

ImageHelper::ImageHelper(const QString &app_name, SupportedFileTypes *supportedFileTypes)
    : m_app_name(app_name)
{
    m_supportedFileTypes = supportedFileTypes;
    gdSetErrorMethod(ImageHelper::gdError);
}

void ImageHelper::gdError(int errcode, const char *format, va_list args)
{
    mylog(format, args);
}

bool ImageHelper::readFileHeader(const char* filename, char* header, int headerSize, QString& err) const
{
    bool ret = false;
    struct stat stat_buf;
    FILE* in = 0;
    if(!header)
        goto Exit;
    if(headerSize<1)
        goto Exit;
    if(!filename || !*filename)
        goto Exit;
    in = fopen(filename, "rb");
    if (!in)
    {
        err = QString("ERR: Could not open input file \"%1\"").arg(filename);
        goto Exit;
    }
    if (fstat(fileno(in), &stat_buf) != 0)
    {
        err = QString("ERR: Could not get stat on input file \"%1\"").arg(filename);
        goto Exit;
    }
    if (fread(header, 1, headerSize, in)
            != headerSize)
    {
        err = QString("ERR: Could not read header of the input file \"%1\"").arg(filename);
        goto Exit;
    }
    ret = true;
Exit:
    if(in)
    {
        fclose(in);
    }
    return ret;
}

bool ImageHelper::isSvg(const char* fileName) const
{
    return strcasecmp(&fileName[strlen(fileName)-4], ".svg")==0;
}
bool ImageHelper::isPng(const char* fileName) const
{
    //return strcasecmp(&fileName[strlen(fileName)-4], ".png")==0;
    const unsigned char pngHeader[8] = {0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a};
    char header[8];
    QString err;
    bool _read = readFileHeader(fileName, header, sizeof(header), err);
    if(err.size())
    {
        mylog("isPng %s err: %s", fileName, err.toUtf8().constData());
    }
    return (_read && memcmp(header, pngHeader, sizeof(pngHeader))==0);
}

bool ImageHelper::isJpeg(const char* fileName) const
{
    //return strcasecmp(&fileName[strlen(fileName)-4], ".jpg")==0;
    char header[2];
    QString err;
    const unsigned char jpgHeader[2] = {0xff,0xd8};
    bool _read = readFileHeader(fileName, header, sizeof(header), err);
    if(err.size())
    {
        mylog("isJpeg %s err: %s", fileName, err.toUtf8().constData());
    }
    return (_read && memcmp(header, jpgHeader, sizeof(jpgHeader))==0);
}

bool ImageHelper::isImageFile(const QString &fileName) const
{
    return isPng(fileName.toUtf8().data()) ||
            isJpeg(fileName.toUtf8().data()) ||
            isSvg(fileName.toUtf8().data() );
}

bool ImageHelper::isVideoFile(const QString &fileName) const
{
    QStringList filetypes = m_supportedFileTypes->getVideoTypes();
    bool ret = false;
    for(int i = 0; i<filetypes.size() && !ret; ++i)
    {
        ret = strcasecmp(fileName.right(4).toUtf8().data(),
                filetypes[i].right(filetypes[i].size()-1).toUtf8().data())==0;
    }
    return ret;
}


QImage ImageHelper::getQImageFromSvg(const QString &src) const
{
    QSvgRenderer svg;
    svg.load(src);
    if(svg.isValid())
    {
        QImage image(svg.viewBox().width(), svg.viewBox().height(), QImage::Format_ARGB32);
        QPainter painter(&image);
        svg.render(&painter);
        return image;
    }

    return QImage();
}

gdImagePtr ImageHelper::gdPngFromSvg( const char* path) const
{
    QImage qimg = getQImageFromSvg(path);
    if(qimg.isNull())
    {
        mylog("failed to open svg file '%s'", path ? path : "");
        return 0;
    }
    else
    {
        return getGdImageFromQImage(qimg);
    }
}

gdImagePtr ImageHelper::getGdImageFromQImage(const QImage &qimg) const
{
    if(qimg.isNull())
    {
        return 0;
    }

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    qimg.save(&buffer, "PNG");
    return gdImageCreateFromPngPtr(buffer.size(), (void*)buffer.data().data());
}

QPixmap ImageHelper::getQPixmapFromGdImage(const gdImagePtr img) const
{
    if(!img)
    {
        return QPixmap();
    }
    FILE* tmp = fopen("~.png", "wb");
    if(tmp)
    {
        gdImagePng(img, tmp);
        fclose(tmp);
        return QPixmap("~.png");
    }

    return QPixmap();
}

gdImagePtr ImageHelper::getGdRotatedImage(gdImagePtr img, int degreeRotation) const
{
//    gdImagePtr rotated = gdImageRotateBicubicFixed(img, degreeRotation,
//                                                   gdImageColorAllocateAlpha(img, 0, 0, 0, gdAlphaTransparent));

    float angle = 0;
    switch(degreeRotation)
    {
    case 90:
        angle = 89.90f;
        break;
    case -90:
        angle = -89.90f;
        break;
    case 180:
        angle = 179.90f;
        break;
    case -180:
        angle = -179.90f;
        break;
    default:
        angle = degreeRotation;
    }

    gdImagePtr rotated = gdImageRotateInterpolated(img, angle,
                                                   gdImageColorAllocateAlpha(img, 0, 0, 0, gdAlphaTransparent));
    if(!rotated)
    {
        mylog("gdImageRotateInterpolated in getGdImageFromPath failed");
    }
    gdImageDestroy(img);
    return rotated;
}

gdImagePtr ImageHelper::getGdImageFromPath(const char* path, int degreeRotation) const
{
    FILE* fp  = 0;
    gdImagePtr img = 0;
    fp = fopen(path, "rb");
    if(!fp)
    {
        mylog("getGdImageFromPath Failed to open %s, errno: %i", path ? path : "", errno);
        goto Exit;
    }
    if(isPng(path))
    {
        img = gdImageCreateFromPng(fp);
    }
    else if(isJpeg(path))
    {
//        exif_imagetype
        img = gdImageCreateFromJpeg(fp);
    }
    else if(isSvg(path))
    {
        img = gdPngFromSvg(path);
    }
    if(img)
    {
        gdImageSaveAlpha(img, 1);
        gdImageAlphaBlending(img, 1);

        if(degreeRotation != 0)
        {
            img = getGdRotatedImage(img, degreeRotation);
        }
    }
Exit:
    if(fp)
    {
        fclose(fp);
    }
    return img;
}

void ImageHelper::gdImageWrite(gdImagePtr dst, FILE* fpOut, IMG_TYPE imgType) const
{
    if(IMG_TYPE_PNG == imgType)
    {
        if(!m_getPngQuality) { mylog("m_getPngQuality"); }
        gdImagePngEx(dst, fpOut, m_getPngQuality());
    }
    else if(IMG_TYPE_JPG == imgType)
    {
        if(!m_getJpegQuality) { mylog("m_getJpegQuality"); }
        gdImageJpeg(dst, fpOut, m_getJpegQuality());
    }
//    else if(IMG_TYPE_WEBP == imgType )
//    {
//        gdImageWebpEx(dst, fpOut, WebPQuality);
//    }
//    else if(IMG_TYPE_WEBP == imgType )
//    {
//        gdImageWebpEx(dst, fpOut, WebPQuality);
//    }
}


gdImagePtr ImageHelper::applyTransparencyToImage(gdImagePtr img, int opacityPercent) const
{
    for(int i = 0; i<img->sx; i++)
    {
        for(int j = 0; j<img->sy; j++)
        {
            int pix = gdImageGetTrueColorPixel(img, i, j);
            int alpha = (((pix & 0xff000000) >> 24));
            pix &= 0x00ffffff;
            alpha = gdAlphaMax - (((gdAlphaMax - alpha) * opacityPercent) / 100);
            alpha <<= 24;
            pix |= alpha;
            gdImageSetPixel(img, i, j, pix);
        }
    }
    return img;
}

WaterPlacement ImageHelper::getTrialImagePlacement(const WaterPlacement watermarkPlacement) const
{
    WaterPlacement trialimgPlacement;
    switch(watermarkPlacement)
    {
    case WaterPlacement_Centered:
    case WaterPlacement_CenterLeft:
    case WaterPlacement_CenterTop:
    case WaterPlacement_CenterBottom:
    case WaterPlacement_CenterRight:
        trialimgPlacement = WaterPlacement_TopLeft;
        break;
    case WaterPlacement_TopLeft:
        trialimgPlacement = WaterPlacement_BottomRight;
        break;
    case WaterPlacement_TopRight:
        trialimgPlacement = WaterPlacement_BottomLeft;
        break;
    case WaterPlacement_BottomLeft:
        trialimgPlacement = WaterPlacement_TopRight;
        break;
    case WaterPlacement_BottomRight:
        trialimgPlacement = WaterPlacement_TopLeft;
        break;
    default:
    case WaterPlacment_Custom:
        trialimgPlacement = WaterPlacment_Custom;
        break;
    }
    return trialimgPlacement;
}

void ImageHelper::getWatermarkPosition(int &dstx, int &dsty,
                          int bgImageWidth, int bgImageHeight,
                          int watermarkWidth, int watermarkHeight,
                          int marginX, int marginY,
                          const WaterPlacement watermarkPlacement) const
{
    switch(watermarkPlacement)
    {
    case WaterPlacement_TopLeft:
        dstx = marginX;
        dsty = marginY;
        break;
    case WaterPlacement_TopRight:
        dstx = bgImageWidth - watermarkWidth - marginX;
        dsty = marginY;
        break;
    case WaterPlacement_BottomLeft:
        dstx = marginX;
        dsty = bgImageHeight - watermarkHeight - marginY;
        break;
    case WaterPlacement_BottomRight:
        dstx = bgImageWidth - watermarkWidth - marginX;
        dsty = bgImageHeight - watermarkHeight - marginY;
        break;
    case WaterPlacment_Custom:
        dstx = marginX;
        dsty = marginY;
        break;
    case WaterPlacement_Centered:
        dstx = bgImageWidth/2 - watermarkWidth/2;
        dsty = bgImageHeight/2 - watermarkHeight/2;
        break;
    case WaterPlacement_CenterLeft:
        dstx = marginX;
        dsty = bgImageHeight/2 - watermarkHeight/2;
        break;
    case WaterPlacement_CenterTop:
        dstx = bgImageWidth/2 - watermarkWidth/2;
        dsty = marginY;
        break;
    case WaterPlacement_CenterBottom:
        dstx = bgImageWidth/2 - watermarkWidth/2;
        dsty = bgImageHeight - watermarkHeight - marginY;
        break;
    case WaterPlacement_CenterRight:
        dstx = bgImageWidth - watermarkWidth - marginX;
        dsty = bgImageHeight/2 - watermarkHeight/2;
        break;
    }
}

gdImagePtr ImageHelper::applyTrialImageWatermark(const WatermarkOptions *options, const gdImagePtr img) const
{
    WaterPlacement placement = WaterPlacement_Centered;//getTrialImagePlacement(options->m_placement);

    int width = img->sx*50/100;
    int height = (options->m_trialimg->sy * width) / options->m_trialimg->sx;
    gdImagePtr trialimg = gdGetScaledImage(options->m_trialimg, width, height, true, false);
    if(!trialimg)
    {
        gdImageDestroy(img);
        return 0;
    }

    int dstx, dsty;
    getWatermarkPosition(dstx,
                         dsty,
                         img->sx,
                         img->sy,
                         trialimg->sx,
                         trialimg->sy,
                         options->m_watermarkMarginX, options->m_watermarkMarginY,
                         placement);
    gdImageCopyResampled (img, trialimg,
                          dstx, dsty,
                          0, 0,
                          trialimg->sx, trialimg->sy,
                          trialimg->sx, trialimg->sy
                          );

    if(trialimg != options->m_trialimg)
    {
        gdImageDestroy(trialimg);
    }
    return img;
}

gdImagePtr ImageHelper::resizeAndWatermarkImage(const gdImagePtr img, const WatermarkOptions *options) const
{
    if(options->m_watermarkimg)
    {
        if(options->m_tileWatermark)
        {
            int dw = (options->m_watermarkimg->sx*options->m_tileMarginAsPercentageOfWatermarkSide)/100;
            int dh = (options->m_watermarkimg->sy*options->m_tileMarginAsPercentageOfWatermarkSide)/100;
            for(int x=-dw; x<img->sx; x+=dw)
            {
                for(int y=-dh; y<img->sy; y+=dh)
                {
                    gdImageCopyResampled (
                                img, options->m_watermarkimg,
                                x, y,
                                0, 0,
                                options->m_watermarkimg->sx,
                                options->m_watermarkimg->sy,
                                options->m_watermarkimg->sx,
                                options->m_watermarkimg->sy
                                );
                }
            }
        }
        else
        {
            int dstx, dsty;
            getWatermarkPosition(dstx,
                                 dsty,
                                 img->sx,
                                 img->sy,
                                 options->m_watermarkimg->sx,
                                 options->m_watermarkimg->sy,
                                 options->m_watermarkMarginX, options->m_watermarkMarginY,
                                 options->m_placement);

            gdImageCopyResampled (
                        img, options->m_watermarkimg,
                        dstx, dsty,
                        0, 0,
                        options->m_watermarkimg->sx,
                        options->m_watermarkimg->sy,
                        options->m_watermarkimg->sx,
                        options->m_watermarkimg->sy
                        );
        }
    }

    return img;
}

QSize ImageHelper::getImageSizeForDevice(gdImagePtr img, const Device &device) const
{
    int w = img->sx;
    int h = img->sy;
    QSize size;
    switch(device.m_scaling)
    {
        case IMG_SCALE_WIDTH_AND_CROP_HEIGHT:
        {
            size.setWidth( img->sx * device.m_widthPixels / device.m_referenceImageSize );
            size.setHeight ( img->sy * device.m_widthPixels / device.m_referenceImageSize );
            mylog("IMG_SCALE_WIDTH_AND_CROP_HEIGHT2 resizeImage %ix%i -> %ix%i", w, h, size.width(), size.height());
            return size;
        }
        case IMG_SCALE_HEIGHT_AND_CROP_WIDTH:
        {
            size.setWidth( img->sx * device.m_heightPixels / device.m_referenceImageSize );
            size.setHeight ( img->sy * device.m_heightPixels / device.m_referenceImageSize );
            mylog("IMG_SCALE_HEIGHT_AND_CROP_WIDTHb2 resizeImage %ix%i -> %ix%i", w, h, size.width(), size.height());
            return size;
        }
        case IMG_SCALING_FIXED_WIDTH_AND_HEIGHT:
        {
            QSize size( device.m_widthPixels, device.m_heightPixels );
            mylog("IMG_SCALING_FIXED_WIDTH_AND_HEIGHT resizeImage %ix%i -> %ix%i", w, h, size.width(), size.height());
            return size;
        }
        case IMG_SCALING_FIXED_WIDTH:
        {
            QSize size( device.m_widthPixels, (device.m_widthPixels * img->sy) / img->sx );
            mylog("IMG_SCALING_FIXED_WIDTH resizeImage %ix%i -> %ix%i", w, h, size.width(), size.height());
            return size;
        }
        case IMG_SCALING_FIXED_HEIGHT:
        {
            QSize size( (device.m_heightPixels * img->sx) / img->sy, device.m_heightPixels );
            mylog("IMG_SCALING_FIXED_HEIGHT resizeImage %ix%i -> %ix%i", w, h, size.width(), size.height());
            return size;
        }
        case IMG_SCALING_NONE:
        {
            QSize size( device.m_widthPixels, device.m_heightPixels );
            mylog("IMG_SCALING_NONE resizeImage %ix%i -> %ix%i", w, h, size.width(), size.height());
            return size;
        }
        default:
        {
            return QSize(0, 0);
        }
    }
}

gdImagePtr ImageHelper::createGdImage(int width, int height, bool keepTransparency) const
{
    return keepTransparency ?
            createTrueColorImageWithTransparentBackground(width, height) :
            gdImageCreateTrueColor(width, height);
}

gdImagePtr ImageHelper::gdGetCroppedImage(gdImagePtr src, const Device &device, bool keepTransparency) const
{
    if(src->sx <= device.m_widthPixels && src->sy <= device.m_heightPixels)
    {
        return gdImageClone(src);
    }
    switch(device.m_scaling)
    {
    case IMG_SCALE_WIDTH_AND_CROP_HEIGHT:
        if(src->sy <= device.m_heightPixels)
        {
            return gdImageClone(src);
        }
        break;
    case IMG_SCALE_HEIGHT_AND_CROP_WIDTH:
        if(src->sx <= device.m_widthPixels)
        {
            return gdImageClone(src);
        }
        break;
    default:
        break;
    }

    gdImagePtr dst = createGdImage(device.m_widthPixels, device.m_heightPixels, keepTransparency);
    if(dst)
    {
        int sx0;
        int sy0;
        switch(device.m_scaling)
        {
        case IMG_SCALE_WIDTH_AND_CROP_HEIGHT:
            sy0 = src->sy/2 - device.m_heightPixels/2;
            gdImageCopyResampled(dst, src,
                                 0, 0, //dst x, y
                                 0, sy0, //src x, y
                                 dst->sx, dst->sy, //dst width, height
                                 src->sx, device.m_heightPixels //src width, height
                                 );
            if(sy0 < 0)
            {
                gdImageAlphaBlending(dst, 0);
                gdImageFilledRectangle(dst, 0, 0, dst->sx,
                                       -sy0,
                                       gdImageColorAllocateAlpha(dst, 0, 0, 0, gdAlphaMax));
                gdImageFilledRectangle(dst, 0,
                                       -sy0 + src->sy, dst->sx,
                                       dst->sy,
                                       gdImageColorAllocateAlpha(dst, 0, 0, 0, gdAlphaMax));
            }
            break;
        case IMG_SCALE_HEIGHT_AND_CROP_WIDTH:
            sx0 = src->sx/2 - device.m_widthPixels/2;
            gdImageCopyResampled(dst, src,
                                 0, 0, //dst x, y
                                 sx0, 0,//src x, y
                                 dst->sx, dst->sy, //dst width, height
                                 device.m_widthPixels, src->sy //src width, height
                                 );
            if(sx0 < 0)//this is untested, check with case above that is tested
            {
                gdImageAlphaBlending(dst, 0);
                gdImageFilledRectangle(dst, 0, 0, -sx0,
                                       dst->sy,
                                       gdImageColorAllocateAlpha(dst, 0, 0, 0, gdAlphaMax));
                gdImageFilledRectangle(dst, -sx0 + src->sx,
                                       0, dst->sx,
                                       dst->sy,
                                       gdImageColorAllocateAlpha(dst, 0, 0, 0, gdAlphaMax));
            }
            break;
        default:
            break;
        }
        return dst;
    }
    return gdImageClone(src);
}

gdImagePtr ImageHelper::gdGetScaledImage(gdImagePtr src, int width, int height,
                                         bool keepTransparency,
                                         bool resizeCanvasOnly) const
{
    gdImagePtr dst = createGdImage(width, height, keepTransparency);
    if(dst)
    {
        if(!keepTransparency)
        {
            gdImageFill(dst, 0, 0, gdImageColorAllocate(dst, 255, 255, 255));
        }

        if(resizeCanvasOnly)
        {
            int dstx = (width - src->sx)/2;
            int dsty = (height- src->sy)/2;
            gdImageCopy(dst, src,
                             dstx, dsty, //dst x, y
                             0, 0, //src x, y
                             src->sx, src->sy //src width, height
                             );
        }
        else
        {
            gdImageCopyResampled(dst, src,
                             0, 0, //dst x, y
                             0, 0, //src x, y
                             width, height, //dst width, height
                             src->sx, src->sy //src width, height
                             );
        }
        return dst;
    }
    return gdImageClone(src);
}

gdImagePtr ImageHelper::resizeImage(gdImagePtr img, const Device &device) const
{
    QSize size = getImageSizeForDevice(img, device);

    if(size.width()>0 && size.height()>0
             //&& !(size.width() == img->sx && size.height() == img->sy)
            )
    {
        gdImagePtr scaled = gdGetScaledImage(img, size.width(), size.height(),
                               IMG_ACTION_KEEP_TRANSPARENCY & device.m_action,
                               IMG_ACTION_RESIZE_CANVAS & device.m_action);

        if(scaled)
        {
            gdImagePtr cropped = gdGetCroppedImage(scaled, device, IMG_ACTION_KEEP_TRANSPARENCY & device.m_action);
            if(!cropped)
            {
                return scaled;
            }
            gdImageDestroy(scaled);

            if(IMG_ACTION_KEEP_TRANSPARENCY & device.m_action)
            {
                gdImageSaveAlpha(cropped, 1);
                gdImageAlphaBlending(cropped, 1);
            }
            return cropped;
        }
    }
    return gdImageClone(img);
}

QString ImageHelper::getOutFilePathAndMakeDir(
        const QFileInfo &fileInfo,
        QString outputDir,
        const Device &device,
        const WatermarkOptions *options) const
{
    QFileInfo dirInfo(fileInfo.
                      absoluteDir().
                      absolutePath());
    if(!outputDir.size())
    {
        outputDir.append(fileInfo.
                  absoluteDir().
                  absolutePath());
        outputDir.append("/").
                append(dirInfo.baseName()).
                append("~");
    }
    QDir dir(outputDir);
    if(!dir.exists())
    {
        dir.mkdir(dir.absolutePath());
    }
    if(device.m_action & IMG_ACTION_RESIZE_FOR_IOS_DEVICE)
    {
        outputDir.append(QString("/%1 %2x%3%4").
                     arg(device.m_name).
                     arg(device.m_widthPixels).
                     arg(device.m_heightPixels).
                     arg(device.m_pixelsPerPoint > 1 ?
                         QString(" @%1x").arg(device.m_pixelsPerPoint) : "")
                    );
    }
    dir = QDir(outputDir);
    if(!dir.exists())
    {
        dir.mkdir(dir.absolutePath());
    }
    QString outFile = QString(dir.absolutePath()).append("/");
    if(!(device.m_action & IMG_ACTION_DONT_PREPEND_FILENAME))
    {
        outFile.append( fileInfo.baseName() );
    }
    outFile.append(device.m_suffix);
    outFile.append(".");

    if(!isVideoFile(fileInfo.absoluteFilePath()))
    {
        switch(options->m_outputImageFormat)
        {
        default:
        case OutputImageFormat_Original:
            if(device.m_action & IMG_ACTION_OUTPUT_WEBP)
            {
                outFile.append("webp");
            }
            else if(fileInfo.suffix().compare("svg", Qt::CaseInsensitive) == 0)
            {
                outFile.append("png");
            }
            else
            {
                outFile.append(fileInfo.suffix());
            }
            break;
        case OutputImageFormat_Jpg:
            outFile.append("jpg");
            break;
        case OutputImageFormat_Png:
            outFile.append("png");
            break;
        }
    }
    else
    {
        switch(options->m_outputVideoFormat)
        {
        default:
        case OutputVideoFormat_Original:
            outFile.append(fileInfo.suffix());
            break;
        case OutputVideoFormat_Mp4:
            outFile.append("mp4");
            break;
        case OutputVideoFormat_Ogg:
            outFile.append("ogg");
            break;
        }
    }
    return outFile;
}

gdImagePtr ImageHelper::rotateImage(Device device, const QString inFile, gdImagePtr img) const
{
    gdImagePtr rotatedImg = gdImageRotateInterpolated(img, device.m_rotationAngle, gdAlphaMax);
    if(!rotatedImg)
    {
        mylog("failed to rotate image '%s'. check that you have enough memory and hard drive space.", inFile.size() ? inFile.toUtf8().data() : "");
        return img;
    }
    return rotatedImg;
}


gdImagePtr ImageHelper::cropImage(const QRect &crop_rect, const QString inFile, gdImagePtr img) const
{
    gdRect gd_crop_rect;
    gd_crop_rect.x = crop_rect.left();
    gd_crop_rect.y = crop_rect.top();
    gd_crop_rect.width = crop_rect.width();
    gd_crop_rect.height = crop_rect.height();
    gdImagePtr croppedImg = gdImageCrop(img, &gd_crop_rect);
    if(!croppedImg)
    {
        mylog("failed to crop image '%s'. check that you have enough memory and hard drive space.", inFile.size() ? inFile.toUtf8().data() : "");
        return img;
    }
    return croppedImg;
}

//void ImageHelper::convertToWebP(const QString inFile, const QString outFile) const
//{
//    //cwebp -q 70 picture_with_alpha.png -o picture_with_alpha.webp
//    QString program("C:/work/libs/libwebp-0.4.1-windows-x64/bin/cwebp.exe");
//    QStringList args;
//    args << "-q" << "50" << inFile << "-o" << outFile;
//    QProcess *myProcess = new QProcess;
//    myProcess->start(program, args);
//    myProcess->waitForFinished();
//    QString strOut = myProcess->readAllStandardOutput();
//    delete myProcess;
//}

gdImagePtr ImageHelper::applyWatermark(QList<WatermarkOptions *> watermarkOptions, gdImagePtr img) const
{
    foreach(const WatermarkOptions *options, watermarkOptions)
    {
        if(options->m_watermarkimg)
        {
            img = resizeAndWatermarkImage(img, options);
        }
    }

    return img;
}

bool ImageHelper::hasWatermarkToApply(QList<WatermarkOptions *> watermarkOptions, gdImagePtr img) const
{
    foreach(const WatermarkOptions *options, watermarkOptions)
    {
        if(options->m_watermarkimg)
        {
            return true;
        }
    }

    return false;
}

QString ImageHelper::getTmpFileName() const
{
    struct timespec now;
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  now.tv_sec = mts.tv_sec;
  now.tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_MONOTONIC, &now);
#endif
    int ms = now.tv_sec*1000 + now.tv_nsec/1000000;
    QString tmp = QString("~tmp%1%2%3%4%5.png").
            arg(ms).
            arg(QChar('a' + char(qrand() % ('z' - 'a')))).
            arg(QChar('a' + char(qrand() % ('z' - 'a')))).
            arg(QChar('a' + char(qrand() % ('z' - 'a')))).
            arg(QChar('a' + char(qrand() % ('z' - 'a'))));

    return tmp;
}

QString ImageHelper::resizeAndWatermarkVideo(
        QObject *mainWnd,
        const QString inFile,
        const QString outFile,
        QList<WatermarkOptions*> watermarkOptions,
        Device device) const
{
    gdImagePtr img;
    bool hasWatermark = false;
    QString savedFile;
    qDebug() << inFile;

    VideoPlayer moviePlayer;
    moviePlayer.open(inFile.toUtf8().data());
    uint32_t width=0, height=0;
    int ret = moviePlayer.getWidthHeight(width, height);
    int numFrames = moviePlayer.getNumFrames();
    moviePlayer.close();

    if(ret!=0)
    {
        mylog("failed to watermark video '%s'. Could not determine video frame width and height.", inFile.size() ? inFile.toUtf8().data() : "");
        goto Exit;
    }
    if(width==0 || height==0)
    {
        mylog("failed to watermark video '%s'. Could not determine video frame width or height.", inFile.size() ? inFile.toUtf8().data() : "");
        goto Exit;
    }
//    if(numFrames==0)
//    {
//        mylog("failed to watermark video '%s'. Could not determine number of frames in video.", inFile.size() ? inFile.toUtf8().data() : "");
//        goto Exit;
//    }
    img = createTrueColorImageWithTransparentBackground(width, height);
    if(!img)
    {
        mylog("failed to watermark video '%s'. Could not create transparent watermark image.", inFile.size() ? inFile.toUtf8().data() : "");
        goto Exit;
    }
    img = applyWatermark(watermarkOptions, img);
    if(!img)
    {
        mylog("failed to watermark video '%s'. Check that you have enough memory and hard drive space.", inFile.size() ? inFile.toUtf8().data() : "");
        goto Exit;
    }

    hasWatermark = hasWatermarkToApply(watermarkOptions, img);
    if(watermarkOptions.last()->m_licenseType != LicenseType_SquiggleMarkPhotoVideo)
    {
        img = applyTrialImageWatermark(watermarkOptions.last(), img);
        hasWatermark = true;
    }
    if(!img)
    {
        mylog("failed to watermark video '%s'. Could not apply demo watermark.", inFile.size() ? inFile.toUtf8().data() : "");
        goto Exit;
    }


    if( device.m_scaling != IMG_SCALING_NONE )
    {
        gdImagePtr ret = resizeImage(img, device);
        gdImageDestroy(img);
        img = ret;
    }

    {
        QString tmp = layerFilePath(getTmpFileName());
        QString watermarkFile = hasWatermark ? saveImgToFile(img, tmp) : "";
        int sx = img->sx;
        int sy = img->sy;
        gdImageDestroy(img);

        if(!m_getVideoQuality) { mylog("m_getVideoQuality"); }
        FFMpeg ffmpeg(mainWnd, numFrames,
                      device.m_rotationAngle != 0,
                      device.m_rotationAngle,
                      device.m_scaling != IMG_SCALING_NONE,
                      device.m_crop_rect,
                      sx, sy,
                      m_getVideoQuality(),
                      &m_stop);
        ffmpeg.waterMark(inFile, outFile, watermarkFile, 0, 0);

        if(hasWatermark)
        {
            QFile file(watermarkFile);
            file.remove();
        }

        savedFile =  outFile;
    }

Exit:
    return savedFile;
}

QString ImageHelper::resizeAndWatermarkImage(
        QObject *mainWnd,
        const QString inFile,
        const QString outFile,
        QList<WatermarkOptions*> watermarkOptions,
        Device device) const
{
    QString savedFile;
    FILE* fp  = 0;
    FILE* fpOut = 0;
    gdImagePtr img = 0;

//   if((device.m_action & IMG_ACTION_OUTPUT_WEBP) && (device.m_scaling == IMG_SCALING_NONE))
//   {
//       convertToWebP(inFile, outFile);
//       return outFile;
//   }

    img = getGdImageFromPath(inFile.toUtf8().data(), 0);
    if(!img)
    {
        mylog("failed to load image '%s'. Check that you have enough memory and hard drive space.", inFile.size() ? inFile.toUtf8().data() : "");
        goto Exit;
    }

    if(device.m_crop_rect.width() > 1 && device.m_crop_rect.height() > 1)
    {
        gdImagePtr ret = cropImage(device.m_crop_rect, inFile, img);
        gdImageDestroy(img);
        img = ret;
    }

    if( device.m_rotationAngle != 0 )
    {
        gdImagePtr ret = rotateImage(device, inFile, img);
        gdImageDestroy(img);
        img = ret;
    }

    if( device.m_scaling != IMG_SCALING_NONE )
    {
        gdImagePtr ret = resizeImage(img, device);
        gdImageDestroy(img);
        img = ret;
    }

    img = applyWatermark(watermarkOptions, img);

    if(watermarkOptions.last()->m_licenseType != LicenseType_SpriteMill
            && watermarkOptions.last()->m_licenseType != LicenseType_SquiggleMarkPhoto
            && watermarkOptions.last()->m_licenseType != LicenseType_SquiggleMarkPhotoVideo)
    {
        img = applyTrialImageWatermark(watermarkOptions.last(), img);
    }

    fpOut = fopen(outFile.toUtf8().data(), "wb");
    if(!fpOut)
    {
        mylog("waterMarkTheImage failed to open %s, errno: %i",
              outFile.size() ? outFile.toUtf8().data() : "", errno);
        goto Exit;
    }
    if(img)
    {
        if(!device.m_trueColor)
        {
            gdImageTrueColorToPalette(img, true, 256);
        }
        IMG_TYPE imgType;
        QFileInfo of(outFile);
        if(of.suffix().toLower() == "jpg")
        {
            imgType = IMG_TYPE_JPG;
        }
        else if(of.suffix().toLower() == "png")
        {
            imgType = IMG_TYPE_PNG;
        }
        else if(of.suffix().toLower() == "webp")
        {
            imgType = IMG_TYPE_WEBP;
        }
        gdImageWrite(img, fpOut, imgType);
        savedFile =  outFile;
    }
Exit:
   if(fpOut)
    {
        fclose(fpOut);
    }
    if(img)
    {
        gdImageDestroy(img);
    }
    return savedFile;
}

QString ImageHelper::resizeAndWatermark(
        QObject *mainWnd,
        const QString inFile,
        const QString outFile,
        QList<WatermarkOptions*> watermarkOptions,
        Device device) const
{
    QString ret;

    if(inFile==outFile)
    {
        mylog("will not overwrite source file. change your destination file");
        return ret;
    }

   if(!m_stop)
   {
       mylog("start render '%s', %i x %i", inFile.size() ? inFile.toUtf8().data() : "", device.m_widthPixels, device.m_heightPixels);
       ret = isVideoFile(inFile) ?
                resizeAndWatermarkVideo(mainWnd, inFile, outFile, watermarkOptions, device) :
                resizeAndWatermarkImage(mainWnd, inFile, outFile, watermarkOptions, device);
   }
   mylog("leave render");
   return ret;
}

QList<QFuture<QString> > ImageHelper::resizeAndWatermarkAsync(
        QObject *mainWnd,
        const QString fileName,
        const QString outputDir,
        QList<WatermarkOptions*> watermarkOptions,
        Device device)
{
    QList<QFuture<QString> > futures;

    if(!m_stop)
    {
        QFileInfo fileInfo(fileName);

        QString outFile = getOutFilePathAndMakeDir(fileInfo, outputDir, device, watermarkOptions.last());
        futures.append(QtConcurrent::run(
                           this,
                           &ImageHelper::resizeAndWatermark,
                           mainWnd,
                           fileInfo.absoluteFilePath(),
                           outFile,
                           watermarkOptions,
                           device
                           ));
    }
    return futures;
}

void ImageHelper::start()
{
    m_stop = false;
}

void ImageHelper::stop()
{
    m_stop = true;
}

bool ImageHelper::saveImgToFile(const gdImagePtr img, const QString &fileName, IMG_TYPE imgType) const
{
    FILE* fpOut = fopen(fileName.toUtf8().data(), "wb");
    if(fpOut)
    {
        gdImageWrite(img, fpOut, imgType);
        fclose(fpOut);
        return true;
    }
    else
    {
        mylog("failed to open file:  %s, errno: %i", fileName.toUtf8().data(), errno);
    }
    return false;
}

QString ImageHelper::saveImgToFile(const gdImagePtr img, const QString &fileName) const
{
    if(saveImgToFile(img, fileName, IMG_TYPE_PNG))
    {
        return fileName;
    }
    return "";
}

gdImagePtr ImageHelper::createTrueColorImageWithTransparentBackground(int width, int height) const
{
    gdImagePtr img = gdImageCreateTrueColor(width, height);
    if(img)
    {
        gdImageSaveAlpha(img, 1);
        gdImageAlphaBlending(img, 0);
        gdImageFilledRectangle(img, 0, 0, width, height,
                               gdImageColorAllocateAlpha(img, 0, 0, 0, gdAlphaMax));
        gdImageAlphaBlending(img, 1);
        return img;
    }
    return 0;
}

void ImageHelper::drawUnderline(gdImagePtr img, int height, int underlineRectHeight, int color, int *brect) const
{
    int underlineHeight = height/25;
    gdImageFilledRectangle(img,
                brect[LOWER_LEFT_X],
                brect[LOWER_LEFT_Y] + underlineRectHeight - underlineHeight,
                brect[LOWER_RIGHT_X],
                brect[LOWER_RIGHT_Y] + underlineRectHeight,
                color);
}

QString ImageHelper::createWatermarkFileFromTextUsingFreeTypeFont(
        const QString& text, const FontHelper *fontHelper) const
{
    //char fontname[] = "C:\\Windows\\Fonts\\Verdana.ttf";
    const int margin = 10;
    int color = 0;

    QString ret;
    int brect[8];
    char *data= strdup(text.toUtf8().data());
    // obtain brect so that we can size the image
    //gdImagePtr im, int *brect, int fg, char *fontname, double ptsize, double angle, int x, int y, char *string
    char *err = 0;
    QString fontFile;
    foreach(fontFile, fontHelper->getFontFilesForCurrentFontFamily())
    {
        if(fontFile.size() == 0)
        {
            mylog("gdImageStringFT fontFile.size: 0\n");
            continue;
        }
        err = gdImageStringFT(
                NULL,
                &brect[0],
                color,
                fontFile.toUtf8().data(),
                fontHelper->getFontSizeInPoints(),
                0.0,
                0,
                0,
                data);
        if (err)
        {
            mylog("gdImageStringFT err: '%s'\n", err);
        }
        else break;
    }
    if(err)
    {
        mylog("gdImageStringFT err: '%s'\n", err);
        return ret;
    }

    /* create an image big enough for the string plus a little whitespace */
    int width = brect[LOWER_RIGHT_X]-brect[UPPER_LEFT_X] + 3*margin;
    int height = brect[LOWER_RIGHT_Y]-brect[UPPER_LEFT_Y] + 3*margin;
    int underlineRectHeight = fontHelper->isUnderlined() ? height/20 : 0;
    gdImagePtr img = createTrueColorImageWithTransparentBackground(
                width,
                height + underlineRectHeight);
    if(!img)
    {
        mylog("createTrueColorImageWithTransparentBackground in createWatermarkFileFromTextUsingFreeTypeFont failed\n");
        return ret;
    }

    color = fontHelper->getFontColor();

    /* render the string, offset origin to center string*/
    /* note that we use top-left coordinate for adjustment
     * since gd origin is in top-left with y increasing downwards. */
    int x =  margin-brect[UPPER_LEFT_X];
    int y =  margin-brect[UPPER_LEFT_Y];
    err = gdImageStringFT(img, &brect[0], color, fontFile.toUtf8().data(), fontHelper->getFontSizeInPoints(), 0.0, x, y, data);
    if (err)
    {
        mylog("gdImageStringFT err '%s'\n", err);
        return ret;
    }

    if(fontHelper->isUnderlined())
    {
        drawUnderline(img, height, underlineRectHeight, color, &brect[0]);
    }

    ret = saveImgToFile(img, layerFilePath(SaveFileName).toUtf8().data());
    gdImageDestroy(img);


    mylog("gdImageStringFT ok\n");
    free(data);
    return ret;
}

void ImageHelper::setQualityGetters(std::function<int()>getJpegQuality,
                       std::function<int()>getPngQuality,
                       std::function<int()>getVideoQuality)
{
    m_getJpegQuality = getJpegQuality;
    m_getPngQuality = getPngQuality;
    m_getVideoQuality = getVideoQuality;
}

QString ImageHelper::layerFilePath(const QString &file) const
{
    QString path(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    path = path.append(QDir::separator()).append(m_app_name);
    if(!QFile(path).exists())
    {
        QDir().mkpath(path);
    }
    if(!QFile(path).exists())
    {
        return file;
    }
    return path.append(QDir::separator()).append(file);
}

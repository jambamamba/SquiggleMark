#ifndef WATERMARKPLACEMENT_H
#define WATERMARKPLACEMENT_H

#include "LicenseType.h"

enum Operation
{
    OPERATION_RESIZING_IMAGES,
    OPERATION_ROTATING_IMAGES,
    OPERATION_WATERMARKING_IMAGES,
    OPERATION_RESIZE_FOR_ITUNES_SCREEN_SHOTS,
    OPERATION_RESIZE_FOR_IOS_ICONS,
    OPERATION_RESIZE_FOR_IOS_IMAGES,
    OPERATION_RESIZE_FOR_MACOS_ICONS
};

enum ImageScaling
{
    IMG_SCALING_NONE,
    IMG_SCALING_FIXED_WIDTH,
    IMG_SCALING_FIXED_HEIGHT,
    IMG_SCALING_FIXED_WIDTH_AND_HEIGHT,
    IMG_SCALE_WIDTH_AND_CROP_HEIGHT,
    IMG_SCALE_HEIGHT_AND_CROP_WIDTH,
};

enum ImageAction
{
    IMG_ACTION_NONE,
    IMG_ACTION_KEEP_TRANSPARENCY = 0x01,
    IMG_ACTION_RESIZE_CANVAS = 0x10,
    IMG_ACTION_DONT_PREPEND_FILENAME = 0x100,
    IMG_ACTION_OUTPUT_WEBP = 0x1000,
    IMG_ACTION_RESIZE_FOR_IOS_DEVICE = 0x10000,
};

enum WaterPlacement {
    WaterPlacement_TopLeft,
    WaterPlacement_TopRight,
    WaterPlacement_BottomLeft,
    WaterPlacement_BottomRight,
    WaterPlacement_Centered,
    WaterPlacment_Custom,
    WaterPlacement_CenterLeft,
    WaterPlacement_CenterTop,
    WaterPlacement_CenterBottom,
    WaterPlacement_CenterRight,
};

enum OutputImageFormat {
    OutputImageFormat_Original,
    OutputImageFormat_Jpg,
    OutputImageFormat_Png
};

enum OutputVideoFormat {
    OutputVideoFormat_Original,
    OutputVideoFormat_Mp4,
    OutputVideoFormat_Ogg
};

struct WatermarkOptions {
    WaterPlacement m_placement;
    gdImagePtr m_watermarkimg;
    gdImagePtr m_trialimg;
    OutputImageFormat m_outputImageFormat;
    OutputVideoFormat m_outputVideoFormat;
    int m_watermarkPercentOpacity;
    int m_watermarkMarginX;
    int m_watermarkMarginY;
    int m_tileWatermark;
    int m_tileMarginAsPercentageOfWatermarkSide;
    LicenseType m_licenseType;

    WatermarkOptions(LicenseType licenseType, gdImagePtr trialimg,
                     OutputImageFormat outputImageFormat, OutputVideoFormat outputVideoFormat)
        : WatermarkOptions(licenseType, trialimg, outputImageFormat, outputVideoFormat,
                           nullptr, WaterPlacement_TopLeft, 0, 0, 0, false, 100)
    {}

    WatermarkOptions(LicenseType licenseType, gdImagePtr trialimg,
                     OutputImageFormat outputImageFormat, OutputVideoFormat outputVideoFormat,
                     gdImagePtr watermarkimg,
                     WaterPlacement placement,
                     int watermarkPercentOpacity,
                     int watermarkMarginX,
                     int watermarkMarginY,
                     int tileWatermark,
                     int tileMarginPercent)
        : m_placement(placement)
        , m_watermarkimg(watermarkimg)
        , m_trialimg(trialimg)
        , m_outputImageFormat(outputImageFormat)
        , m_outputVideoFormat(outputVideoFormat)
        , m_watermarkPercentOpacity(watermarkPercentOpacity)
        , m_watermarkMarginX(watermarkMarginX)
        , m_watermarkMarginY(watermarkMarginY)
        , m_tileWatermark(tileWatermark)
        , m_tileMarginAsPercentageOfWatermarkSide(tileMarginPercent)
        , m_licenseType(licenseType)
    {}
};

class IOSDeviceProperties
{
public:
    IOSDeviceProperties(int referenceImageSize = -1,
                        ImageScaling scaling=IMG_SCALING_NONE,
                        bool trueColor = true,
                        bool dither = true )
        : m_referenceImageSize(referenceImageSize)
        , m_scaling(scaling)
        , m_trueColor(trueColor)
        , m_dither(dither)
    {}
    int m_referenceImageSize;
    ImageScaling m_scaling;
    bool m_trueColor;
    bool m_dither;
};

class Device
{
public:
    Device()
        : m_widthPixels(0)
        , m_rotationAngle(0)
        , m_heightPixels(0)
        , m_scaling(IMG_SCALING_NONE)
        , m_pixelsPerPoint(1)
        , m_referenceImageSize(4096)
        , m_trueColor(true)
        , m_action(IMG_ACTION_KEEP_TRANSPARENCY)
    {
    }

    Device(ImageScaling scaling,
           int rotationAngle,
           const QRect &crop_rect,
           int width,
           int height,
           int pixelsPerPoint,
           const QString &suffix,
           const QString &name,
           int action)
        : m_rotationAngle(rotationAngle)
        , m_crop_rect(crop_rect)
        , m_widthPixels(width)
        , m_heightPixels(height)
        , m_pixelsPerPoint(pixelsPerPoint)
        , m_referenceImageSize(4096)
        , m_trueColor(true)
        , m_dither(false)
        , m_scaling(scaling)
        , m_suffix(suffix)
        , m_name(name)
        , m_action(action)
    {
    }

    Device(const Device &rhs)
    {
        m_rotationAngle = rhs.m_rotationAngle;
        m_widthPixels = rhs.m_widthPixels;
        m_heightPixels = rhs.m_heightPixels;
        m_pixelsPerPoint = rhs.m_pixelsPerPoint;
        m_referenceImageSize = rhs.m_referenceImageSize;
        m_dither = rhs.m_dither;
        m_trueColor = rhs.m_trueColor;
        m_suffix = rhs.m_suffix;
        m_name = rhs.m_name;
        m_scaling = rhs.m_scaling;
        m_action = rhs.m_action;
        m_crop_rect = rhs.m_crop_rect;
    }

    const Device &operator=(const Device &rhs)
    {
        m_rotationAngle = rhs.m_rotationAngle;
        m_widthPixels = rhs.m_widthPixels;
        m_heightPixels = rhs.m_heightPixels;
        m_pixelsPerPoint = rhs.m_pixelsPerPoint;
        m_referenceImageSize = rhs.m_referenceImageSize;
        m_trueColor = rhs.m_trueColor;
        m_dither = rhs.m_dither;
        m_suffix = rhs.m_suffix;
        m_name = rhs.m_name;
        m_scaling = rhs.m_scaling;
        m_action = rhs.m_action;
        m_crop_rect = rhs.m_crop_rect;
        return *this;
    }

    int m_rotationAngle;
    int m_widthPixels;
    int m_heightPixels;
    int m_pixelsPerPoint;
    int m_referenceImageSize;
    bool m_trueColor;
    bool m_dither;
    QString m_suffix;
    QString m_name;
    ImageScaling m_scaling;
    QRect m_crop_rect;
    int m_action;
};
Q_DECLARE_METATYPE(Device);

#endif // WATERMARKPLACEMENT_H

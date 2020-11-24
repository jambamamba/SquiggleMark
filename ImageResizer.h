#ifndef IMAGERESIZER_H
#define IMAGERESIZER_H

#include <gd.h>

#include "ImageHelper.h"

namespace {

    enum DEVICE_TYPE
    {
        DEVICE_TYPE_IPAD,
        DEVICE_TYPE_IPAD_R,
        DEVICE_TYPE_IPHONE_3,
        DEVICE_TYPE_IPHONE_4,
        DEVICE_TYPE_IPHONE_5
    };

    typedef struct SCREEN_TYPE_
    {
        enum DEVICE_TYPE deviceType;
        int widthPixels;
        int heightPixels;
        char postFixName[32];
    }
    SCREEN_TYPE;

    static const SCREEN_TYPE DeviceTypes[] = {
        { DEVICE_TYPE_IPAD, 1024, 768, "-ipad"},
        { DEVICE_TYPE_IPAD_R, 2048, 1536, "-ipad@2x"},
        { DEVICE_TYPE_IPHONE_3, 480, 320, "-iphone3"},
        { DEVICE_TYPE_IPHONE_4, 960, 640, "-iphone4@2x"},
        { DEVICE_TYPE_IPHONE_5, 1136, 640, "-iphone5@2x"}
    };

    typedef struct PATH_COMPONENTS_
    {
        char* fullPath;
        char* outDir;
        int osz;
        char* dir;
        int dsz;
        char* filename;
        int fsz;
        char* ext;
        int esz;
    }
    PATH_COMPONENTS;
}

class ImageResizer
{
public:
    ImageResizer(ImageHelper *imageHelper);
    void loadImageAndGenerateImagesForDevices(char *inFile, PATH_COMPONENTS pathC);

private:
    char* makeOutputPath(const PATH_COMPONENTS *pathC, char *filenamePostfix);
    void saveImgResampled(PATH_COMPONENTS pathC, gdImagePtr loadedImg,
                            SCREEN_TYPE screenType, IMG_TYPE imgType);
    void generateImagesForDevices(PATH_COMPONENTS pathC, gdImagePtr loadedImg, IMG_TYPE imgType);

    ImageHelper *m_imageHelper;

};

#endif // IMAGERESIZER_H

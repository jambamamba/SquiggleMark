#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <QDir>

#include "ImageResizer.h"
#include "ImageHelper.h"

extern void mylog(const char *fmt, ...);

ImageResizer::ImageResizer(ImageHelper *imageHelper)
    : m_imageHelper(imageHelper)
{
}

char* ImageResizer::makeOutputPath(const PATH_COMPONENTS *pathC, char *filenamePostfix)
{
    int sz = strlen(pathC->fullPath) + pathC->osz + 1024;
    char *outPath = (char*) malloc(sz);
    if(!outPath)
    {
        //sprintf(err, "ERR: Not enough memory for output file name");
        goto Exit;
    }
    outPath[0] = 0;
    if(pathC->outDir)
    {
        memcpy(outPath, pathC->outDir, pathC->osz);
        outPath[pathC->osz] = QDir::separator().toLatin1();
        outPath[pathC->osz + 1] = 0;
    }
    else if(pathC->dir)
    {
        memcpy(outPath, pathC->dir, pathC->dsz);
        outPath[pathC->dsz] = QDir::separator().toLatin1();
        outPath[pathC->dsz + 1] = 0;
    }
    if(pathC->filename)
    {
        int len = strlen(outPath);
        memcpy(outPath + len, pathC->filename, pathC->fsz);
        outPath[len + pathC->fsz] = 0;
    }
    if(filenamePostfix)
    {
        strcat(outPath, filenamePostfix);
    }
    if(pathC->ext)
    {
        strcat(outPath, ".");
        strcat(outPath, pathC->ext);
    }
Exit:
    return outPath;
}

void ImageResizer::saveImgResampled(PATH_COMPONENTS pathC, gdImagePtr loadedImg,
                        SCREEN_TYPE screenType, IMG_TYPE imgType)
{
    char* outPath = 0;
    FILE* fpOut = 0;
    gdImagePtr imgOut = 0;
    int width = loadedImg->sx * screenType.widthPixels /
        DeviceTypes[DEVICE_TYPE_IPAD].widthPixels;
    int height = loadedImg->sy * screenType.widthPixels /
        DeviceTypes[DEVICE_TYPE_IPAD].widthPixels;
    imgOut = gdImageCreateTrueColor(width, height);
    if(!imgOut)
        goto Exit;
    gdImageAlphaBlending(imgOut, 0);
    gdImageSaveAlpha(imgOut, 1);
    gdImageCopyResampled(imgOut, loadedImg,
                         0, 0,
                         0, 0,
                         width, height,
                         loadedImg->sx, loadedImg->sy
                         );

    outPath = makeOutputPath(&pathC, screenType.postFixName);
    fpOut = fopen(outPath, "wb");
    if(!fpOut)
    {
        mylog("saveImgResampled Failed to open %s, errno: %i",
              outPath ? outPath : "", errno);
        goto Exit;
    }
    m_imageHelper->gdImageWrite(imgOut, fpOut, imgType);
Exit:
    if(fpOut) fclose(fpOut);
    if(imgOut) gdFree(imgOut);
    if(outPath) free(outPath);
}

void ImageResizer::generateImagesForDevices(PATH_COMPONENTS pathC, gdImagePtr loadedImg, IMG_TYPE imgType)
{
    int i;
    for(i=0; i<sizeof(DeviceTypes)/sizeof(SCREEN_TYPE); i++)
    {
        saveImgResampled(pathC, loadedImg, DeviceTypes[i], imgType);
    }
}

void ImageResizer::loadImageAndGenerateImagesForDevices(char *inFile, PATH_COMPONENTS pathC)
{
    IMG_TYPE imgType = IMG_TYPE_UNKNOWN;
    gdImagePtr loadedImg = m_imageHelper->getGdImageFromPath(inFile, imgType);
    if(loadedImg)
    {
        generateImagesForDevices(pathC, loadedImg, imgType);
        gdFree(loadedImg);
    }
}

#include "SupportedFileTypes.h"

SupportedFileTypes::SupportedFileTypes()
    : m_videoSupported(true)
{
}

void SupportedFileTypes::dontSupportVideo()
{
    m_videoSupported = false;
}

QStringList SupportedFileTypes::getImageTypes()
{
    QStringList ret;
    ret << "*.jpg";
    ret << "*.jpeg";
    ret << "*.png";
    return ret;
}

QStringList SupportedFileTypes::getVideoTypes()
{
    QStringList ret;
    if(!m_videoSupported)
    {
        return ret;
    }

    ret << "*.avi";
    ret << "*.mp4";
    ret << "*.mov";
    ret << "*.qt";
    ret << "*.wmv";
    ret << "*.m4v";
    ret << "*.m2v";
    ret << "*.mpg";
    ret << "*.mpeg";
    ret << "*.webm";
    ret << "*.vob";
    ret << "*.ogg";
    ret << "*.mkv";
    ret << "*.3gg";
    ret << "*.3gp";
    ret << "*.3g2";
    ret << "*.flv";

    return ret;
}

QStringList SupportedFileTypes::getList()
{
    QStringList ret;
    ret << SupportedFileTypes::getImageTypes();
    ret << SupportedFileTypes::getVideoTypes();
    return ret;
}

QString SupportedFileTypes::getString()
{
   QString ret;
   QStringList list = SupportedFileTypes::getList();
   for(int i=0; i < list.size(); ++i)
   {
       if(i>0)
       {
           ret.append(" ");
       }
       ret.append(list.at(i));
   }
   return ret;
}


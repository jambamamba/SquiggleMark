#ifndef SUPPORTEDFILETYPES_H
#define SUPPORTEDFILETYPES_H

#include <QStringList>

class SupportedFileTypes
{
public:
    SupportedFileTypes();
    QStringList getList();
    QString getString();
    QStringList getImageTypes();
    QStringList getVideoTypes();
    void dontSupportVideo();
private:
    bool m_videoSupported;
};

#endif // SUPPORTEDFILETYPES_H

#include "FontHelper.h"

#include <QDesktopServices>
#include <QDir>

#include <ft2build.h>
#include FT_FREETYPE_H

extern "C" void mylog(const char *fmt, ...);

namespace {
    const double FONT_SIZE_POINT_INCREMENT = 6.;
    const double MIN_FONT_SIZE_POINTS = 1.;
    const double DEFAULT_FONT_SIZE_POINTS = 36.;
}

FontHelper::FontHelper()
    : m_currentFontSizeInPoints(DEFAULT_FONT_SIZE_POINTS)
    , m_fontColorRgb(0xffffff)
    , m_currentFontStyle(FONTHELPER_STYLE_FLAG_REGULAR)
    , m_underline(false)
{
}

QStringList FontHelper::scanDirectory(const QDir &dir, QStringList fileTypes) const
{
    QStringList files = dir.entryList(fileTypes,
                                            QDir::Files | QDir::NoSymLinks);
    QStringList filesWithPath;
    foreach(QString file, files)
    {
        file = dir.absolutePath().append("/").append(file);
        filesWithPath.append(file);
    }

    QStringList dirTypes;
    dirTypes.append("*");
    QStringList dirs = dir.entryList(dirTypes,
                                               QDir::Dirs | QDir::NoSymLinks);
    foreach(const QString &subdir, dirs)
    {
        if(subdir == "." || subdir == ".." )
            continue;
        QString subdir_ = dir.absolutePath().append("/").append(subdir);
        filesWithPath.append ( scanDirectory(
                           QDir(subdir_).absolutePath(), fileTypes) );
    }
    return filesWithPath;
}

bool FontHelper::loadFonts(const QString &additionalFontsDir)
{
    FT_Library  library;
    FT_Error error = FT_Init_FreeType( &library );
    if ( error )
    {
        mylog("an error occurred during FreeType library initialization");
        return false;
    }

    QDir dir(QStandardPaths::standardLocations(QStandardPaths::FontsLocation).first());

    QStringList fileTypes;
    fileTypes.append("*.ttf");
    QStringList files = dir.entryList(fileTypes,
                                      QDir::Files | QDir::NoSymLinks);
    QStringList filesWithPath;
    foreach(QString file, files)
    {
        file = dir.absolutePath().append("/").append(file);
        filesWithPath.append(file);
    }
    filesWithPath.append( scanDirectory(QDir("/usr/share/fonts"), fileTypes) );
    filesWithPath.append( scanDirectory(QDir("/Library/Fonts"), fileTypes) );
    if(additionalFontsDir.size()>0)
    {
        filesWithPath.append( scanDirectory(QDir(additionalFontsDir), fileTypes) );
    }

    foreach(const QString &fontFile, filesWithPath)
    {
        FT_Face face;
        error = FT_New_Face( library,
                             fontFile.toUtf8().data(),
                             0,
                             &face );
        if ( error == FT_Err_Unknown_File_Format )
        {
            mylog("the font file could be opened and read, but it appears"
                   " that its font format is unsupported '%s'",
                  fontFile.size() > 0 ? fontFile.toUtf8().data() : "");
        }
        else if ( error )
        {
            mylog("the font file could not"
                   " be opened or read, or simply that it is broken '%s'",
                  fontFile.size() > 0 ? fontFile.toUtf8().data() : "");
        }
        else if ( FT_IS_SCALABLE(face) )
        {
            addToFontMap(fontFile, face);
            FT_Done_Face(face);
        }
    }

    handleMissingRegularStyle();

    return m_fontMap.size() > 0;
}

void FontHelper::handleMissingRegularStyle()
{
    QMapIterator<QString, QMap<int, QStringList> > itFamily(m_fontMap);
    while( itFamily.hasNext() )
    {
        itFamily.next();
        if( !itFamily.value().contains(FONTHELPER_STYLE_FLAG_REGULAR) )
        {
            if(itFamily.value().contains(FONTHELPER_STYLE_FLAG_BOLD))
            {
                m_fontMap[itFamily.key()].insert(FONTHELPER_STYLE_FLAG_REGULAR,
                                        itFamily.value()[FONTHELPER_STYLE_FLAG_BOLD]);
            }
            else if(itFamily.value().contains(FONTHELPER_STYLE_FLAG_ITALIC))
            {
                m_fontMap[itFamily.key()].insert(FONTHELPER_STYLE_FLAG_REGULAR,
                                        itFamily.value()[FONTHELPER_STYLE_FLAG_ITALIC]);
            }
            else if(itFamily.value().contains(FONTHELPER_STYLE_FLAG_BOLD|FONTHELPER_STYLE_FLAG_ITALIC))
            {
                m_fontMap[itFamily.key()].insert(FONTHELPER_STYLE_FLAG_REGULAR,
                                        itFamily.value()[FONTHELPER_STYLE_FLAG_BOLD|FONTHELPER_STYLE_FLAG_ITALIC]);
            }
            else
            {
                m_fontMap.remove(itFamily.key());
            }
        }
    }
}

void FontHelper::addToFontMap(const QString &fontFile, FT_Face face)
{
    QString family(face->family_name);
    //QString key(face->family_name).append("+").append(face->style_name);
    if(!m_fontMap.contains(family))
    {
        QStringList files;
        files.append(fontFile);
        QMap<int, QStringList> map;
        map.insert( freeTypeStyleToFontHelperStyle(face->style_flags) , files);
        m_fontMap[family] = map;
    }
    else if(!m_fontMap[family].contains( freeTypeStyleToFontHelperStyle(face->style_flags) ))
    {
        QStringList files;
        files.append(fontFile);
        m_fontMap[family].insert( freeTypeStyleToFontHelperStyle(face->style_flags) , files);
    }
    else
    {
        m_fontMap[family][ freeTypeStyleToFontHelperStyle(face->style_flags) ].append(fontFile);
    }
}

QStringList FontHelper::getFontFilesForCurrentFontFamily() const
{
    return m_fontMap[m_currentFontFamily][m_currentFontStyle];
}

double FontHelper::getFontSizeInPoints() const
{
    return m_currentFontSizeInPoints;
}

const QMap<QString, QMap<int, QStringList> > FontHelper::getFonts() const
{
    return m_fontMap;
}

bool FontHelper::incrementFontSize()
{
    m_currentFontSizeInPoints += FONT_SIZE_POINT_INCREMENT;
    return true;
}

bool FontHelper::decrementFontSize()
{
    if((m_currentFontSizeInPoints - FONT_SIZE_POINT_INCREMENT) >= MIN_FONT_SIZE_POINTS)
    {
        m_currentFontSizeInPoints -= FONT_SIZE_POINT_INCREMENT;
        return true;
    }
    return false;
}

bool FontHelper::setFontSizeInPoints(double fontSizeInPoints)
{
    if(fontSizeInPoints >= MIN_FONT_SIZE_POINTS)
    {
        m_currentFontSizeInPoints = fontSizeInPoints;
        return true;
    }
    return false;
}

unsigned int FontHelper::fontColorNormalizeAlphaFromMax255toMax127(unsigned int rgb) const
{
    unsigned int color = 0x00ffffff & rgb;
    unsigned int alpha = ((0xff000000 & rgb));
    alpha /= 2;
    color = (color | (alpha << 24));

    return color;
}

void FontHelper::setFontColor(unsigned int rgb)
{
    unsigned int color = fontColorNormalizeAlphaFromMax255toMax127(rgb);

    m_fontColorRgb = color;
}

unsigned int FontHelper::getFontColor() const
{
    return m_fontColorRgb;
}

unsigned int FontHelper::getFontColorAlpha() const
{
    return (( m_fontColorRgb & 0xff000000) >> 24);
}
unsigned int FontHelper::getFontColorRed() const
{
    return (( m_fontColorRgb & 0x00ff0000) >> 16);
}
unsigned int FontHelper::getFontColorGreen() const
{
    return (( m_fontColorRgb & 0x0000ff00) >> 8);
}
unsigned int FontHelper::getFontColorBlue() const
{
    return (( m_fontColorRgb & 0x000000ff));
}

void FontHelper::setFontFamily(const QString &fontFamily)
{
    m_currentFontFamily = fontFamily;
}

QString FontHelper::getFontFamily() const
{
    return m_currentFontFamily;
}

bool FontHelper::addFontStyle(FONTHELPER_STYLE_FLAG fontStyle)
{
    if(fontStyle > FONTHELPER_STYLE_FLAG_REGULAR && fontStyle <= FONTHELPER_STYLE_FLAG_BOLD)
    {
        m_currentFontStyle |= fontStyle;
        return true;
    }
    return false;
}

bool FontHelper::removeFontStyle(FONTHELPER_STYLE_FLAG fontStyle)
{
    if(fontStyle > FONTHELPER_STYLE_FLAG_REGULAR && fontStyle <= FONTHELPER_STYLE_FLAG_BOLD)
    {
        m_currentFontStyle &= ~fontStyle;
        return true;
    }
    return false;
}

bool FontHelper::setFontStyle(FONTHELPER_STYLE_FLAG fontStyle)
{
    if(fontStyle > FONTHELPER_STYLE_FLAG_REGULAR && fontStyle <= FONTHELPER_STYLE_FLAG_BOLD)
    {
        m_currentFontStyle = fontStyle;
        return true;
    }
    return false;
}

FONTHELPER_STYLE_FLAG FontHelper::getFontStyle() const
{
    return (FONTHELPER_STYLE_FLAG)m_currentFontStyle;
}

bool FontHelper::isFontStyleAvailable(int style) const
{
    return m_fontMap.contains(m_currentFontFamily) && m_fontMap[m_currentFontFamily].contains(style) &&
            m_fontMap[m_currentFontFamily][style].size() > 0;
}

int FontHelper::freeTypeStyleToFontHelperStyle(int freeTypeStyle) const
{
    return freeTypeStyle;
//    int style = FONTHELPER_STYLE_FLAG_NONE;
//    switch(freeTypeStyle)
//    {
//        case FT_STYLE_FLAG_ITALIC:
//            style |= FONTHELPER_STYLE_FLAG_ITALIC;
//            break;
//        case FT_STYLE_FLAG_BOLD:
//            style |= FONTHELPER_STYLE_FLAG_BOLD;
//            break;
//        case FT_STYLE_FLAG_ITALIC|FT_STYLE_FLAG_BOLD:
//            style |= FONTHELPER_STYLE_FLAG_ITALIC|FONTHELPER_STYLE_FLAG_BOLD;
//            break;
//        default:
//            style |= FONTHELPER_STYLE_FLAG_REGULAR;
//            break;
//    }
//    return style;
}

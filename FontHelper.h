#ifndef FONTHELPER_H
#define FONTHELPER_H

#include <QMap>
#include <QStringList>

class QDir;

enum FONTHELPER_STYLE_FLAG
{
    FONTHELPER_STYLE_FLAG_REGULAR = 0,
    FONTHELPER_STYLE_FLAG_ITALIC = ( 1 << 0 ),//FT_STYLE_FLAG_ITALIC,
    FONTHELPER_STYLE_FLAG_BOLD = ( 1 << 1 ),//FT_STYLE_FLAG_BOLD
};

typedef struct FT_FaceRec_*  FT_Face;
class FontHelper
{
public:
    FontHelper();

    void setFontFamily(const QString &fontFamily);
    QString getFontFamily() const;
    bool setFontStyle(FONTHELPER_STYLE_FLAG fontStyle);
    bool addFontStyle(FONTHELPER_STYLE_FLAG fontStyle);
    bool removeFontStyle(FONTHELPER_STYLE_FLAG fontStyle);
    FONTHELPER_STYLE_FLAG getFontStyle() const;
    bool setFontSizeInPoints(double fontSizeInPoints);
    double getFontSizeInPoints() const;
    const QMap<QString, QMap<int, QStringList> > getFonts() const;
    bool incrementFontSize();
    bool decrementFontSize();
    QStringList getFontFilesForCurrentFontFamily() const;
    bool isFontStyleAvailable(int style) const;
    void setFontColor(unsigned int rgb);
    unsigned int getFontColor() const;
    unsigned int getFontColorAlpha() const;
    unsigned int getFontColorRed() const;
    unsigned int getFontColorGreen() const;
    unsigned int getFontColorBlue() const;
    bool isUnderlined() const { return m_underline; }
    void setUnderlined(bool underlined) { m_underline = underlined; }
    bool loadFonts(const QString &additionalFontsDir);

private:
    QString m_currentFontFamily;
    //QMap<key:font-face+style, QMap<key(int):regular|bold|italic, list-of-font-files>
    QMap<QString, QMap<int, QStringList> > m_fontMap;
    unsigned int m_fontColorRgb;
    double m_currentFontSizeInPoints;
    int m_currentFontStyle;
    bool m_underline;

    unsigned int fontColorNormalizeAlphaFromMax255toMax127(unsigned int rgb) const;
    int freeTypeStyleToFontHelperStyle(int freeTypeStyle) const;
    void handleMissingRegularStyle();
    void addToFontMap(const QString &fontFile, FT_Face face);
    QStringList scanDirectory(const QDir &dir, QStringList fileTypes) const;
};

#endif // FONTHELPER_H

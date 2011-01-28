/*----------------------------------------------------------\
|                         Font.h                            |
|                         ------                            |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/
// Verwaltung von Schriften

#ifndef FONT_H
#define FONT_H

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include <map>
#include <string>
#include <list>
#include <boost/shared_ptr.hpp>

#define ORIENT_CCW true   // Gegenuhrzeigersiin
#define ORIENT_CW false   // Uhrzeigersinn

struct Box;
struct Font;

class FTFont;

enum Align
{
    AlignLeft,
    AlignCenter,
    AlignRight,
    AlignTop = AlignLeft,
    AlignBottom = AlignRight
};

typedef std::string FontIdType;

//----------------------------------------//
// Klasse für das Verwalten von Schriften //
//----------------------------------------//
class FontManager
{
public:
    typedef std::map< FontIdType,boost::shared_ptr<FTFont> > FontMap;

    FontManager();
    ~FontManager();
    void LoadFont( const char* fileName, int size, FontIdType id );
    void FreeFont( FontIdType id );

    // DrawString
    // x/y: upper left position of text box
    void DrawString(const std::string &str, const FontIdType &fontId, float x, float y, Align horizAlign, Align vertAlign,
            float red, float green, float blue, float alpha);

    void GetDimensions(const std::string &text, const FontIdType &fontId, float& w, float& h) const;

private:
    void GetDetailedDimensions(const std::string &text, FTFont& font, float* totalWidth, float* totalHeight, std::list<std::string>* lines,
            std::list<float>* lineWidths, float* lineSpacing) const;
    FontMap m_fonts; // Texturen
};

#endif

// Astro Attack - Christian Zommerfelds - 2009

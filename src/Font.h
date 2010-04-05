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
#include <boost/shared_ptr.hpp>

typedef std::string FontIdType;

#define ORIENT_CCW true   // Gegenuhrzeigersiin
#define ORIENT_CW false   // Uhrzeigersinn

struct Box;
struct Font;

enum Align
{
    AlignLeft,
    AlignCenter,
    AlignRight
};

//----------------------------------------//
// Klasse für das Verwalten von Schriften //
//----------------------------------------//
class FontManager
{
public:
    FontManager();
    ~FontManager();
    void LoadFont( const char* fileName, int size, FontIdType id );
    void FreeFont( FontIdType id );
    void DrawString(const std::string &str, float x, float y, const FontIdType &fontId, bool orientation, float alpha, Align horizAlign, Align vertAlign );
    void GetDimensionsOfText(const std::string &str, float& w, float& h, const FontIdType &fontId) const;

private:
    std::map< FontIdType,boost::shared_ptr<Font> > m_fonts;      // Texturen
};

// ---- STRUKTUREN ----

// Ein Rechteck
struct Box
{
    int x,y;
    int w,h;
};

// Informationen einer Schrift
struct Font
{
    int width, height;  // Breite und Höhe der Textur, wo die Zeichen gespeichert sind
    Box coord[256];     // Genaue Box für jedes Zeichen damit man weiss wo genau das Zeichen in der Textur liegt.
    bool isValid[256];  // gültiger Symbol? Kann man es Zeichnen?
    unsigned int texId; // OpenGL Texturadresse (wo die Font-Bilder gespeichert sind), das gleiche wie GLuint
    int lineSkip;       // Wie viele Punkte der Abstand zwischen zwei Zeilen dieser Schrift beträgt
    ~Font();            // Wenn diese Klasse gelöscht wird, wird auch die Textur in OpenGL gelöscht.
};

#endif

// Astro Attack - Christian Zommerfelds - 2009

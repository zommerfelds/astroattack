/*----------------------------------------------------------\
|                        Font.cpp                           |
|                        --------                           |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (für Linux Systeme)

#include "Font.h"

// Simple DirectMedia Layer (freie Plattform-Übergreifende Multimedia-Programmierschnittstelle)
#include <SDL/SDL.h>
// OpenGL via SDL inkludieren (Plattform-Übergreifende Definitionen)
#include <SDL/SDL_opengl.h>

//#include "contrib/utfcpp/utf8.h"
#include <FTGL/ftgl.h>

#include <utility>
#include <boost/make_shared.hpp>

#include "main.h"

FontManager::FontManager()
{
}

FontManager::~FontManager()
{
}

int PowerOf2(int num)
{
  int value = 1;

  while (value < num)
  {
    value <<= 1;
  }
  return value;
}

// .ttf Datei in in OpenGL Texturen laden laden
void FontManager::LoadFont( const char* fileName, int size, FontIdType id )
{
    if ( m_fonts.find(id) != m_fonts.end() )
        return;

    // Create a pixmap font from a TrueType file.
    boost::shared_ptr<FTTextureFont> font(boost::make_shared<FTTextureFont>(fileName) );

    // If something went wrong, return
    if(font->Error())
        return;

    // Set the font size and render a small text.
    font->FaceSize(size);

    m_fonts.insert( std::make_pair(id, font) );
}

void FontManager::FreeFont( FontIdType id )
{
    FontMap::iterator c_it = m_fonts.find( id );
    if ( c_it != m_fonts.end() )
        m_fonts.erase( c_it ); 
}

//const float s = 0.0025f;
//const float line_height = 1.2f;

void FontManager::DrawString(const std::string &str, const FontIdType &fontId, float x, float y, Align horizAlign, Align vertAlign, float red, float green, float blue, float alpha )
{
    FontMap::iterator font_it = m_fonts.find( fontId );
    assert ( font_it != m_fonts.end() );

    // convert to FTGL coordinates
    x = x / 4.0f * gAaConfig.GetInt("ScreenWidth");
    y = (1.0f - y / 3.0f) * gAaConfig.GetInt("ScreenHeight");

    FTFont* font = font_it->second.get();

    glColor4f( red, green, blue, alpha );

    std::list<std::string> lines;
    std::list<float> lineWidths;
    float lineSpacing = 0.0f;
    GetDetailedDimensions(str, *font, NULL, NULL, &lines, &lineWidths, &lineSpacing);

    float lineY = 0.0f;
    switch ( vertAlign )
    {
    case AlignTop:
        lineY = y - font->Ascender();
        break;
    case AlignCenter:
        lineY = y + (lineSpacing * (lines.size() - 1) - font->Ascender() - font->Descender()) / 2;
        break;
    case AlignBottom:
        lineY = y + lineSpacing * (lines.size() - 1) - font->Descender();
        break;
    }

    float lineX = 0.0f;
    std::list<float>::iterator lineWidthsIt = lineWidths.begin();
    for (std::list<std::string>::iterator lineIt=lines.begin(); lineIt != lines.end(); lineIt++, lineWidthsIt++)
    {
        switch ( horizAlign )
        {
        case AlignLeft:
            lineX = x;
            break;
        case AlignCenter:
            lineX = x-*lineWidthsIt/2;
            break;
        case AlignRight:
            lineX = x-*lineWidthsIt;
            break;
        }
        font->Render(lineIt->c_str(),-1,FTPoint(lineX, lineY));
        lineY -= lineSpacing;
    }
}

void FontManager::GetDimensions(const std::string &text, const FontIdType &fontId, float& w, float& h) const
{

    FontMap::const_iterator font_it = m_fonts.find( fontId );
    if ( font_it == m_fonts.end() )
        return; // TODO: log error

    GetDetailedDimensions(text, *font_it->second.get(), &w, &h, NULL, NULL, NULL);
    w = w/gAaConfig.GetInt("ScreenWidth")*4.0f;
    h = h/gAaConfig.GetInt("ScreenHeight")*3.0f;
}

void FontManager::GetDetailedDimensions(const std::string &text, FTFont& font, float* totalWidth, float* totalHeight,
        std::list<std::string>* lines, std::list<float>* lineWidths, float* lineSpacing) const
{
    int line_count = 1;
    float cur_width = 0.0f;
    float max_width = 0.0f;
    for(size_t cur_pos=0, last_pos=0;; last_pos=cur_pos, line_count++)
    {
        cur_pos=text.find('\n',cur_pos); // find next new line
        if (cur_pos==std::string::npos)
            cur_pos=text.size();

        std::string line = text.substr(last_pos,cur_pos-last_pos); // extract the next line

        cur_width = font.BBox(line.c_str(), -1).Upper().Xf(); // get line size

        max_width = std::max(cur_width, max_width);

        if (lines)
            lines->push_back( line );
        if (lineWidths)
            lineWidths->push_back(cur_width);

        cur_pos += 1;
        if (cur_pos>=text.size())
            break;
    }

    if (totalWidth)
        *totalWidth = max_width;

    float line_spacing = font.LineHeight()*0.8;
    if (lineSpacing)
        *lineSpacing = line_spacing;
    if (totalHeight) {
        *totalHeight = ((line_count-1) * line_spacing) + font.Ascender() - font.Descender();
    }
}

// Astro Attack - Christian Zommerfelds - 2009

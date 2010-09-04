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
    boost::shared_ptr<FTTextureFont> font(new FTTextureFont(fileName) );

    // If something went wrong, return
    if(font->Error())
        return;

    // Set the font size and render a small text.
    font->FaceSize(size);

    m_fonts.insert( std::make_pair(id, font) );
}

void FontManager::FreeFont( FontIdType id )
{
    std::map< FontIdType,boost::shared_ptr<FTFont> >::iterator c_it = m_fonts.find( id );
    if ( c_it != m_fonts.end() )
        m_fonts.erase( c_it ); 
}

//const float s = 0.0025f;
//const float line_height = 1.2f;

void FontManager::DrawString(const std::string &str, const FontIdType &fontId, float x, float y, Align horizAlign, Align vertAlign, float red, float green, float blue, float alpha )
{
    std::map< FontIdType,boost::shared_ptr<FTFont> >::iterator font_it = m_fonts.find( fontId );
    assert ( font_it != m_fonts.end() );

    FTFont* font = font_it->second.get();
    
    glColor4f( red, green, blue, alpha );

    float w,h;
    //GetDimensionsOfText( str, fontId, w, h );

    std::list<std::string> lines;
    std::list<float> lineWidths;
    float lineSpacing = 0.0f;
    GetDetailedDimensionsOfTextLines(str, fontId, w, h, &lines, &lineWidths, NULL, &lineSpacing);

    /*switch ( horizAlign )
    {
    case AlignLeft:
        break;
    case AlignCenter:
        x -= w/2;
        break;
    case AlignRight:
        x -= w;
        break;
    }*/
    switch ( vertAlign )
    {
    case AlignTop:
        y += h;
        break;
    case AlignCenter:
        y += h/2;
        break;
    case AlignBottom:
        break;
    }

    float lineX = 0.0f;
    float lineY = y-lineSpacing*(lines.size()-1);
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
        font->Render(lineIt->c_str(),-1,FTPoint(lineX/4.0*gAaConfig.GetInt("ScreenWidth"),(1.0-lineY/3.0)*gAaConfig.GetInt("ScreenHeight")));
        lineY += lineSpacing;
    }
}

void FontManager::GetDimensionsOfText(const std::string &text, const FontIdType &fontId, float& w, float& h) const
{
    GetDetailedDimensionsOfTextLines(text, fontId, w, h, NULL, NULL, NULL, NULL);
}

void FontManager::GetDetailedDimensionsOfTextLines(const std::string &text, const FontIdType &fontId, float& totalWidth, float& totalHeight, std::list<std::string>* lines, std::list<float>* lineWidths, std::list<float>* lineHeights, float* lineSpacing) const
{
    std::map< FontIdType,boost::shared_ptr<FTFont> >::const_iterator font_it = m_fonts.find( fontId );
    assert ( font_it != m_fonts.end() );

    FTFont* font = font_it->second.get();

    int lineCount = 0;
    float cur_width = 0.0f;
    float max_width = 0.0f;
    float last_line_height = 0.0f;
    for(size_t cur_pos=0,last_pos=0;;last_pos=cur_pos)
    {
        lineCount++;
        cur_pos=text.find('\n',cur_pos);

        if (cur_pos==std::string::npos)
            cur_pos=text.size();
        std::string line = text.substr(last_pos,cur_pos-last_pos);
        FTBBox bbox = font->BBox(line.c_str(),-1);
        cur_width = bbox.Upper().Xf();
        if (cur_width>max_width)
            max_width=cur_width;

        if (lines != NULL)
            lines->push_back( line );
        if (lineWidths != NULL)
            lineWidths->push_back(cur_width/gAaConfig.GetInt("ScreenWidth")*4.0f);
        if (lineHeights != NULL)
            lineHeights->push_back(bbox.Upper().Yf()/gAaConfig.GetInt("ScreenHeight")*3.0f);

        if (cur_pos>=text.size())
        {
            last_line_height = bbox.Upper().Yf();
            break;
        }
        cur_pos += 1;
    }
    totalWidth = max_width/gAaConfig.GetInt("ScreenWidth")*4.0f;
    float line_spacing = font->LineHeight();
    if (lineSpacing != NULL)
        *lineSpacing = line_spacing/gAaConfig.GetInt("ScreenHeight")*3.0f;
    totalHeight = ((lineCount-1)*line_spacing+last_line_height)/gAaConfig.GetInt("ScreenHeight")*3.0f;

    /*FTBBox bbox = font->BBox(str.c_str(),-1);
    totalWidth = bbox.Upper().Xf()/gAaConfig.GetInt("ScreenWidth")*4.0f;
    totalHeight = bbox.Upper().Yf()/gAaConfig.GetInt("ScreenHeight")*3.0f;*/
}

// Astro Attack - Christian Zommerfelds - 2009

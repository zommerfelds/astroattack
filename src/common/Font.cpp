/*
 * Font.cpp
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

#include <utility>
#include <SDL_opengl.h>
#include <FTGL/ftgl.h>
#include <boost/make_shared.hpp>

#include "Font.h"
#include "game/Configuration.h"

// .ttf Datei in in OpenGL Texturen laden laden
void FontManager::loadFont( const std::string& fileName, int size, FontId id )
{
    if ( m_fonts.find(id) != m_fonts.end() )
        return;

    // Create a pixmap font from a TrueType file.
    boost::shared_ptr<FTTextureFont> font(boost::make_shared<FTTextureFont>(fileName.c_str()) );

    // If something went wrong, return
    if(font->Error())
        return;

    // Set the font size and render a small text.
    font->FaceSize(size);

    m_fonts.insert( std::make_pair(id, font) );
}

void FontManager::freeFont( FontId id )
{
    FontMap::iterator c_it = m_fonts.find( id );
    if ( c_it != m_fonts.end() )
        m_fonts.erase( c_it ); 
}

void FontManager::drawString(const std::string &str, const FontId &fontId, float x, float y, Align horizAlign, Align vertAlign, float red, float green, float blue, float alpha )
{
    FontMap::iterator font_it = m_fonts.find( fontId );
    assert ( font_it != m_fonts.end() );

    // convert to FTGL coordinates
    x = x / 4.0f * gConfig.get<int>("ScreenWidth");
    y = (1.0f - y / 3.0f) * gConfig.get<int>("ScreenHeight");

    FTFont* font = font_it->second.get();

    glColor4f( red, green, blue, alpha );

    std::list<std::string> lines;
    std::list<float> lineWidths;
    float lineSpacing = 0.0f;
    getDetailedDimensions(str, *font, NULL, NULL, &lines, &lineWidths, &lineSpacing);

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
    glColor4f( 255, 255, 255, 255 );
}

void FontManager::getDimensions(const std::string &text, const FontId &fontId, float& w, float& h) const
{

    FontMap::const_iterator font_it = m_fonts.find( fontId );
    if ( font_it == m_fonts.end() )
        return; // TODO: log error

    getDetailedDimensions(text, *font_it->second.get(), &w, &h, NULL, NULL, NULL);
    w = w/gConfig.get<int>("ScreenWidth")*4.0f;
    h = h/gConfig.get<int>("ScreenHeight")*3.0f;
}

void FontManager::getDetailedDimensions(const std::string &text, FTFont& font, float* totalWidth, float* totalHeight,
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

    float line_spacing = font.LineHeight()*0.8f;
    if (lineSpacing)
        *lineSpacing = line_spacing;
    if (totalHeight) {
        *totalHeight = ((line_count-1) * line_spacing) + font.Ascender() - font.Descender();
    }
}
/*
 * Font.cpp
 * This file is part of Astro Attack
 * Copyright 2012 Christian Zommerfelds
 */

#include "Font.h"

#include "Logger.h"
#include "Renderer.h"
#include "Foreach.h"

#include <utility>
#include <FTGL/ftgl.h>
#include <SDL_opengl.h>
#include <boost/make_shared.hpp>

FontManager::FontManager(const RenderSystem& renderer)
: m_renderer (renderer)
{}

void FontManager::reloadFonts()
{
    foreach (FontMap::value_type& v, m_fonts)
    {
        FontData& font = v.second;
        if (!font.fix)
            loadFontRel(font.fileName, font.sizeF, v.first);
    }
}


void FontManager::loadFontFix(const std::string& fileName, unsigned int size, const FontId& id)
{
    loadFont(fileName, 0.0f, size, id, true);
}

void FontManager::loadFontRel(const std::string& fileName, float size, const FontId& id)
{
    loadFont(fileName, size, 0, id, false);
}

void FontManager::loadFont(const std::string& fileName, float sizeF, unsigned int sizeI, const FontId& id, bool fix)
{
    unsigned int ftglSize = sizeI;
    if (!fix)
        ftglSize = (unsigned int)(sizeF / 3.0f * m_renderer.getViewPortHeight());

    /*if ( m_fonts.count( id )==1 )
    {
        log(Warning) << "Loading font: ID '" << id << "' exists already, new font was not loaded\n";
        return;
    }*/ // reloadFonts uses this method

    // Create a pixmap font from a TrueType file.
    boost::shared_ptr<FTTextureFont> ftglFont = boost::make_shared<FTTextureFont>(fileName.c_str());

    // If something went wrong, return
    if(ftglFont->Error())
    {
        log(Error) << "Error loading font <" << fileName << ">\n";
        return;
    }

    // textures are generated now
    ftglFont->FaceSize(ftglSize);

    if (fix)
        m_fonts[id] = FontData(ftglFont, fileName, sizeI, fix);
        //m_fonts.insert( std::make_pair(id, Font(ftglFont, fileName, sizeI, fix)) );
    else
        m_fonts[id] = FontData(ftglFont, fileName, sizeF, fix);
}

void FontManager::freeFont(const FontId& id)
{
    FontMap::iterator it = m_fonts.find(id);
    if (it != m_fonts.end())
        m_fonts.erase(it);
}

void FontManager::drawString(const std::string &str, const FontId &fontId, float x, float y, Align horizAlign, Align vertAlign, float red, float green, float blue, float alpha )
{
    FontMap::iterator font_it = m_fonts.find( fontId );
    assert ( font_it != m_fonts.end() );

    FTFont* font = font_it->second.ftFont.get();

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
        font->Render(lineIt->c_str(), -1, FTPoint(lineX, lineY));
        lineY -= lineSpacing;
    }
    glColor4f( 255, 255, 255, 255 );
}

void FontManager::getDimensions(const std::string &text, const FontId &fontId, float& w, float& h) const
{
    FontMap::const_iterator font_it = m_fonts.find( fontId );
    if ( font_it == m_fonts.end() )
    {
        log(Warning) << "FontManager::getDimension: font '" << fontId << "'\n";
        return;
    }

    getDetailedDimensions(text, *font_it->second.ftFont.get(), &w, &h, NULL, NULL, NULL);
    w = w/m_renderer.getViewPortWidth()*4.0f;
    h = h/m_renderer.getViewPortHeight()*3.0f;
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

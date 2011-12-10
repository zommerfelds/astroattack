/*
 * Font.h
 * This file is part of Astro Attack
 * Copyright 2011 Christian Zommerfelds
 */

// Verwaltung von Schriften

#ifndef FONT_H
#define FONT_H

#include <map>
#include <list>
#include <string>
#include <boost/shared_ptr.hpp>

struct Box;
class FTFont;
class RenderSubSystem;

enum Align
{
    AlignLeft,
    AlignCenter,
    AlignRight,
    AlignTop = AlignLeft,
    AlignBottom = AlignRight
};

typedef std::string FontId;

//----------------------------------------//
// Klasse für das Verwalten von Schriften //
//----------------------------------------//
class FontManager
{
public:
    FontManager(const RenderSubSystem& renderer);

    typedef std::map< FontId,boost::shared_ptr<FTFont> > FontMap;

    void loadFont( const std::string& fileName, int size, FontId id );
    void freeFont( FontId id );

    // DrawString
    // x/y: upper left position of text box
    void drawString(const std::string& str, const FontId &fontId, float x, float y, Align horizAlign, Align vertAlign,
            float red, float green, float blue, float alpha);

    void getDimensions(const std::string &text, const FontId &fontId, float& w, float& h) const;

private:
    void getDetailedDimensions(const std::string &text, FTFont& font, float* totalWidth, float* totalHeight, std::list<std::string>* lines,
            std::list<float>* lineWidths, float* lineSpacing) const;
    FontMap m_fonts; // Texturen
    const RenderSubSystem& m_renderer;
};

#endif

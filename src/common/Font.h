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

    /**
     * This method loads a font at a fix pixel size. (appears smaller in bigger resolutions)
     * @param size Size of the font in pixels
     */
    void loadFontFix(const std::string& fileName, unsigned int size, const FontId& id);
    /**
     * This method loads a font which size is independent of screen resolution
     * @param size Size of the font in GUI coordinates
     */
    void loadFont(const std::string& fileName, float size, const FontId& id);

    void freeFont(const FontId&);

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

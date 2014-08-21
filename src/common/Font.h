/*
 * Font.h
 * This file is part of Astro Attack
 * Copyright (c) 2014 Christian Zommerfelds
 */

// Verwaltung von Schriften

#ifndef FONT_H
#define FONT_H

#include <map>
#include <list>
#include <string>
#include <boost/shared_ptr.hpp>

class Box;
class FTFont;
class RenderSystem;

enum Align
{
    AlignLeft,
    AlignCenter,
    AlignRight,
    AlignTop = AlignLeft,
    AlignBottom = AlignRight
};

typedef std::string FontId;

class FontData {
public:
    FontData() : fix (false), sizeF (0.0f) {}
    FontData(boost::shared_ptr<FTFont> font, const std::string fileName, float size, bool fix)
        : ftFont (font), fileName (fileName), fix (fix), sizeF (size) {}
    FontData(boost::shared_ptr<FTFont> font, const std::string fileName, unsigned int size, bool fix)
        : ftFont (font), fileName (fileName), fix (fix), sizeI (size) {}

    boost::shared_ptr<FTFont> ftFont;
    std::string fileName;
    bool fix;
    union {
        float sizeF;
        unsigned int sizeI;
    };
};

//----------------------------------------//
// Klasse für das Verwalten von Schriften //
//----------------------------------------//
class FontManager
{
public:
    FontManager(const RenderSystem& renderer);

    typedef std::map<FontId, FontData> FontMap;

    /**
     * This method loads a font at a fix pixel size. (appears smaller in bigger resolutions)
     * @param size Size of the font in pixels
     */
    void loadFontFix(const std::string& fileName, unsigned int size, const FontId& id);
    /**
     * This method loads a font which size is independent of screen resolution
     * @param size Size of the font in GUI coordinates
     */
    void loadFontRel(const std::string& fileName, float size, const FontId& id);

    void freeFont(const FontId&);

    void reloadFonts();

    // DrawString
    // x/y: upper left position of text box in text coordinates (0/0 is lower left, 1 pixel per unit)
    void drawString(const std::string& str, const FontId &fontId, float x, float y, Align horizAlign, Align vertAlign,
            float red, float green, float blue, float alpha);

    void getDimensions(const std::string &text, const FontId &fontId, float& w, float& h) const;

private:

    void loadFont(const std::string& fileName, float sizeF, unsigned int sizeI, const FontId& id, bool fix);

    void getDetailedDimensions(const std::string &text, FTFont& font, float* totalWidth, float* totalHeight, std::list<std::string>* lines,
            std::list<float>* lineWidths, float* lineSpacing) const;
    FontMap m_fonts; // FTGL Fonts - are unloaded automatically upon destruction
    const RenderSystem& m_renderer;
};

#endif

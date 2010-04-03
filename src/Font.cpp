/*----------------------------------------------------------\
|                        Font.cpp                           |
|                        --------                           |
|               Quelldatei von Astro Attack                 |
|                  Christian Zommerfelds                    |
|                          2009                             |
\----------------------------------------------------------*/

#include "GNU_config.h" // GNU Compiler-Konfiguration einbeziehen (fÃ¼r Linux Systeme)

#include "Font.h"
#include <SDL/SDL_ttf.h>

// Simple DirectMedia Layer (freie Plattform-Ãbergreifende Multimedia-Programmierschnittstelle)
#include <SDL/SDL.h>
// OpenGL via SDL inkludieren (Plattform-Ãbergreifende Definitionen)
#include <SDL/SDL_opengl.h>

FontManager::FontManager()
{
    // SDL_ttf initialisieren
    TTF_Init();
}

FontManager::~FontManager()
{
    TTF_Quit();
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

    TTF_Font *font;

    // Font Ã¶ffnen (.ttf)
    font = TTF_OpenFont( fileName, size );
    
    // Falls fehler
    if( font == NULL )
    {
        return;
    }

    int textureWidth = 512;
    int textureHeight = 0;
    int fontHeight = TTF_FontHeight( font )+1;
    int numLines = 1;
    Font* newFont = m_fonts.insert( std::pair< FontIdType,boost::shared_ptr<Font> >(id, boost::shared_ptr<Font>(new Font) ) ).first->second.get();
    
    newFont->lineSkip = TTF_FontLineSkip(font);

    for ( int i = 0; i < 256; ++i )
    {
        newFont->coord[i].x = 0;
        newFont->coord[i].y = 0;
        newFont->coord[i].w = 0;
        newFont->coord[i].h = 0;
        newFont->isValid[i] = false;
    }
    int currentWidth = 0;
    for ( int i = 32; i < 256; ++i )
    {
        int w = 0;
        int h = 0;
        const char c[2] = {(char)i,'\0'};
        TTF_SizeText( font, c, &w, &h );

        if ( currentWidth+w > textureWidth )
        {
            ++numLines;
            currentWidth = 0;
        }

        newFont->coord[i].w = w;
        newFont->coord[i].h = h;
        newFont->coord[i].x = currentWidth;
        newFont->coord[i].y = fontHeight*(numLines-1);
        if ( newFont->coord[i].w > 0 && newFont->coord[i].h > 0 )
            newFont->isValid[i] = true;
        //textureWidth += w;
        
        currentWidth += w;
    }

    //textureWidth = PowerOf2(textureWidth);
    textureHeight = PowerOf2( numLines*fontHeight );
    if ( !(textureWidth == 0 || textureHeight == 0) )
    {        
        newFont->width = textureWidth;
        newFont->height = textureHeight;
        SDL_Surface* sdlFontSurface = SDL_CreateRGBSurface(SDL_SWSURFACE,
				     textureWidth, textureHeight,
				     32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN // RGBA Masken
				     0x000000FF, 
				     0x0000FF00, 
				     0x00FF0000, 
				     0xFF000000
#else
				     0xFF000000,
				     0x00FF0000, 
				     0x0000FF00, 
				     0x000000FF
#endif
				     );

        for ( int i = 31; i < 256; ++i )
        {
            if ( !newFont->isValid[i] )
                continue;
            const char c[2] = {(char)i,'\0'};
            //SDL_Color textColor = {0x00, 0x00, 0x00};
            SDL_Color textColor = {0xFF, 0xFF, 0xFF};
            SDL_Surface *sdlCharSurface = TTF_RenderText_Blended( font, c, textColor );
            if( sdlCharSurface == NULL )
                continue;

            SDL_Rect offset;
            offset.x = (Sint16)newFont->coord[i].x;
            offset.y = (Sint16)newFont->coord[i].y;

            SDL_SetAlpha( sdlCharSurface, 0, 0 );
            SDL_BlitSurface( sdlCharSurface, NULL, sdlFontSurface, &offset );
            SDL_FreeSurface( sdlCharSurface );
        }
        // Falls man das Resultat der bisherigen Befehlen sehen mÃ¶chte, kann man die folgende Zeile unkommentieren:
        //SDL_SaveBMP(sdlFontSurface,"font.bmp");

        // OpenGL Textur aufbauen
        glGenTextures(1, &newFont->texId);
        glBindTexture(GL_TEXTURE_2D, newFont->texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               textureWidth, textureHeight,
               0,
               GL_RGBA,
               GL_UNSIGNED_BYTE,
               sdlFontSurface->pixels);

        SDL_FreeSurface( sdlFontSurface );
    }

    TTF_CloseFont(font);
}

void FontManager::FreeFont( FontIdType id )
{
    std::map< FontIdType,boost::shared_ptr<Font> >::iterator c_it = m_fonts.find( id );
    if ( c_it != m_fonts.end() )
        m_fonts.erase( c_it );        
}

const float s = 0.0025f;

// only AlignLeft!
void FontManager::DrawString(const std::string &str, float x, float y, const FontIdType &fontId, bool orientation, float alpha, Align horizAlign, Align vertAlign )
{
    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glColor4f( 1.0f, 1.0f, 1.0f, alpha );

    float w,h;
    GetDimensionsOfText( str, w, h, fontId );

    switch ( horizAlign )
    {
    case AlignLeft:
        break;
    case AlignCenter:
        x -= w/2;
        break;
    case AlignRight:
        x -= w;
        break;
    }
    switch ( vertAlign )
    {
    case AlignLeft:
        break;
    case AlignCenter:
        y -= h/2;
        break;
    case AlignRight:
        y -= h;
        break;
    }

    std::map< FontIdType,boost::shared_ptr<Font> >::iterator font_it = m_fonts.find( fontId );
    if ( font_it == m_fonts.end() )
        return;
    glBindTexture(GL_TEXTURE_2D, font_it->second->texId );

    float cursorPosX = x;
    float cursorPosY = y;

    glBegin ( GL_QUADS );

    for ( unsigned int i = 0; i < str.size(); ++i )
    {
        const char c = str[i];
        int c_i = c;
        if ( c_i < 0 )
            c_i += 256;

        if ( c == '\n' || (c == '\\' && (i+1 < str.size()) && str[i+1] == 'n') )
        {
            cursorPosX = x;
            cursorPosY += font_it->second->lineSkip*s;
            if ( c == '\\' )
                ++i;
            continue;
        }

        float tex_x1 = font_it->second->coord[ c_i ].x / (float)font_it->second->width;
        float tex_x2 = tex_x1 + font_it->second->coord[ c_i ].w / (float)font_it->second->width;

        float tex_y1 = font_it->second->coord[ c_i ].y / (float)font_it->second->height;
        float tex_y2 = tex_y1 + font_it->second->coord[c_i ].h / (float)font_it->second->height;

        // Oben links
        if ( orientation == ORIENT_CW )
            glTexCoord2f(tex_x1, tex_y1);
        else
            glTexCoord2f(tex_x1, tex_y2);
        glVertex2f(cursorPosX, cursorPosY);

        if ( orientation == ORIENT_CW )
        {
            // Unten links
            glTexCoord2f(tex_x1, tex_y2);
            glVertex2f(cursorPosX, cursorPosY + font_it->second->coord[ c_i ].h*s);
        }
        else
        {
            // Oben rechts
            glTexCoord2f(tex_x2, tex_y2);
            glVertex2f(cursorPosX + font_it->second->coord[ c_i ].w*s, cursorPosY);
        }

        // Unten rechts
        if ( orientation == ORIENT_CW )
            glTexCoord2f(tex_x2, tex_y2);
        else
            glTexCoord2f(tex_x2, tex_y1);
        glVertex2f(cursorPosX + font_it->second->coord[ c_i ].w*s, cursorPosY + font_it->second->coord[ c_i ].h*s);

        if ( orientation == ORIENT_CW )
        {
            // Oben rechts
            glTexCoord2f(tex_x2, tex_y1);
            glVertex2f(cursorPosX + font_it->second->coord[ c_i ].w*s, cursorPosY);
        }
        else
        {
            // Unten links
            glTexCoord2f(tex_x1, tex_y1);
            glVertex2f(cursorPosX, cursorPosY + font_it->second->coord[ c_i ].h*s);
        }
        
        cursorPosX += font_it->second->coord[ c_i ].w*s;
    }
    glEnd();

    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
}

void FontManager::GetDimensionsOfText(const std::string &str, float& w, float& h, const FontIdType &fontId) const
{
    std::map< FontIdType,boost::shared_ptr<Font> >::const_iterator font_it = m_fonts.find( fontId );
    if ( font_it == m_fonts.end() )
        return;

    w = 0.0f;
    h = -1.0f;
    float max_w = 0.0f;

    for ( unsigned int i = 0; i < str.size(); ++i )
    {
        const char c = str[i];
        int c_i = c;
        if ( c_i < 0 )
            c_i += 256;

        if ( h < 0.0f )
            h = font_it->second->coord[ c_i ].h*s;

        if ( c == '\n' || (c == '\\' && (i+1 < str.size()) && str[i+1] == 'n') )
        {
            h += font_it->second->lineSkip*s;
            if ( c == '\\' )
                ++i;
            if ( w > max_w )
                max_w = w;
            w = 0.0f;
            continue;
        }

        w += font_it->second->coord[ c_i ].w*s;
    }
    if ( w < max_w )
        w = max_w;
}

Font::~Font()
{
    GLint curTexId = 0;
    glGetIntegerv( GL_TEXTURE_BINDING_2D, &curTexId );

    if ( curTexId == (GLint)texId )
        glDisable( GL_TEXTURE_2D );
    glDeleteTextures(1, &texId);
}

// Astro Attack - Christian Zommerfelds - 2009

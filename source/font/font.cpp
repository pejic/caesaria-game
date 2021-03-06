// This file is part of CaesarIA.
//
// CaesarIA is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CaesarIA is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CaesarIA.  If not, see <http://www.gnu.org/licenses/>.
//
// Copyright 2012-2013 Dalerank, dalerankn8@gmail.com

#include "font.hpp"
#include "font_collection.hpp"
#include <GameGfx>
#include <GameLogger>
#include <GameCore>
#include <GameVfs>
#include "SDL_ttf.h"
#include "SDL_version.h"
#include <map>

using namespace gfx;

const char* Font::defname = "default";

class Font::Impl
{
public:
  TTF_Font *ttfFont = nullptr;
  SDL_Color color;
  int size = 0;
  int style = 0;
};


Font::Font() : _d(new Impl)
{
  _d->ttfFont = 0;
  _d->color = SDL_Color();
}

Font& Font::fallback(int size, bool italic, bool bold, const NColor& color)
{
  if (!isValid())
    *this = FontCollection::instance().getDefault(size, italic, bold ).withColor(color);

  return *this;
}

Font Font::create(const std::string& family, int size, bool italic, bool bold, const NColor& color)
{
  Font font;
  if (family == Font::defname)
    font = FontCollection::instance().getDefault(size, italic, bold);
  else
    font = FontCollection::instance().getFont(family, size, italic, bold );
  return font.withColor(color);
}

Font::Font( const Font& other ) : _d( new Impl )
{
  *this = other;
}

unsigned int Font::getWidthFromCharacter( unsigned int c ) const
{
  int minx, maxx, miny, maxy, advance;
  TTF_GlyphMetrics( _d->ttfFont, c, &minx, &maxx, &miny, &maxy, &advance );

  return advance;
}

unsigned int Font::kerningHeight() const {  return 3; }

int Font::getCharacterFromPos(const std::wstring& text, int pixel_x) const
{
  int x = 0;
  int idx = 0;

  while( text[idx] )
  {
    x += getWidthFromCharacter( text[idx] );

    if (x >= pixel_x)
      return idx;

    idx++;
  }

  return -1;
}

int Font::color() const
{
  int ret = 0;
#if SDL_MAJOR_VERSION>1
  ret = (_d->color.a << 24 ) + (_d->color.r << 16) + (_d->color.g << 8) + _d->color.b;
#else
  ret = (_d->color.unused << 24 ) + (_d->color.r << 16) + (_d->color.g << 8) + _d->color.b;
#endif
  return ret;
}

bool Font::isValid() const {  return _d->ttfFont != 0; }

Size Font::getTextSize( const std::string& text ) const
{
  int w=0, h=0;
  if( isValid() )
  {
    TTF_SizeUTF8( _d->ttfFont, text.c_str(), &w, &h );
  }

  return Size( w, h );
}

bool Font::operator!=( const Font& other ) const
{
  return !( _d->ttfFont == other._d->ttfFont );
}

Rect Font::getTextRect(const std::string& text, const Rect& baseRect,
                             align::Type horizontalAlign, align::Type verticalAlign )
{
  Rect resultRect;
  Size d = getTextSize( text );

  // justification
  switch (horizontalAlign)
  {
  case align::center:
    // align to h centre
    resultRect.setLeft( (baseRect.width()/2) - (d.width()/2) );
    resultRect.setRight( (baseRect.width()/2) + (d.width()/2) );
    break;
  case align::lowerRight:
    // align to right edge
    resultRect.setLeft( baseRect.width() - d.width() );
    resultRect.setRight( baseRect.width() );
    break;
  default:
    // align to left edge
    resultRect.setLeft( 0 );
    resultRect.setRight( d.width() );
  }

  switch (verticalAlign)
  {
  case align::center:
    // align to v centre
    resultRect.setTop( (baseRect.height()/2) - (d.height()/2) );
    resultRect.setBottom( (baseRect.height()/2) + (d.height()/2) );
    break;
  case align::lowerRight:
    // align to bottom edge
    resultRect.setTop( baseRect.height() - d.height() );
    resultRect.setBottom( baseRect.height() );
    break;
  default:
    // align to top edge
    resultRect.setTop( 0 );
    resultRect.setBottom( d.height() );
    break;
  }

  resultRect += baseRect.lefttop();

  return resultRect;
}

void Font::setColor( NColor color )
{
#ifdef GAME_PLATFORM_ANDROID
  color.set(color.abgr());
#endif
  _d->color.b = color.blue();
  _d->color.g = color.green();
  _d->color.r = color.red();
#if SDL_MAJOR_VERSION>1
  _d->color.a = color.alpha();
#else
  _d->color.unused = color.alpha();
#endif
}

Font Font::withColor(NColor color)
{
  Font ret( *this );
  ret.setColor( color );
  return ret;
}

void Font::_setHdc(void* ptr)
{
  _d->ttfFont = (TTF_Font*)ptr;
}

void Font::_setStyle(int style)
{
  _d->style = style;
}

void Font::draw( Picture& dstpic, const std::string &text, const int dx, const int dy, bool useAlpha, bool updatextTx )
{
  if( !_d->ttfFont || !dstpic.isValid() )
    return;

#if defined(GAME_PLATFORM_EMSCRIPTEN)
  SDL_Surface* sText = TTF_RenderText_Solid( _d->ttfFont, text.c_str(), _d->color );
#else
  SDL_Surface* sText = TTF_RenderUTF8_Blended( _d->ttfFont, text.c_str(), _d->color );
#endif

  if( sText )
  {
    if( useAlpha ) {
#if SDL_MAJOR_VERSION>1
      SDL_SetSurfaceBlendMode( sText, SDL_BLENDMODE_NONE );
#else
      SDL_SetAlpha( sText, 0, 0 );
#endif
    }

    if( !dstpic.surface() ) {
      Logger::warning("Font::draw dstpic surface is null");
      return;
    }

    SDL_Rect srcRect, dstRect;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = sText->w;
    srcRect.h = sText->h;
    dstRect.x = dx;
    dstRect.y = dy;
    dstRect.w = sText->w;
    dstRect.h = sText->h;

    SDL_BlitSurface( sText, &srcRect, dstpic.surface(), &dstRect );
    SDL_FreeSurface( sText );
  }

  if (updatextTx) {
    dstpic.update();
  }
}

void Font::draw(Picture &dstpic, const std::string &text, const Point& pos, bool useAlpha , bool updateTx)
{
  draw(dstpic, text, pos.x(), pos.y(), useAlpha, updateTx);
}

Picture Font::once(const std::string &text, bool mayChange)
{
  SDL_Surface* textSurface = TTF_RenderUTF8_Blended( _d->ttfFont, text.c_str(), _d->color );
  Picture ret( Size( textSurface->w, textSurface->h ), (unsigned char*)textSurface->pixels, mayChange );
  SDL_FreeSurface( textSurface );
  ret.update();

  return ret;
}

Font::~Font() {}

Font& Font::operator=( const Font& other )
{
  _d->ttfFont = other._d->ttfFont;
  _d->color = other._d->color;
  return *this;
}

Font Font::create(const std::string& alias)
{
  return FontCollection::instance().getFont(alias);
}

static StringArray _font_breakText(const std::string& text, const Font& f, int elWidth, bool RightToLeft )
{
  StringArray brokenText;

  Font font = f;

  if( !font.isValid() )
  {
    Logger::warning( "utils::breakText font must be exists" );
    brokenText.push_back( text );
    return brokenText;
  }

  std::string line;
  std::string word;
  std::string rwhitespace;
  std::string rText = text;
  int size = rText.size();
  int length = 0;

  char c;

  // We have to deal with right-to-left and left-to-right differently
  // However, most parts of the following code is the same, it's just
  // some order and boundaries which change.
  if( !RightToLeft)
  {
    // regular (left-to-right)
    for (int i=0; i<size; ++i)
    {
      c = rText[i];
      bool lineBreak = false;

      if( c == '\r' ) // Mac or Windows breaks
      {
        lineBreak = true;
        if (rText[i+1] == '\n') // Windows breaks
        {
          rText.erase(i+1);
          --size;
        }
        c = '\0';
      }
      else if (c == '\n') // Unix breaks
      {
        lineBreak = true;
        c = '\0';
      }

      bool isWhitespace = (c == ' ' || c == 0);
      if ( !isWhitespace )
      {
        // part of a word
        word += c;
      }

      if ( isWhitespace || i == (size-1))
      {
        if (word.size())
        {
          // here comes the next whitespace, look if
          // we must break the last word to the next line.
          const int whitelgth = font.getTextSize( rwhitespace ).width();
          const int wordlgth = font.getTextSize( word ).width();

          if (wordlgth > elWidth)
          {
            // This word is too long to fit in the available space, look for
            // the Unicode Soft HYphen (SHY / 00AD) character for a place to
            // break the word at
            int where = word.find_first_of( char(0xAD) );
            if (where != -1)
            {
              std::string first  = word.substr(0, where);
              std::string second = word.substr(where, word.size() - where);
              brokenText.push_back(line + first + "-");
              const int secondLength = font.getTextSize( second ).width();

              length = secondLength;
              line = second;
            }
            else
            {
              // No soft hyphen found, so there's nothing more we can do
              // break to next line
              if (length)
                brokenText.push_back(line);
              length = wordlgth;
              line = word;
            }
          }
          else if (length && (length + wordlgth + whitelgth > elWidth))
          {
            // break to next line
            brokenText.push_back(line);
            length = wordlgth;
            line = word;
          }
          else
          {
            // add word to line
            line += rwhitespace;
            line += word;
            length += whitelgth + wordlgth;
          }

          word = "";
          rwhitespace = "";
        }

        if ( isWhitespace )
        {
          rwhitespace += c;
        }

        // compute line break
        if (lineBreak)
        {
          line += rwhitespace;
          line += word;
          brokenText.push_back(line);
          line = "";
          word = "";
          rwhitespace = "";
          length = 0;
        }
      }
    }

    line += rwhitespace;
    line += word;
    brokenText.push_back(line);
  }
  else
  {
    // right-to-left
    for (int i=size; i>=0; --i)
    {
      c = rText[i];
      bool lineBreak = false;

      if(c == '\r') // Mac or Windows breaks
      {
        lineBreak = true;
        if( (i>0) && rText[i-1] == '\n' ) // Windows breaks
        {
          rText.erase(i-1);
          --size;
        }
        c = '\0';
      }
      else if (c == '\n') // Unix breaks
      {
        lineBreak = true;
        c = '\0';
      }

      if (c==' ' || c==0 || i==0)
      {
        if (word.size())
        {
          // here comes the next whitespace, look if
          // we must break the last word to the next line.
          const int whitelgth = font.getTextSize( rwhitespace ).width();
          const int wordlgth = font.getTextSize( word ).width();

          if (length && (length + wordlgth + whitelgth > elWidth))
          {
            // break to next line
            brokenText.push_back(line);
            length = wordlgth;
            line = word;
          }
          else
          {
            // add word to line
            line = rwhitespace + line;
            line = word + line;
            length += whitelgth + wordlgth;
          }

          word = "";
          rwhitespace = "";
        }

        if (c != 0)
          rwhitespace = std::string(&c, 1) + rwhitespace;

        // compute line break
        if (lineBreak)
        {
          line = rwhitespace + line;
          line = word + line;
          brokenText.push_back(line);
          line = "";
          word = "";
          rwhitespace = "";
          length = 0;
        }
      }
      else
      {
        // yippee this is a word..
        word = std::string(&c, 1) + word;
      }
    }

    line = rwhitespace + line;
    line = word + line;
    brokenText.push_back(line);
  }

  return brokenText;
}

StringArray Font::breakText(const std::string& text, int pixelLength)
{
  return _font_breakText( text, *this, pixelLength, false );
}

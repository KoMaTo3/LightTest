#pragma once

#include "windows.h"

typedef void          Void;
typedef bool          Bool;
typedef char          Char;
typedef unsigned char Byte;
typedef int           Int;
//typedef unsigned int  Word;
typedef unsigned short Word;
typedef short         Short;
typedef unsigned int  Dword;
typedef long          Long;
typedef unsigned long uLong;
typedef float         Float;
typedef double        Double;
typedef __int64       Int64;

typedef int HashValue;


#define ONEBY255  ( 1.0f / 255.0f )
#define KM_INLINE             __forceinline


struct Size
{
  Dword width;
  Dword height;

  Size() {}
  Size( const Size &newSize ) { this->height = newSize.height; this->width = newSize.width; }
  Size( const Dword &newWidth, const Dword &newHeight ) { this->width = newWidth; this->height = newHeight; }
  Size operator+( const Size& when ){ return Size( this->width + when.width, this->height + when.height ); }
};


template<class T>
struct Rect
{
  T     left,
        top,
        right,
        bottom;

  Rect() {}
  Rect( const Rect<T> &newSize )
    :left( newSize.left ), right( newSize.right ), top( newSize.top ), bottom( newSize.bottom )
  { }
  Rect( T _left, T _top, T _right, T _bottom )
    :left( _left ), top( _top ), right( _right ), bottom( _bottom )
  { }
};

typedef Rect<Float> SizeLTRB;


template
<class Type>
struct Pos
{
  Type x;
  Type y;

  Pos() {}
  Pos( const Pos<Type> &newPos ) { this->x = newPos.x; this->y = newPos.y; }
  Pos( const Type &newX, const Type &newY ) { this->x = newX; this->y = newY; }
  Pos& operator+=( Pos& by ) { this->x += by.x; this->y += by.y; return *this; }
  Pos  operator*( Type by ) { return Pos<Type>( this->x * by, this->y * by ); }
};


template
<class Type>
struct Tex
{
  Type u;
  Type v;

  Tex() {}
  Tex(const Tex<Type> &newTex) { this->u = newTex.u; this->v = newTex.v; }
  Tex(const Type &newU, const Type &newV) { this->u = newU; this->v = newV; }
  Tex<Type>& operator=( const Tex<Type> newTex ) { this->u = newTex.u; this->v = newTex.v; return *this; };
};


struct Color
{
  Float R, G, B, A;
  Color() {}

  Color( Float newR, Float newG, Float newB, Float newA )
  :R( newR ), G( newG ), B( newB ), A( newA )
  { }

  Color( Dword color_rgba )
  {
    this->FromRGBA( color_rgba );
  }//constructor

  Color* FromRGBA( Dword color_rgba )
  {
    this->R = Float( color_rgba % 0x100 ) * ONEBY255;
    this->G = Float( ( color_rgba >> 8 ) % 0x100 ) * ONEBY255;
    this->B = Float( ( color_rgba >> 16 ) % 0x100 ) * ONEBY255;
    this->A = Float( color_rgba >> 24 ) * ONEBY255;
    return this;
  }//FromRGBA

  inline Dword ToRGBA()
  {
    return Byte( this->R * 255.99f ) | ( Byte( this->G * 255.99f ) << 8 ) | ( Byte( this->B * 255.99f ) << 16 ) | ( Byte( this->A * 255.99f ) << 24 );
  }//ToRGBA

  const Bool operator==( const Color& withColor ) const
  {
    return (
      this->R == withColor.R &&
      this->G == withColor.G &&
      this->B == withColor.B &&
      this->A == withColor.A
    );
  }//==

  Color operator*( const Color& withColor )
  {
    return Color(
      this->R * withColor.R,
      this->G * withColor.G,
      this->B * withColor.B,
      this->A * withColor.A
    );
  }//*

};


enum { aOK    = 0,
       aERR   = -1,
       aERROR = aERR,
       aNEEDTORESETSCENE = 2,
     };

enum { MAX_PATH_LENGTH = 1024 };

#define alert(t) MessageBox(0,t,"",0)

#ifdef WIN32

#define DIR_DELIMITER     "\\"

#else

#define DIR_DELIMITER     "/"

#endif
#include "mouse.h"
#include "file.h"
#include "imageloader.h"
#include "tools.h"



Mouse::Mouse()
:windowSize( 1, 1 )
{
  this->cursor.position.Set( 0.0f, 0.0f );
  this->cursor.size.Set( 1.0f, 1.0f );
  this->cursor.isHardware = true;
  this->cursor.hCursor = NULL;
}//constructor



Mouse::~Mouse()
{
}//destructor




/*
=============
  Init
=============
*/
void Mouse::Init( const Size& newWindowSize )
{
  this->windowSize = newWindowSize;
  this->cursor.pixelsToTexels = Vec2( 100.0f / float( this->windowSize.width ), 100.0f / float( this->windowSize.height ) );
}//Init



/*
=============
  SetCursor
=============
*/
void Mouse::SetCursor( const std::string &imageFileName )
{
  this->cursor.isHardware = true;

  ImageLoader image;
  image.LoadFromFile( imageFileName );

  this->cursor.size.Set( float( image.GetImageSize()->width ) , float( image.GetImageSize()->height ) );
  this->cursor.spriteOffset.Set( this->cursor.size.x * 0.5f * this->cursor.pixelsToTexels.x, this->cursor.size.y * 0.5f * this->cursor.pixelsToTexels.y, 0.0f );

  ShowCursor( false );
}//SetCursor




/*
=============
  DestroyCursor
=============
*/
void Mouse::DestroyCursor() {
  ShowCursor( true );
}//DestroyCursor




/*
=============
  MoveCursor
=============
*/
void Mouse::MoveCursor( const Vec2& newPosition )
{
  this->cursor.position = newPosition;

  if( this->moveListeners.size() )
  {
    MoveListenerList::iterator iter, iterEnd = this->moveListeners.end();
    for( iter = this->moveListeners.begin(); iter != iterEnd; ++iter )
      ( *iter )( this->cursor.position );
  }
}//MoveCursor



/*
=============
  GetCursorPosition
=============
*/
const Vec2& Mouse::GetCursorPosition()
{
  return this->cursor.position;
}//GetCursorPosition



/*
=============
  AddMoveListener
=============
*/
void Mouse::AddMoveListener( MoveListener *listenerProc )
{
  if( listenerProc )
    this->moveListeners.push_back( listenerProc );
  else
    __log.PrintInfo( Filelevel_WARNING, "Mouse::AddMoveListener => function is NULL" );
}//AddMoveListener



/*
=============
  RemoveMoveListener
=============
*/
void Mouse::RemoveMoveListener( MoveListener *listenerProc )
{
  MoveListenerList::iterator iter, iterEnd = this->moveListeners.end();
  for( iter = this->moveListeners.begin(); iter != iterEnd; ++iter )
    if( *iter == listenerProc )
    {
      this->moveListeners.erase( iter );
      return;
    }
}//RemoveMoveListener




/*
=============
  _CreateCursorFromMemory
=============
*/
HCURSOR Mouse::_CreateCursorFromMemory( Byte *buffer, const Size *size, COLORREF clrTransparent, DWORD xHotspot, DWORD yHotspot )
{
  HCURSOR hRetCursor = NULL;

  HBITMAP hSourceBitmap = CreateBitmap( size->width, size->height, 4, 8, buffer );

  if( hSourceBitmap )
  {
    HBITMAP hAndMask = NULL;
    HBITMAP hXorMask = NULL;
    this->_GetMaskBitmaps( hSourceBitmap, clrTransparent, hAndMask, hXorMask );
    if( NULL == hAndMask || NULL == hXorMask ) {
      __log.PrintInfo( Filelevel_ERROR, "CreateCursorFromBitmap" );
      return NULL;
    }

    ICONINFO iconinfo = { 0 };
    iconinfo.fIcon    = FALSE;
    iconinfo.xHotspot = xHotspot;
    iconinfo.yHotspot = yHotspot;
    iconinfo.hbmMask  = hAndMask;
    iconinfo.hbmColor = hXorMask;

    hRetCursor = ::CreateIconIndirect( &iconinfo );
    ::DeleteObject( hSourceBitmap );
  }

  return hRetCursor;
}//_CreateCursorFromMemory


/*
=============
  ResetCursor
=============
*/
void Mouse::ResetCursor()
{
  if( this->cursor.isHardware && this->cursor.hCursor ) {
    ::SetCursor( this->cursor.hCursor );
  }
}//ResetCursor



/*
=============
  _GetMaskBitmaps
=============
*/
void Mouse::_GetMaskBitmaps( HBITMAP hSourceBitmap, COLORREF clrTransparent, HBITMAP &hAndMaskBitmap, HBITMAP &hXorMaskBitmap )
{
  HDC hDC         = ::GetDC( NULL );
  HDC hMainDC     = ::CreateCompatibleDC( hDC );
  HDC hAndMaskDC  = ::CreateCompatibleDC( hDC );
  HDC hXorMaskDC  = ::CreateCompatibleDC( hDC );

  BITMAP bm;
  ::GetObject( hSourceBitmap, sizeof( BITMAP ), &bm );

  hAndMaskBitmap  = ::CreateCompatibleBitmap( hDC, bm.bmWidth, bm.bmHeight );
  hXorMaskBitmap  = ::CreateCompatibleBitmap( hDC, bm.bmWidth, bm.bmHeight );

  HBITMAP hOldMainBitmap    = ( HBITMAP ) ::SelectObject( hMainDC, hSourceBitmap );
  HBITMAP hOldAndMaskBitmap = ( HBITMAP ) ::SelectObject( hAndMaskDC, hAndMaskBitmap );
  HBITMAP hOldXorMaskBitmap = ( HBITMAP ) ::SelectObject( hXorMaskDC, hXorMaskBitmap );

  COLORREF MainBitPixel;
  for( Int x = 0; x < bm.bmWidth; ++x ) {
    for( Int y = 0; y < bm.bmHeight; ++y ) {
      MainBitPixel = ::GetPixel( hMainDC, x, y );
      if( MainBitPixel == clrTransparent ) {
        ::SetPixel( hAndMaskDC, x, y, RGB( 255, 255, 255 ) );
        ::SetPixel( hXorMaskDC, x, y, RGB( 0, 0, 0) );
      } else {
        ::SetPixel( hAndMaskDC, x, y, RGB( 0, 0, 0) );
        ::SetPixel( hXorMaskDC, x, y, MainBitPixel );
      }
    }
  }
  this->cursor.size.Set( ( float ) bm.bmWidth, ( float ) bm.bmHeight );
  
  ::SelectObject( hMainDC, hOldMainBitmap );
  ::SelectObject( hAndMaskDC, hOldAndMaskBitmap );
  ::SelectObject( hXorMaskDC, hOldXorMaskBitmap );

  ::DeleteDC( hXorMaskDC );
  ::DeleteDC( hAndMaskDC );
  ::DeleteDC( hMainDC );

  ::ReleaseDC( NULL, hDC );
}//_GetMaskBitmaps


void Mouse::Update() {
  this->wheel = 0;
  this->ResetKeyStates();
}//Update

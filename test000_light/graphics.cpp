#include "time.h"
#include "stdlib.h"
#include <string>
#include "windows.h"
#include "gl/gl.h"
#include <vector>


#include "graphics.h"
#include "types.h"
#include "logs.h"
#include "tools.h"
#include "file.h"
#include "timer.h"

#define GRAPHICSEXTERNAL_EXTERN
#define GRAPHICSEXTERNAL_NULL = NULL
#include "graphicsexternal.h"


#pragma comment( lib, "opengl32.lib" )


extern File __log;


Graphics::Graphics( int width, int height )
:_isInitialized( false ), _screenSize( width, height ), _window( NULL ), _dc( NULL ), _glRC( NULL )
{
  this->Init();
}


Graphics::~Graphics() {
  this->Destroy();
}


bool Graphics::Init() {
  if( this->_isInitialized ) {
    return false;
  }
  srand( ( Word ) time( 0 ) );


  WNDCLASSEX wndClass;
  memset( &wndClass, 0, sizeof( wndClass ) );
  std::string className = "OpenGL graphics engine";

  wndClass.cbSize       = sizeof( wndClass );
  wndClass.style        = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  wndClass.lpfnWndProc  = Graphics::WindowProc;
  wndClass.cbClsExtra   = 0;
  wndClass.cbWndExtra   = 0 | DLGWINDOWEXTRA;
  wndClass.hInstance    = GetModuleHandle( NULL );
  wndClass.hIcon        = LoadIcon( NULL, IDI_APPLICATION );
  wndClass.hCursor      = 0;//LoadCursor( NULL, IDC_ARROW );
  wndClass.hbrBackground = ( HBRUSH ) GetStockObject( WHITE_BRUSH );
  wndClass.lpszMenuName = 0;
  wndClass.lpszClassName = className.c_str();
  wndClass.hIconSm = 0;
  if( !( RegisterClassEx( &wndClass ) ) ) {
    return false;
  }

  DWORD style   = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;  //WS_POPUP for fullscreen
  DWORD styleEx = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; //WS_EX_APPWINDOW for fullscreen
  RECT windowRect;
  windowRect.left   = 0;
  windowRect.top    = 0;
  windowRect.right  = this->_screenSize.width;
  windowRect.bottom = this->_screenSize.height;
  AdjustWindowRectEx( &windowRect, style, false, styleEx );

  this->_window = CreateWindowEx(
    styleEx,
    className.c_str(),
    "OpenGL",
    style | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
    100,
    100,
    windowRect.right - windowRect.left,
    windowRect.bottom - windowRect.top,
    0,
    NULL,
    GetModuleHandle( NULL ),
    NULL
  );

  SetWindowLong( this->_window, GWL_USERDATA, LONG( this ) );

  ShowWindow  ( this->_window, SW_NORMAL );
  UpdateWindow( this->_window);

  if( !this->_InitGraphics() ) {
    return false;
  }

  GL_CHECK_ERROR;

  this->_isInitialized = true;
  LOGD( "Graphics engine initialized\n" );
  return true;
}//Init


void Graphics::Destroy() {
  if( !this->_isInitialized ) {
    return;
  }
  if( this->_glRC ) {
    wglMakeCurrent( NULL, NULL );
    wglDeleteContext( this->_glRC );
    this->_glRC = NULL;
  }
  LOGD( "Graphics engine destroyed\n" );
}//Destroy




/*
=============
  CheckGLError
=============
*/
bool Graphics::CheckGLError( int line, const std::string& fileName )
{
  GLenum error = glGetError();
  if( error )
  {
    //__log.PrintInfo( Filelevel_ERROR, "OpenGL error at %s:%d: x%X [%d]", fileName.c_str(), line, error, error );
    LOGE( "[ERROR] OpenGL error at %s:%d: x%X [%d]\n", fileName.c_str(), line, error, error );
    return true;
  }
  return false;
}//CheckGLError



/*
=============
  _InitGraphics
  Инициализация OpenGL
=============
*/
bool Graphics::_InitGraphics()
{
  if( !( this->_dc = GetDC( this->_window ) ) )
  {
    __log.PrintInfo( Filelevel_CRITICALERROR, "Core::_InitGraphics => GetDC failed" );
    this->SetState( CORE_STATE_EXIT );
    return false;
  }
  this->_InitPixelFormat();
  this->_InitPalette();


  if( !( this->_glRC = ::wglCreateContext( this->_dc ) ) )
  {
    __log.PrintInfo( Filelevel_CRITICALERROR, "Core::_InitGraphics => wglCreateContext failed" );
    this->SetState( CORE_STATE_EXIT );
    return false;
  }

  //__log.PrintInfo( Filelevel_CRITICALERROR, "Core::InitGraphics => wglCreateContext" );
  ::wglMakeCurrent( this->_dc, this->_glRC );
  this->_InitViewport();

  const char* version = ( const char* ) ::glGetString( GL_VERSION );
  GL_CHECK_ERROR;
                                  //(допустим, debug-спрайт рендерится без текстуры)

  //this->_InitExtensions();

  glIsShader = ( PFNGLISSHADERPROC ) wglGetProcAddress( "glIsShader" );
  glGetShaderiv = ( PFNGLGETSHADERIVPROC ) wglGetProcAddress( "glGetShaderiv" );
  glGetShaderInfoLog = ( PFNGLGETSHADERINFOLOGPROC ) wglGetProcAddress( "glGetShaderInfoLog" );
  glCreateProgram = ( PFNGLCREATEPROGRAMPROC ) wglGetProcAddress( "glCreateProgram" );
  glAttachShader = ( PFNGLATTACHSHADERPROC ) wglGetProcAddress( "glAttachShader" );
  glLinkProgram = ( PFNGLLINKPROGRAMPROC ) wglGetProcAddress( "glLinkProgram" );
  glUseProgram = ( PFNGLUSEPROGRAMPROC ) wglGetProcAddress( "glUseProgram" );
  glGetUniformLocation = ( PFNGLGETUNIFORMLOCATIONPROC ) wglGetProcAddress( "glGetUniformLocation" );
  glUniform3f = ( PFNGLUNIFORM3FPROC ) wglGetProcAddress( "glUniform3f" );
  glUniform2f = ( PFNGLUNIFORM2FPROC ) wglGetProcAddress( "glUniform2f" );
  glGetAttribLocation = ( PFNGLGETATTRIBLOCATIONPROC ) wglGetProcAddress( "glGetAttribLocation" );
  glActiveTexture = ( PFNGLACTIVETEXTUREPROC ) wglGetProcAddress( "glActiveTexture" );
  glUniform1i = ( PFNGLUNIFORM1IPROC ) wglGetProcAddress( "glUniform1i" );
  glVertexAttrib2f = ( PFNGLVERTEXATTRIB2FPROC ) wglGetProcAddress( "glVertexAttrib2f" );
  glVertexAttrib3f = ( PFNGLVERTEXATTRIB3FPROC ) wglGetProcAddress( "glVertexAttrib3f" );
  glVertexAttrib4f = ( PFNGLVERTEXATTRIB4FPROC ) wglGetProcAddress( "glVertexAttrib4f" );
  glFramebufferTexture2D = ( PFNGLFRAMEBUFFERTEXTURE2DPROC ) wglGetProcAddress( "glFramebufferTexture2D" );
  glDrawBuffers = ( PFNGLDRAWBUFFERSPROC ) wglGetProcAddress( "glDrawBuffers" );
  glGenFramebuffers = ( PFNGLGENFRAMEBUFFERSPROC ) wglGetProcAddress( "glGenFramebuffers" );
  glBindFramebuffer = ( PFNGLBINDFRAMEBUFFERPROC ) wglGetProcAddress( "glBindFramebuffer" );
  glFramebufferTexture = ( PFNGLFRAMEBUFFERTEXTUREPROC ) wglGetProcAddress( "glFramebufferTexture" );
  glCreateShader = ( PFNGLCREATESHADERPROC ) wglGetProcAddress( "glCreateShader" );
  glShaderSource = ( PFNGLSHADERSOURCEPROC ) wglGetProcAddress( "glShaderSource" );
  glCompileShader = ( PFNGLCOMPILESHADERPROC ) wglGetProcAddress( "glCompileShader" );
  glCheckFramebufferStatus = ( PFNGLCHECKFRAMEBUFFERSTATUSPROC ) wglGetProcAddress( "glCheckFramebufferStatus" );
  glDeleteFramebuffers = ( PFNGLDELETEFRAMEBUFFERSPROC ) wglGetProcAddress( "glDeleteFramebuffers" );
  glVertexAttrib1f = ( PFNGLVERTEXATTRIB1FPROC ) wglGetProcAddress( "glVertexAttrib1f" );
  glBindFragDataLocation = ( PFNGLBINDFRAGDATALOCATIONPROC ) wglGetProcAddress( "glBindFragDataLocation" );
  glGenBuffers = ( PFNGLGENBUFFERSPROC ) wglGetProcAddress( "glGenBuffers" );
  glDeleteBuffers = ( PFNGLDELETEBUFFERSPROC ) wglGetProcAddress( "glDeleteBuffers" );
  glBindBuffer = ( PFNGLBINDBUFFERPROC ) wglGetProcAddress( "glBindBuffer" );
  glBufferData = ( PFNGLBUFFERDATAPROC ) wglGetProcAddress( "glBufferData" );
  glEnableVertexAttribArray = ( PFNGLENABLEVERTEXATTRIBARRAYPROC ) wglGetProcAddress( "glEnableVertexAttribArray" );
  glVertexAttribPointer = ( PFNGLVERTEXATTRIBPOINTERPROC ) wglGetProcAddress( "glVertexAttribPointer" );
  glDisableVertexAttribArray = ( PFNGLDISABLEVERTEXATTRIBARRAYPROC ) wglGetProcAddress( "glDisableVertexAttribArray" );
  glDeleteProgram = ( PFNGLDELETEPROGRAMPROC ) wglGetProcAddress( "glDeleteProgram" );
  glDeleteShader = ( PFNGLDELETESHADERPROC ) wglGetProcAddress( "glDeleteShader" );
  glDetachShader = ( PFNGLDETACHSHADERPROC ) wglGetProcAddress( "glDetachShader" );


  return true;
}//_InitGraphics




/*
=============
  _InitViewport
=============
*/
void Graphics::_InitViewport()
{
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho( 0.0, 100.0, 100.0, 0.0, -10.0, 10.0 ); // -10.0f - ближайшая к зрителю точка
  glMatrixMode( GL_MODELVIEW );
  GL_CHECK_ERROR;

  glShadeModel( GL_SMOOTH );
  glClearColor( 1.0f, 1.0f, 1.0f, 0.0f );
  glClearDepth( 1.0f );
  glDepthFunc( GL_LEQUAL );
  //glDepthFunc( GL_NICEST );
  glEnable( GL_DEPTH_TEST );

  glEnable( GL_TEXTURE_2D );

  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glEnable( GL_BLEND );

  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );  //отрисовка всех сторон. м.б. стоит сделать только FRONT, нужно тестировать

  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  glDisableClientState( GL_COLOR_ARRAY );
  glDisableClientState( GL_VERTEX_ARRAY);
  GL_CHECK_ERROR;
}//_InitViewport




/*
=============
  _InitPalette
=============
*/
void Graphics::_InitPalette()
{
  int pixelFormat = GetPixelFormat( this->_dc );
  PIXELFORMATDESCRIPTOR pfd;
  LOGPALETTE* pPal;
  int paletteSize;

  DescribePixelFormat( this->_dc, pixelFormat, sizeof( PIXELFORMATDESCRIPTOR ), &pfd );

  if ( pfd.dwFlags & PFD_NEED_PALETTE )
    paletteSize = 1 << pfd.cColorBits;
  else
  {
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitPalette => palette not needed" );
    return; //палитра не нужна
  }

  pPal = ( LOGPALETTE* ) malloc( sizeof( LOGPALETTE ) + paletteSize * sizeof( PALETTEENTRY ) );
  pPal->palVersion = 0x300;
  pPal->palNumEntries = paletteSize;

  /* build a simple RGB color palette */
  {
    int redMask   = ( 1 << pfd.cRedBits   ) - 1;
    int greenMask = ( 1 << pfd.cGreenBits ) - 1;
    int blueMask  = ( 1 << pfd.cBlueBits  ) - 1;
    int i;

    for( i = 0; i < paletteSize; ++i )
    {
      pPal->palPalEntry[ i ].peRed =
        ( ( ( i >> pfd.cRedShift ) & redMask ) * 255 ) / redMask;
      pPal->palPalEntry[ i ].peGreen =
        ( ( ( i >> pfd.cGreenShift ) & greenMask ) * 255 ) / greenMask;
      pPal->palPalEntry[ i ].peBlue =
        ( ( ( i >> pfd.cBlueShift ) & blueMask ) * 255 ) / blueMask;
      pPal->palPalEntry[ i ].peFlags = 0;
    }
  }

  this->_palette = CreatePalette( pPal );
  free( pPal );

  if( this->_palette )
  {
    SelectPalette( this->_dc, this->_palette, FALSE );
    RealizePalette( this->_dc );
    //__log.PrintInfo( Filelevel_DEBUG, "Core::InitPalette finished" );
  }
  else {
    this->SetState( CORE_STATE_EXIT );
  }
    //__log.PrintInfo( Filelevel_CRITICALERROR, "Core::InitPalette => CreatePalette failed" );
}//_InitPalette



/*
=============
  _InitPixelFormat
=============
*/
void Graphics::_InitPixelFormat()
{
  PIXELFORMATDESCRIPTOR pfd =
  {
    sizeof( PIXELFORMATDESCRIPTOR ),/* size */
    1,                              /* version */
    PFD_SUPPORT_OPENGL |
    PFD_DRAW_TO_WINDOW |
    PFD_DOUBLEBUFFER,               /* support double-buffering */
    PFD_TYPE_RGBA,                  /* color type */
    32,                             /* prefered color depth */
    0, 0, 0, 0, 0, 0,               /* color bits (ignored) */
    0,                              /* no alpha buffer */
    0,                              /* alpha bits (ignored) */
    0,                              /* no accumulation buffer */
    0, 0, 0, 0,                     /* accum bits (ignored) */
      32,                             /* depth buffer */
    0,                              /* no stencil buffer */
    0,                              /* no auxiliary buffers */
    PFD_MAIN_PLANE,                 /* main layer */
    0,                              /* reserved */
    0, 0, 0,                        /* no layer, visible, damage masks */
  };
  bool pixelFormatOk = false;
  std::vector<BYTE> depths( 4 );
  depths[ 0 ] = 24;
  depths[ 1 ] = 16;
  depths[ 2 ] = 8;
  depths[ 3 ] = 32;
  for( BYTE num = 0; num < depths.size(); ++num )
  {
    //__log.PrintInfo( Filelevel_DEBUG, "ChoosePixelFormat... %d", depths[ num ] );
    pfd.cDepthBits = depths[ num ];
    int pixelFormat = ChoosePixelFormat( this->_dc, &pfd );
    //__log.PrintInfo( Filelevel_DEBUG, "ChoosePixelFormat Done", pixelFormat );
    if ( pixelFormat )
      pixelFormatOk = true;
    else
      continue;

    //__log.PrintInfo( Filelevel_DEBUG, "SetPixelFormat..." );
    if( ::SetPixelFormat( this->_dc, pixelFormat, &pfd ) == TRUE )
    {
      pixelFormatOk = true;
      //__log.PrintInfo( Filelevel_INFO , "SetPixelFormat => cDepthBits[ %d ]", depths[ num ] );
      break;
    }
    else
      ;//__log.PrintInfo( Filelevel_DEBUG, "SetPixelFormat Done" );
  }
  if( !pixelFormatOk )
  {
    //__log.PrintInfo( Filelevel_CRITICALERROR, "InitPixelFormat failed" );
    exit( 1 );
    this->SetState( CORE_STATE_EXIT );
  }
}//_InitPixelFormat




/*
=============
  _Resize
=============
*/
void Graphics::_Resize( WORD screenWidth, WORD screenHeight )
{
  glViewport( 0, 0, screenWidth, screenHeight );
}//_Resize



/*
=============
  _UpdatePalette
=============
*/
bool Graphics::_UpdatePalette( const HWND window )
{
  if( window != this->_window )
  {
    this->SetState( CORE_STATE_EXIT );
    return false;
  }
  if ( !this->_glRC )
  {
    this->SetState( CORE_STATE_EXIT );
    return false;
  }
  if ( !this->_palette )
  {
    this->SetState( CORE_STATE_EXIT );
    return false;
  }

  ::UnrealizeObject ( this->_palette );
  ::SelectPalette   ( this->_dc, this->_palette, FALSE );
  ::RealizePalette  ( this->_dc );
  //this->Redraw    ();

  return true;
}//_UpdatePalette




/*
=============
  SetState
=============
*/
void Graphics::SetState( CoreStates newState )
{
  CoreStates oldState = this->_state;
  this->_state = newState;
  switch( oldState )  //в зависимости от перехода из состояние в состояние возможны какие-то действия
  {
  case CORE_STATE_UNDEFINED:
  break;
  }
}//SetState



/*
=============
  SetWindowTitle
=============
*/
void Graphics::SetWindowTitle( const std::string &title )
{
  if( this->_window ) {
    SetWindowText( this->_window, title.c_str() );
  }
}//SetWindowTitle


void Graphics::Update() {
  GL_CHECK_ERROR;
  sTimer.Update();
  this->keyboard.Update();
  this->mouse.Update();

  MSG msg;
  while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
  if( GetMessage( &msg, NULL, 0, 0 ) == TRUE )
  {
    TranslateMessage( &msg );
    DispatchMessage( &msg );
  }
}//Update


void Graphics::Redraw( RedrawProc *drawProc ) {
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); //GL_STENCIL_BUFFER_BIT
  GL_CHECK_ERROR;

  if( drawProc ) {
    drawProc();
  }

  SwapBuffers( this->_dc );
}//Redraw


bool Graphics::_CheckShaderError( const std::string& text, GLuint shader ) {
  bool result = false;
  if( !glIsShader( shader ) ) {
    LOGE( "[ERROR] %d is not a shader (%s)\n", shader, text.c_str() );
    return true;
  }
  Int maxLength, logLength = 0;
  glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );
  GL_CHECK_ERROR;
  char *log = new Char[ maxLength + 1 ];
  log[ maxLength ] = 0;
  glGetShaderInfoLog( shader, maxLength, &logLength, log );
  if( log[ 0 ] /* && this->CheckGLError( __LINE__, __FILE__ ) */ )
  {
    std::string tmpLog = log;
    BYTE errorType = Filelevel_INFO;
    if( tmpLog.find_first_of( "warning" ) >= 0 )
      errorType = Filelevel_WARNING;
    else
      errorType = Filelevel_CRITICALERROR;
    LOGE( "[ERROR] Graphics::_CheckShaderError => %s: %s\n", text.c_str(), log );
    //MessageBox(0,log,text.c_str(),0);
    result = true;
  }
  //__log.PrintInfo( Filelevel_DEBUG, "%s: %s", text.c_str(), log );
  delete [] log;
  return result;
}//_CheckShaderError


void Graphics::MakeScreenshot( const std::string& fileName ) {
  int size = this->_screenSize.width * this->_screenSize.height;
  memory data( size * 4 );
  glReadPixels( 0, 0, this->_screenSize.width, this->_screenSize.height, GL_RGBA, GL_UNSIGNED_BYTE, data.getData() );
  BYTE *bytes = ( BYTE* ) data.getData();
  for( int q = 0; q < size; ++q ) {
    *( ( DWORD* ) bytes ) = RGBA( bytes[ 2 ], bytes[ 1 ], bytes[ 0 ], bytes[ 3 ] );
    bytes += 4;
  }
  File f( fileName.c_str(), File_mode_WRITE );
  BITMAPFILEHEADER file;
  BITMAPINFOHEADER info;
  memset( &file, 0, sizeof( file ) );
  memset( &info, 0, sizeof( info ) );

  file.bfType = *( ( WORD* ) "BM" );
  file.bfSize = sizeof( file ) + sizeof( info ) + data.getLength();
  file.bfOffBits = sizeof( file ) + sizeof( info );

  info.biSize = sizeof( info );
  info.biWidth = this->_screenSize.width;
  info.biHeight = this->_screenSize.height;
  info.biPlanes = 1;
  info.biBitCount = 32;
  info.biCompression = BI_RGB;
  info.biSizeImage = data.getLength();
  info.biXPelsPerMeter = this->_screenSize.width;
  info.biYPelsPerMeter = this->_screenSize.height;

  f.Write( &file, sizeof( file ) );
  f.Write( &info, sizeof( info ) );
  f.Write( data.getData(), data.getLength() );
  f.Close();
}


LRESULT APIENTRY Graphics::WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
  Graphics *graph = ( Graphics* ) GetWindowLong( hWnd, GWL_USERDATA );
  switch (message)
  {
    case WM_CLOSE:
      if( graph ) {
        graph->SetState( CORE_STATE_EXIT );
        return 0;
      }
    break;

    case WM_DESTROY:
      PostQuitMessage( 0 );
    return 0;

    case WM_SIZE:
      //if( core )
      //  core->Signal( CORE_SIGNAL_RESIZE, lParam );
    return 0;

    case WM_PALETTECHANGED:
      //if( core && ( HWND ) wParam != hWnd )
      //  core->Signal( CORE_SIGNAL_UPDATEPALETTE, ( LPARAM ) hWnd );
    break;

    case WM_QUERYNEWPALETTE:
      //if( core && core->Signal( CORE_SIGNAL_UPDATEPALETTE, ( LPARAM ) hWnd ) )
      return TRUE;
    break;

    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_SYSCHAR: {
      bool altPressed = ( ( lParam&( 1<<29 ) ) == ( 1<<29) );
      if( wParam == VK_F4 && altPressed ) { //Alt+F4
        graph->SetState( CORE_STATE_EXIT );
      }
      return 0;
      break;
    }

    case WM_ACTIVATE:
      //core->Signal( 1, !HIWORD( wParam ) );
      return 0;
    break;

    case WM_SYSCOMMAND: // Перехватываем системную команду
    {
      switch( wParam )   // Останавливаем системный вызов
      {
        case SC_SCREENSAVE:   // Пытается ли запустится скринсейвер?
        case SC_MONITORPOWER: // Пытается ли монитор перейти в режим сбережения энергии?
        return 0;
      }
      break;
    }

    case WM_KEYDOWN:
    {
      if( !( lParam & 0x40000000 ) ) {  //prevent auto-repeating
        //__log.PrintInfo( Filelevel_DEBUG, "WM_KEYDOWN: %d[x%X] => %d mods[%d]", wParam, wParam, Keyboard::KeyCodeToGlut( wParam ), KeyboardGetModifiers() );
        graph->keyboard.DoPress( wParam );
      }
      //}
      return 0;
    }

    case WM_KEYUP:
    {
      graph->keyboard.DoRelease( wParam );
      return 0;
    }

    case WM_MOUSEMOVE:
    {
      graph->mouse.MoveCursor( Vec2( float( LOWORD( lParam ) ), float( HIWORD( lParam ) ) ) );
      return 0;
    }

    case WM_LBUTTONDOWN:
      graph->mouse.DoPress( VK_LBUTTON );
      return 0;
    break;

    case WM_LBUTTONUP:
      graph->mouse.DoRelease( VK_LBUTTON );
      return 0;
    break;

    case WM_RBUTTONDOWN:
      graph->mouse.DoPress( VK_RBUTTON );
      return 0;
    break;

    case WM_RBUTTONUP:
      graph->mouse.DoRelease( VK_RBUTTON );
      return 0;
    break;

    case WM_MBUTTONDOWN:
      graph->mouse.DoPress( VK_MBUTTON );
      return 0;
    break;

    case WM_MBUTTONUP:
      graph->mouse.DoRelease( VK_MBUTTON );
      return 0;
    break;

    case WM_SETCURSOR:
      return 0;
    break;

    case WM_MOUSEWHEEL:
      graph->mouse.SetWheel( short( HIWORD( wParam ) ) );
    break;

    default:
    break;
  }
  return DefWindowProc( hWnd, message, wParam, lParam );
}//WndProc

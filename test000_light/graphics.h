#pragma once


#include "types.h"
#include "keyboard.h"
#include "mouse.h"
#include "gl/gl.h"
#include "gl/glext.h"


enum CoreStates
{
  CORE_STATE_UNKNOWN,     //����������� ���������
  CORE_STATE_RUN,         //������������� ������� ���������
  CORE_STATE_EXIT,        //������������ ���������� ������
  CORE_STATE_INIT,        //���������� �������������


  CORE_STATE_UNDEFINED    //dummy
};


class Graphics
{
public:
  typedef void RedrawProc();
  Keyboard  keyboard;
  Mouse     mouse;

  Graphics( int width, int height );
  virtual ~Graphics();

  bool Init();
  void Destroy();
  static LRESULT APIENTRY WindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
  void SetState( CoreStates newState );
  __forceinline CoreStates GetState() { return this->_state; }
  void Update();
  void Redraw( RedrawProc *drawProc );
  void SetWindowTitle( const std::string &title );
  bool CheckGLError( int line = -1, const std::string& fileName = "" );
  void MakeScreenshot( const std::string& fileName );
  inline const Size& GetWindowSize() {
    return this->_screenSize;
  }
  bool _CheckShaderError( const std::string& text, GLuint shader );

private:
  bool _isInitialized;
  //Word _screenWidth,
  //     _screenHeight;
  Size _screenSize;
  HWND _window;
  HDC _dc;
  HGLRC _glRC;
  CoreStates _state;  //��������� ����
  HPALETTE _palette;

  Graphics();
  Graphics( const Graphics& );
  Graphics& operator=( const Graphics& );
  void _Resize( WORD screenWidth, WORD screenHeight );
  bool _InitGraphics();
  void _InitPixelFormat();
  void _InitPalette();
  void _InitViewport();
  bool _UpdatePalette( const HWND window );
};

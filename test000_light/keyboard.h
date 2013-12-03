#pragma once

#include "types.h"
#include "memory.h"
#include <vector>

int KeyboardGetModifiers();

class Keyboard
{
public:
  typedef void Listener( Dword keyId, bool isPressed );
  typedef std::vector< Listener* > ListenerList;

private:
  memory  keys,
          keysOld,
          keysPressed,
          keysReleased;
  ListenerList  listeners;

private:
  void  _ReallocBuffers( Dword newKeysCount );

public:
  Keyboard();
  virtual ~Keyboard();

  void  Update();

  void  DoPress     ( Dword keyId );
  void  DoRelease   ( Dword keyId );
  bool  IsHold      ( Dword keyId );
  bool  IsFree      ( Dword keyId );
  bool  IsPressed   ( Dword keyId );
  bool  IsReleased  ( Dword keyId );

  void  AddListener     ( Listener *listenerProc );
  void  RemoveListener  ( Listener *listenerProc );

  static int KeyCodeToGlut( WPARAM code );
  static int KeyModifiers;

protected:
  void ResetKeyStates();
};

/*
=============
  Keys
=============
*/

#define VK_LETTER_A   0x41
#define VK_LETTER_B   0x42
#define VK_LETTER_C   0x43
#define VK_LETTER_D   0x44
#define VK_LETTER_E   0x45
#define VK_LETTER_F   0x46
#define VK_LETTER_G   0x47
#define VK_LETTER_H   0x48
#define VK_LETTER_I   0x49
#define VK_LETTER_J   0x4A
#define VK_LETTER_K   0x4B
#define VK_LETTER_L   0x4C
#define VK_LETTER_M   0x4D
#define VK_LETTER_N   0x4E
#define VK_LETTER_O   0x4F
#define VK_LETTER_P   0x50
#define VK_LETTER_Q   0x51
#define VK_LETTER_R   0x52
#define VK_LETTER_S   0x53
#define VK_LETTER_T   0x54
#define VK_LETTER_U   0x55
#define VK_LETTER_V   0x56
#define VK_LETTER_W   0x57
#define VK_LETTER_X   0x58
#define VK_LETTER_Y   0x59
#define VK_LETTER_Z   0x5A

#define VK_NUMBER_0   0x30
#define VK_NUMBER_1   0x31
#define VK_NUMBER_2   0x32
#define VK_NUMBER_3   0x33
#define VK_NUMBER_4   0x34
#define VK_NUMBER_5   0x35
#define VK_NUMBER_6   0x36
#define VK_NUMBER_7   0x37
#define VK_NUMBER_8   0x38
#define VK_NUMBER_9   0x39

#include "keyboard.h"
#include "file.h"


int Keyboard::KeyModifiers = 0;


Keyboard::Keyboard()
{
  this->keys.alloc        ( 1024 );
  this->keysOld.alloc     ( 1024 );
  this->keysPressed.alloc ( 1024 );
  this->keysReleased.alloc( 1024 );
  memset( this->keys.getData(),         0, this->keys.getLength() );
  memset( this->keysOld.getData(),      0, this->keys.getLength() );
  memset( this->keysPressed.getData(),  0, this->keys.getLength() );
  memset( this->keysReleased.getData(), 0, this->keys.getLength() );
}//constructor



Keyboard::~Keyboard()
{
}//destructor



/*
=============
  _ReallocBuffers
=============
*/
void Keyboard::_ReallocBuffers( Dword newKeysCount )
{
  if( this->keys.getLength() >= newKeysCount )
    return;

  memory  tmpKeys( this->keys ),
          tmpKeysOld( this->keysOld ),
          tmpKeysPressed( this->keysPressed ),
          tmpKeysReleased( this->keysReleased );
  this->keys.realloc        ( newKeysCount );
  this->keysOld.realloc     ( newKeysCount );
  this->keysPressed.realloc ( newKeysCount );
  this->keysReleased.realloc( newKeysCount );
  memset( this->keys.getData(),         0, this->keys.getLength() );
  memset( this->keysOld.getData(),      0, this->keysOld.getLength() );
  memset( this->keysPressed.getData(),  0, this->keysPressed.getLength() );
  memset( this->keysReleased.getData(), 0, this->keysReleased.getLength() );
  memcpy( this->keys.getData(),         tmpKeys.getData(),        tmpKeys.getLength() );
  memcpy( this->keysOld.getData(),      tmpKeysOld.getData(),     tmpKeysOld.getLength() );
  memcpy( this->keysPressed.getData(),  tmpKeysPressed.getData(), tmpKeysPressed.getLength() );
  memcpy( this->keysReleased.getData(), tmpKeysReleased.getData(),tmpKeysReleased.getLength() );
}//_ReallocBuffers




/*
=============
  DoPress
=============
*/
void Keyboard::DoPress( Dword keyId )
{
  if( this->keys.getLength() <= keyId )
    this->_ReallocBuffers( keyId + 1 );

  this->keys.getData()[ keyId ] = true;

  if( !this->keysOld.getData()[ keyId ] )
    this->keysPressed.getData()[ keyId ] = true;
  __log.PrintInfo( Filelevel_DEBUG, "Keyboard::DoPress => x%X", keyId );

  if( this->listeners.size() )
  {
    ListenerList::iterator iter, iterEnd = this->listeners.end();
    for( iter = this->listeners.begin(); iter != iterEnd; ++iter )
      ( *iter )( keyId, true );
  }

  switch( keyId )
  {
    case VK_SHIFT: {
      Keyboard::KeyModifiers |= 1;//GLUT_ACTIVE_SHIFT;
      break;
    }
    case VK_CONTROL: {
      Keyboard::KeyModifiers |= 2;//GLUT_ACTIVE_CTRL;
      break;
    }
  }//switch keyId
}//DoPress



/*
=============
  DoRelease
=============
*/
void Keyboard::DoRelease( Dword keyId )
{
  if( this->keys.getLength() <= keyId )
    this->_ReallocBuffers( keyId + 1 );

  this->keys.getData()[ keyId ] = false;

  if( this->keysOld.getData()[ keyId ] )
    this->keysReleased.getData()[ keyId ] = true;

  if( this->listeners.size() )
  {
    ListenerList::iterator iter, iterEnd = this->listeners.end();
    for( iter = this->listeners.begin(); iter != iterEnd; ++iter )
      ( *iter )( keyId, false );
  }

  switch( keyId )
  {
    case VK_SHIFT: {
      Keyboard::KeyModifiers &= ~( ( int ) 1 /* GLUT_ACTIVE_SHIFT */ );
      break;
    }
    case VK_CONTROL: {
      Keyboard::KeyModifiers &= ~( ( int ) 2 /* GLUT_ACTIVE_CTRL */ );
      break;
    }
  }//switch keyId
}//DoPress



/*
=============
  IsHold
=============
*/
bool Keyboard::IsHold( Dword keyId )
{
  if( keyId >= this->keys.getLength() )
    return false;
  return this->keys.getData()[ keyId ] != 0;
}//IsHold



/*
=============
  IsFree
=============
*/
bool Keyboard::IsFree( Dword keyId )
{
  if( keyId >= this->keys.getLength() )
    return false;
  return this->keys.getData()[ keyId ] == 0;
}//IsFree



/*
=============
  IsPressed
=============
*/
bool Keyboard::IsPressed( Dword keyId )
{
  if( keyId >= this->keys.getLength() )
    return false;
  return this->keysPressed.getData()[ keyId ] != 0;
}//IsPressed



/*
=============
  IsReleased
=============
*/
bool Keyboard::IsReleased( Dword keyId )
{
  if( keyId >= this->keys.getLength() )
    return false;
  return this->keysReleased.getData()[ keyId ] != 0;
}//IsReleased



/*
=============
  Update
=============
*/
void Keyboard::Update()
{
  this->ResetKeyStates();
}//Update



/*
=============
  ResetKeyStates
=============
*/
void Keyboard::ResetKeyStates() {
  if( this->keys.getLength() )
  {
    memcpy( this->keysOld.getData(), this->keys.getData(), this->keys.getLength() );
    memset( this->keysPressed.getData(), 0, this->keysPressed.getLength() );
    memset( this->keysReleased.getData(), 0, this->keysReleased.getLength() );
  }
}//ResetKeyStates



/*
=============
  AddListener
=============
*/
void Keyboard::AddListener( Listener *listenerProc )
{
  if( listenerProc )
    this->listeners.push_back( listenerProc );
  else
    __log.PrintInfo( Filelevel_WARNING, "Keyboard::AddListener => function is NULL" );
}//AddListener



/*
=============
  RemoveListener
=============
*/
void Keyboard::RemoveListener( Listener *listenerProc )
{
  ListenerList::iterator iter, iterEnd = this->listeners.end();
  for( iter = this->listeners.begin(); iter != iterEnd; ++iter )
    if( *iter == listenerProc )
    {
      this->listeners.erase( iter );
      return;
    }
}//RemoveListener



/*
=============
  KeyCodeToGlut
=============
*/
int Keyboard::KeyCodeToGlut( WPARAM code )
{
  bool isShift  = ( KeyboardGetModifiers()&1 ) ? true : false;
  bool isCtrl   = ( KeyboardGetModifiers()&2 ) ? true : false;

  if( isCtrl ) {
    if( code >= VK_LETTER_A && code <= VK_LETTER_Z ) {
      code -= VK_LETTER_A - 1;
      return code;
    }
  }//ctrl

  switch( code )
  {
    case VK_BACK: return 8;
    case VK_DELETE: return 127;
    case VK_RETURN: return '\r';
    case VK_LEFT: return 100; //GLUT_KEY_LEFT;
    case VK_RIGHT: return 102; //GLUT_KEY_RIGHT;
    case VK_NUMBER_0: return ( isShift ? ')' : code );
    case VK_NUMBER_1: return ( isShift ? '!' : code );
    case VK_NUMBER_2: return ( isShift ? '@' : code );
    case VK_NUMBER_3: return ( isShift ? '#' : code );
    case VK_NUMBER_4: return ( isShift ? '$' : code );
    case VK_NUMBER_5: return ( isShift ? '%' : code );
    case VK_NUMBER_6: return ( isShift ? '^' : code );
    case VK_NUMBER_7: return ( isShift ? '&' : code );
    case VK_NUMBER_8: return ( isShift ? '*' : code );
    case VK_NUMBER_9: return ( isShift ? '(' : code );
    case VK_OEM_PERIOD: return ( isShift ? ',' : '.' );
    case VK_OEM_MINUS: return ( isShift ? '_' : '-' );
    case VK_OEM_PLUS: return ( isShift ? '+' : '=' );
  }//switch code

  if( code >= VK_LETTER_A && code <= VK_LETTER_Z && !isShift ) { //without shift
    code += 32;
  }

  return code;
}//KeyCodeToGlut

int KeyboardGetModifiers()
{
  return Keyboard::KeyModifiers;
}

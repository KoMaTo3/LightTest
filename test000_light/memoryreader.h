/*==============================
  Класс MemoryReader
  Класс работы с буффером как с файлом
==============================*/

#pragma once

#include "memory.h"
#include "klib.h"
#include "file.h"
#include "tools.h"
#include <string>

class MemoryReader
{
private:
  memory  data;
  Dword   curPos,
          curLength;
  char    *curData;  //либо указатель на data либо на внешний блок
  DISALLOW_COPY_AND_ASSIGN( MemoryReader );

public:
  MemoryReader();
  MemoryReader( memory &source );
  virtual ~MemoryReader();

  void    SetSource     ( void *srcData, Dword length );
  void    CopyFromSource( void *srcData, Dword length );
  int     Read          ( void *dest, Dword len );
  size_t  Read          ( void *dest, Dword len, Dword numBlocks );
  void    Free          ();
  void    SeekFromStart ( Dword pos );
  void    SeekFromCur   ( long dpos );
  inline
    int   GetCurPos     () { return this->curPos; }

  void operator>>( std::string &str );

  template< class T >
  void operator>>( T &data );

  bool CheckSizeToRead( Dword sizeToRead );

  template< class T >
  bool CheckDataSizeToRead( T dataToRead );
};


template< class T >
void MemoryReader::operator>>( T &data )
{
  if( !this->CheckDataSizeToRead( data ) )
  {
    __log.PrintInfo( Filelevel_WARNING, "MemoryReader::operator>> => out of bounds" );
    return;
  }
  memcpy( &data, this->curData + this->curPos, sizeof( data ) );
  this->curPos += sizeof( data );
}//operator>>

template< class T >
bool MemoryReader::CheckDataSizeToRead( T dataToRead )
{
  return this->CheckSizeToRead( sizeof( dataToRead ) );
}//CheckDataSizeToRead

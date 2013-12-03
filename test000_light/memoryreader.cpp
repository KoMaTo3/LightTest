#include "memoryreader.h"


MemoryReader::MemoryReader()
:curPos( 0 )
{
}//constructor


MemoryReader::MemoryReader( memory &source )
:curPos( 0 )
{
  this->curData = source.getData();
  this->curLength = source.getLength();
  this->curPos = 0;
}//constructor


MemoryReader::~MemoryReader()
{
}//destructor


/*
----------
  Read
  Чтение блока
----------
*/
int MemoryReader::Read( void *dest, Dword length )
{
  Dword realLen = min( length, this->curLength - this->curPos );

  memcpy( dest, this->curData + this->curPos, realLen );
  this->curPos += realLen;

  return realLen;
}//Read


/*
----------
  Read
  Чтение нескольких блоков
----------
*/
size_t MemoryReader::Read( void *dest, Dword length, Dword numBlocks )
{
  Dword realLen = min( length * numBlocks, this->curLength - this->curPos );
  size_t ret;
  if( realLen < length * numBlocks )
    ret = size_t( realLen / length );
  else
    ret = numBlocks;

  memcpy( dest, this->curData + this->curPos, realLen );
  this->curPos += realLen;

  return ret;
}//Read


/*
----------
  SetSource
  Установка источника данных
----------
*/
void MemoryReader::SetSource( void *srcData, Dword length )
{
  this->curData = (char*) srcData;
  this->curLength = length;
  this->curPos = 0;
}//SetSource


/*
----------
  CopyFromSource
----------
*/
void MemoryReader::CopyFromSource( void *srcData, Dword length )
{
  if( !srcData )
    return;

  this->data.alloc( length );
  this->curData = this->data.getData();
  memcpy( this->curData, srcData, length );
  this->curLength = length;
}//CopyFromSource


/*
----------
  Free
----------
*/
void MemoryReader::Free()
{
  this->data.free();
  this->curLength = this->curPos = 0;
  this->curData = 0;
}//Free


/*
----------
  SeekFromStart
----------
*/
void MemoryReader::SeekFromStart( Dword pos )
{
  if( pos > this->curLength )
    return;

  this->curPos = pos;
}//SeekFromStart


/*
----------
  SeekFromCur
----------
*/
void MemoryReader::SeekFromCur( long dpos )
{
  if( this->curPos + dpos > this->curLength ) {
    return;
  }

  this->curPos += dpos;
}//SeekFromCur




/*
=============
  CheckSizeToRead
=============
*/
bool MemoryReader::CheckSizeToRead( Dword sizeToWrite )
{
  return ( this->curPos + sizeToWrite <= this->curLength );
}//CheckSizeToRead



/*
=============
  operator>>
=============
*/
void MemoryReader::operator>>( std::string &str )
{
  Dword strLen;
  this->operator >>( strLen );
  if( !this->CheckSizeToRead( strLen ) )
  {
    __log.PrintInfo( Filelevel_WARNING, "MemoryReader::operator>> => out of bounds" );
    str = "";
    return;
  }

  memory tmp( strLen + 1 );
  memcpy( tmp.getData(), this->curData + this->curPos, strLen );
  tmp[ tmp.getLength() - 1 ] = 0;
  str = tmp.getData();

  this->curPos += strLen;
}//operator>>

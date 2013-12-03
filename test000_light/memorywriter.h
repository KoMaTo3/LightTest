#pragma once

#include "memory.h"
#include "klib.h"
#include "file.h"
#include <string>

class MemoryWriter
{
private:
  memory  &buffer;  //буффер, с которым связан врайтер
  Dword   pos;      //текущая позиция в буффере
  Dword blockSize;  //размер блока, выделяемого при нехватке места в блоке

public:
  MemoryWriter( memory& dest, Dword newBlockSize = 1024 );
  ~MemoryWriter();

  inline Dword  GetLength() { return this->pos; }

  MemoryWriter& operator<<( const std::string &str );

  template< class T >
  MemoryWriter& operator<<( T data );

  void CheckSizeToWrite( Dword sizeToWrite );

  template< class T >
  void CheckDataSizeToWrite( T dataToWrite );
};





template< class T >
MemoryWriter& MemoryWriter::operator<<( T data )
{
  //__log.PrintInfo( Filelevel_DEBUG, "MemoryWriter::operator<< => sizeof(data) = %d, pos[%d] length[%d]", sizeof( T ), this->pos, this->buffer.getLength() );
  this->CheckDataSizeToWrite( data );
  memcpy( this->buffer.getData() + this->pos, &data, sizeof( data ) );
  this->pos += sizeof( data );
  //__log.PrintInfo( Filelevel_DEBUG, "MemoryWriter::operator<< => done. pos[%d] length[%d]", this->pos, this->buffer.getLength() );
  return *this;
}//operator<<

template< class T >
void MemoryWriter::CheckDataSizeToWrite( T dataToWrite )
{
  this->CheckSizeToWrite( sizeof( dataToWrite ) );
}//CheckDataSizeToWrite

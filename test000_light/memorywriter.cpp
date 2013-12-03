#include "memorywriter.h"
#include "file.h"



MemoryWriter::MemoryWriter( memory& dest, Dword newBlockSize )
:buffer( dest ), pos( 0 ), blockSize( newBlockSize )
{
}//constructor



MemoryWriter::~MemoryWriter()
{
}//destructor



MemoryWriter& MemoryWriter::operator<<( const std::string &str )
{
  Dword strLen = str.size();
  //__log.PrintInfo( Filelevel_DEBUG, "MemoryWriter::operator<<(string) => sizeof(data:'%s') = %d, pos[%d] length[%d]", str.c_str(), sizeof( strLen ) + strLen, this->pos, this->buffer.getLength() );
  this->CheckSizeToWrite( sizeof( strLen ) + strLen );

  this->operator<<( strLen );
  memcpy( this->buffer.getData() + this->pos, &str[ 0 ], strLen );

  this->pos += strLen;

  //__log.PrintInfo( Filelevel_DEBUG, "MemoryWriter::operator<<(string) => done. pos[%d] length[%d]", this->pos, this->buffer.getLength() );
  return *this;
}




/*
=============
  CheckSizeToWrite
=============
*/
void MemoryWriter::CheckSizeToWrite( Dword sizeToWrite )
{
    if( this->pos + sizeToWrite > this->buffer.getLength() )
    {
      Dword toAdd = Dword( Math::Ceil( float( sizeToWrite - ( this->buffer.getLength() - this->pos ) ) / float( this->blockSize ) ) ) * this->blockSize;
      if( this->buffer.getLength() )
      {
        memory temp( this->buffer );
        this->buffer.realloc( this->buffer.getLength() + toAdd );
        memcpy( this->buffer.getData(), temp.getData(), temp.getLength() );
      }
      else
        this->buffer.alloc( toAdd );
    }
}//CheckSizeToWrite

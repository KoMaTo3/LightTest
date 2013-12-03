#include "vertexarray.h"


VertexArray::VertexArray() {
  //glGenVertexArrays( 1, &this->object );
  glGenBuffers( 1, &this->buffer );
}


VertexArray::VertexArray( void *data, int dataLength ) {
  //glGenVertexArrays( 1, &this->object );
  glGenBuffers( 1, &this->buffer );
  this->SetData( data, dataLength );
}


void VertexArray::SetData( void *data, int dataLength ) {
  glBindBuffer( GL_ARRAY_BUFFER, this->buffer );
  glBufferData( GL_ARRAY_BUFFER, dataLength, data, GL_STATIC_DRAW );
  glBindBuffer( GL_ARRAY_BUFFER, NULL );
}//SetData


VertexArray::~VertexArray() {
  //glDeleteVertexArrays( 1, &this->object );
  glDeleteBuffers( 1, &this->buffer );
}


void VertexArray::Bind() {
  //glBindVertexArray( this->object );
  glBindBuffer( GL_ARRAY_BUFFER, this->buffer );
}//Bind


void VertexArray::UnBind() {
  //glBindVertexArray( NULL );
  glBindBuffer( GL_ARRAY_BUFFER, NULL );
}//UnBind

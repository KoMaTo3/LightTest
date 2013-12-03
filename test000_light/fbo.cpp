//#include "gl/gl.h"


#include "fbo.h"
#include "logs.h"
#include "graphicsexternal.h"


Fbo::Fbo( int setWidth, int setHeight )
  :width( setWidth ), height( setHeight )
{
  glGenFramebuffers( 1, &this->frameBufferObject );
  GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
}



void Fbo::AddTexture( GLint components, GLenum format, GLenum type ) {
  int number = this->textures.size();
  this->textures.push_back( 0 );
  glGenTextures( 1, &this->textures[ number ] );
  glBindTexture( GL_TEXTURE_2D, this->textures[ number ] );
  glTexImage2D( GL_TEXTURE_2D, 0, components, this->width, this->height, 0, format, type, NULL );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glBindTexture( GL_TEXTURE_2D, 0 );
  glBindFramebuffer( GL_FRAMEBUFFER, this->frameBufferObject );
  glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + number, GL_TEXTURE_2D, this->textures[ number ], 0 );
  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
  this->drawBuffersParameters.push_back( GL_COLOR_ATTACHMENT0 + number );
}//AddTexture


Fbo::~Fbo() {
  glDeleteFramebuffers( 1, &this->frameBufferObject );
  glDeleteTextures( this->textures.size(), &this->textures[ 0 ] );
  this->textures.clear();
}


void Fbo::Bind() {
  glBindFramebuffer( GL_FRAMEBUFFER, this->frameBufferObject );
}//Bind


void Fbo::Unbind() {
  //glPopAttrib();
  glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}//Unbind


void Fbo::BindTexture( unsigned int levelNum ) {
  if( levelNum < this->textures.size() ) {
    glBindTexture( GL_TEXTURE_2D, this->textures[ levelNum ] );
  }
}//BindTexture


void Fbo::UnbindTexture() {
  glBindTexture( GL_TEXTURE_2D, 0 );
}//UnbindTexture


void Fbo::Draw() {
  glDrawBuffers( this->drawBuffersParameters.size(), &this->drawBuffersParameters[ 0 ] );
}//Draw

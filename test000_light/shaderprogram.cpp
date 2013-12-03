#include "shaderprogram.h"
#include "filemanager.h"
#include "logs.h"
#include "graphicsexternal.h"


ShaderProgram::ShaderProgram( const std::string& vertexShaderFileName, const std::string& fragmentShaderFileName ) {
  memory vertexData, fragmentData;
  __fileManager->GetFile( vertexShaderFileName, vertexData, true );
  __fileManager->GetFile( fragmentShaderFileName, fragmentData, true );
  this->vertex = glCreateShader( GL_VERTEX_SHADER );
  this->fragment = glCreateShader( GL_FRAGMENT_SHADER );
  GLint
    lengthShaderVertex = vertexData.getLength(),
    lengthShaderFragment = fragmentData.getLength();
  const GLchar* srcVertex = vertexData.getData();
  const GLchar* srcFragment = fragmentData.getData();
  glShaderSource( this->vertex, 1, &srcVertex, &lengthShaderVertex );
  glShaderSource( this->fragment, 1, &srcFragment, &lengthShaderFragment );
  glCompileShader( this->vertex );
  glCompileShader( this->fragment );
  this->program = glCreateProgram();
  glAttachShader( this->program, this->vertex );
  glAttachShader( this->program, this->fragment );
  glLinkProgram( this->program );
  this->_CheckShaderError( "ShaderProgram::vertexShader", this->vertex );
  this->_CheckShaderError( "ShaderProgram::fragmentShader", this->fragment );
}


bool ShaderProgram::_CheckShaderError( const std::string& text, GLuint shader ) {
  bool result = false;
  if( !glIsShader( shader ) ) {
    LOGD( "[ERROR] %d is not a shader (%s)\n", shader, text.c_str() );
    return true;
  }
  Int maxLength, logLength = 0;
  glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );
  char *log = new Char[ maxLength + 1 ];
  log[ maxLength ] = 0;
  glGetShaderInfoLog( shader, maxLength, &logLength, log );
  if( log[ 0 ] ) {
    std::string tmpLog = log;
    LOGD( "[ERROR] LightMgr::_CheckShaderError => %s: %s\n", text.c_str(), log );
    result = true;
  }
  delete [] log;
  return result;
}//_CheckShaderError


ShaderProgram::~ShaderProgram() {
  glDetachShader( this->program, this->vertex );
  glDetachShader( this->program, this->fragment );
  glDeleteShader( this->vertex );
  glDeleteShader( this->fragment );
  glDeleteProgram( this->program );
}


GLint ShaderProgram::GetUniformLocation( const std::string& uniformName ) {
  return glGetUniformLocation( this->program, uniformName.c_str() );
}//GetUniformLocation

void ShaderProgram::UseProgram() {
  glUseProgram( this->program );
}//UseProgram

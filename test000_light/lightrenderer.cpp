#include "lightrenderer.h"
#include "lightmgr.h"
#include "interngl.h"
#include "file.h"
#include "vertexarray.h"


LightRenderer::LightRenderer(
  const Vec2& setTextureSize,
  const std::string& vertexShaderLightFileName,
  const std::string& fragmentShaderLightFileName,
  const std::string& vertexShaderResultFileName,
  const std::string& fragmentShaderResultFileName,
  LightsListPtr setLightList
  )
:manager( new LightMgr() ), fbo( new Fbo( int( setTextureSize.x ), int( setTextureSize.y ) ) ) {
  this->manager->Init( setTextureSize, new ShaderProgram( vertexShaderLightFileName, fragmentShaderLightFileName ), setLightList );
  this->fbo->AddTexture( GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE );

  this->shader.shaderProgram = new ShaderProgram( vertexShaderResultFileName, fragmentShaderResultFileName );

  this->shader.uniformTextureScene = this->shader.shaderProgram->GetUniformLocation( "texture0" );
  this->shader.uniformTextureLight = this->shader.shaderProgram->GetUniformLocation( "texture1" );
  this->shader.uniformTextureBrightness = this->shader.shaderProgram->GetUniformLocation( "texture2" );
  this->shader.uniformOneByWindowSize = this->shader.shaderProgram->GetUniformLocation( "oneByWindowCoeff" );

  glGenBuffers( 1, &this->sceneVertexBuffer );
  const Vertice vertices[] = {
    { Vec2( -1.0f,  1.0f ), Vec2( 0.0f, 0.0f ) },
    { Vec2(  1.0f,  1.0f ), Vec2( 1.0f, 0.0f ) },
    { Vec2( -1.0f, -1.0f ), Vec2( 0.0f, 1.0f ) },
    { Vec2(  1.0f, -1.0f ), Vec2( 1.0f, 1.0f ) }
  };
  glBindBuffer( GL_ARRAY_BUFFER, this->sceneVertexBuffer );
  glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );

  this->vao = new VertexArray();
}


LightRenderer::~LightRenderer() {
  delete this->fbo;
  delete this->manager;
  delete this->shader.shaderProgram;
  delete this->vao;
  glDeleteBuffers( 1, &this->sceneVertexBuffer );
}


void LightRenderer::BeginScene() {
  this->manager->Render();
  this->fbo->Bind();
  glPushAttrib( GL_VIEWPORT_BIT );
}//BeginScene


void LightRenderer::EndScene() {
  glPopAttrib();
  this->fbo->Unbind();
  glDrawBuffer( GL_FRONT );
  //glEnable( GL_TEXTURE_2D );
  glDisable( GL_BLEND );
  glDisable(GL_DEPTH_TEST);

  this->shader.shaderProgram->UseProgram();
  //this->vao->Bind();

  glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glUniform2f( this->shader.uniformOneByWindowSize, this->manager->GetOneByWindowSize().x, this->manager->GetOneByWindowSize().y );

  glActiveTexture( GL_TEXTURE0 + 1 );
  this->manager->GetFBO()->BindTexture( 0 );
  glUniform1i( this->shader.uniformTextureLight, 1 );

  glActiveTexture( GL_TEXTURE0 + 0 );
  this->fbo->BindTexture( 0 );
  glUniform1i( this->shader.uniformTextureScene, 0 );

  //glActiveTexture( GL_TEXTURE0 + 2 );
  //this->manager->GetFBO()->BindTexture( 1 );
  //glUniform1i( this->shader.uniformTextureBrightness, 2 );
  //glActiveTexture( GL_TEXTURE0 + 0 );

  glBindBuffer( GL_ARRAY_BUFFER, this->sceneVertexBuffer );
  glEnableVertexAttribArray( 0 );
  glEnableVertexAttribArray( 1 );

  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertice ), 0 );
  glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof( Vertice ), ( void* ) ( sizeof( Vec2 ) ) );
  glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

  glDisableVertexAttribArray( 0 );
  glDisableVertexAttribArray( 1 );
  glBindBuffer( GL_ARRAY_BUFFER, 0 );

  //this->vao->UnBind();
  glUseProgram( 0 );
  //glActiveTexture( GL_TEXTURE0 + 2 );
  //glBindTexture( GL_TEXTURE_2D, 0 );
  //glActiveTexture( GL_TEXTURE0 + 1 );
  //glBindTexture( GL_TEXTURE_2D, 0 );
  //glActiveTexture( GL_TEXTURE0 + 0 );
  //glBindTexture( GL_TEXTURE_2D, 0 );
}//EndScene


void LightRenderer::Update() {
  this->manager->Update();
}//Update


void LightRenderer::SetRect( const Vec2& leftTop, const Vec2& rightBottom ) {
  this->manager->SetRect( leftTop, rightBottom );
}//SetRect

#include "test.h"
#include "tools.h"
#include "lightrenderer.h"
#include "filemanager.h"
#include "timer.h"
#include "graphics.h"
#include "imageloader.h"
#include "logs.h"
#include "graphicsexternal.h"


extern File __log;


//AABB
class LightBlock: public LightMap::ILightBlock {
public:
  Vec2 position;
  Vec2
    size,
    lastSize;
  Vec2 halfSize;
  const float epsilon;

  LightBlock( const Vec2& setPosition, const Vec2& setSize )
  :position( setPosition ), size( setSize ), halfSize( setSize * 0.5f ), epsilon( 0.0f ), lastSize( -setSize )
  {
  }

  virtual inline const Vec2& GetPosition() const {
    return this->position;
  }
  virtual inline const Vec2& GetSize() const {
    return this->size;
  }
  virtual inline const Vec2& GetHalfSize() const {
    return this->halfSize;
  }

  virtual void FillBuffer( const Vec2& lightPosition, const Vec2& size, LBuffer *buffer, LBufferCacheEntity *cache ) {
    //default
    /*
    if( lightPosition.x < this->position.x + this->halfSize.x ) { //add right edge
      this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->position.x + this->halfSize.x, this->position.y - this->halfSize.y - this->epsilon ), Vec2( this->position.x + this->halfSize.x, this->position.y + this->halfSize.y + this->epsilon ), cache );
    }
    if( lightPosition.x > this->position.x - this->halfSize.x ) { //add left edge
      this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->position.x - this->halfSize.x, this->position.y - this->halfSize.y - this->epsilon ), Vec2( this->position.x - this->halfSize.x, this->position.y + this->halfSize.y + this->epsilon ), cache );
    }
    if( lightPosition.y < this->position.y + this->halfSize.y ) { //add bottom edge
      this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->position.x - this->halfSize.x - this->epsilon, this->position.y + this->halfSize.y ), Vec2( this->position.x + this->halfSize.x + this->epsilon, this->position.y + this->halfSize.y ), cache );
    }
    if( lightPosition.y > this->position.y - this->halfSize.y ) { //add top edge
      this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->position.x - this->halfSize.x - this->epsilon, this->position.y - this->halfSize.y ), Vec2( this->position.x + this->halfSize.x + this->epsilon, this->position.y - this->halfSize.y ), cache );
    }
    */

    //darkness
    //__log.PrintInfo( Filelevel_DEBUG, "FillBuffer => pos[%3.3f; %3.3f] size[%3.3f; %3.3f] halfSize[%3.3f; %3.3f] light[%3.3f; %3.3f]", this->position.x - lightPosition.x, this->position.y - lightPosition.y, this->size.x, this->size.y, this->halfSize.x, this->halfSize.y, lightPosition.x, lightPosition.y );
    if( lightPosition.x < this->position.x - this->halfSize.x ) { //add right edge
      this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->position.x - this->halfSize.x, this->position.y - this->halfSize.y - this->epsilon ), Vec2( this->position.x - this->halfSize.x, this->position.y + this->halfSize.y + this->epsilon ), cache );
    }
    if( lightPosition.x > this->position.x + this->halfSize.x ) { //add left edge
      this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->position.x + this->halfSize.x, this->position.y - this->halfSize.y - this->epsilon ), Vec2( this->position.x + this->halfSize.x, this->position.y + this->halfSize.y + this->epsilon ), cache );
    }
    if( lightPosition.y < this->position.y - this->halfSize.y ) { //add bottom edge
      this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->position.x - this->halfSize.x - this->epsilon, this->position.y - this->halfSize.y ), Vec2( this->position.x + this->halfSize.x + this->epsilon, this->position.y - this->halfSize.y ), cache );
    }
    if( lightPosition.y > this->position.y + this->halfSize.y ) { //add top edge
      this->AddEdgeToBuffer( lightPosition, buffer, Vec2( this->position.x - this->halfSize.x - this->epsilon, this->position.y + this->halfSize.y ), Vec2( this->position.x + this->halfSize.x + this->epsilon, this->position.y + this->halfSize.y ), cache );
    }
    //*/
  }

  virtual void SetPosition( const Vec2& setPosition ) {
    this->position = setPosition;
  }

  void SetSize( const Vec2& setSize ) {
    this->size = setSize;
  }

  virtual void Update() {
    if( this->lastSize != this->size ) {
      this->halfSize = this->size * 0.5f;
      this->lastSize = this->size;
    }
  }

  virtual void Redraw() {
    glBegin( GL_QUADS );
    Vec2 v2 = this->GetPosition();
    Vec2 sz = this->GetHalfSize();

    glTexCoord2f( 0.0f, 0.0f );
    glVertex3f( v2.x - sz.x, v2.y - sz.y, 0.0f );

    glTexCoord2f( 1.0f, 0.0f );
    glVertex3f( v2.x + sz.x, v2.y - sz.y, 0.0f );

    glTexCoord2f( 1.0f, 1.0f );
    glVertex3f( v2.x + sz.x, v2.y + sz.y, 0.0f );

    glTexCoord2f( 0.0f, 1.0f );
    glVertex3f( v2.x - sz.x, v2.y + sz.y, 0.0f );
    glEnd();
  }
};



class LightBlockPoly: public LightMap::ILightBlock {
public:
  Vec2
    position,
    lastPosition;
  Vec2 size;
  Vec2 halfSize;
  Mat4 matrix;
  const float epsilon;
  typedef std::vector< Vec2 > VerticesList;
  VerticesList
    verticesSrc,
    vertices;
  float
    alpha,
    lastAlpha;

  LightBlockPoly( const Vec2& setPosition )
    :position( setPosition ), epsilon( 1.0f ), size( Vec2Null ), halfSize( Vec2Null ), lastPosition( -setPosition ), alpha( 1.0f ), lastAlpha( 0.0f )
  {
  }

  virtual inline const Vec2& GetPosition() const {
    return this->position;
  }
  virtual inline const Vec2& GetSize() const {
    return this->size;
  }
  virtual inline const Vec2& GetHalfSize() const {
    return this->halfSize;
  }

  virtual void FillBuffer( const Vec2& lightPosition, const Vec2& size, LBuffer *buffer, LBufferCacheEntity *cache ) {
    if( this->vertices.size() < 2 ) {
      return;
    }
    VerticesList::const_iterator
      iter = this->vertices.begin() + 1,
      iterEnd = this->vertices.end(),
      iterPrev = this->vertices.begin();
    for(; iter != iterEnd; ++iter ) {
      Vec2 vec( *iter - *iterPrev );
      vec.Rotate90CCW();
      if( vec.Dot( lightPosition - *iter ) < 0.0f ) {
        Vec2
          vec0( *iter - *iterPrev ),
          vec1( *iterPrev - *iter )
          ;
        vec0.NormalizeFast();
        vec1.NormalizeFast();
        vec.NormalizeFast();
        //vec *= ( 0.5f + this->epsilon );  //углубляем ребро на 1+epsilon
        vec0 *= ( 1.0f + this->epsilon ); //удлинняем ребро на 1+epsilon
        vec1 *= ( 1.0f + this->epsilon );
        this->AddEdgeToBuffer( lightPosition, buffer, *iter + vec0 + vec, *iterPrev + vec1 + vec, cache );
      }
      iterPrev = iter;
    }
  }

  virtual void SetPosition( const Vec2& setPosition ) {
    this->position = setPosition;
  }

  virtual void Update() {
    if( this->lastPosition != this->position || this->lastAlpha != this->alpha ) {
      int num = 0;
      this->matrix.Identity();
      Mat4 matrixTranslate, matrixRotate;
      matrixTranslate.Identity();
      matrixRotate.Identity();
      matrixTranslate[ 0 ][ 3 ] = this->position.x;
      matrixTranslate[ 1 ][ 3 ] = this->position.y;
      float cosf = Math::Cos16( this->alpha );
      float sinf = Math::Sin16( this->alpha );
      matrixRotate[ 0 ][ 0 ] = cosf;
      matrixRotate[ 1 ][ 1 ] = cosf;
      matrixRotate[ 0 ][ 1 ] = sinf;
      matrixRotate[ 1 ][ 0 ] = -sinf;
      this->matrix = matrixTranslate * matrixRotate;
      this->size = Vec2Null;
      for( auto &vertex: this->verticesSrc ) {
        Vec4 vec( vertex.x, vertex.y, 0.0f, 1.0f );
        vec = vec * this->matrix;
        this->vertices[ num++ ].Set( vec.x, vec.y );
        if( Math::Fabs( vec.x ) > this->size.x ) {
          this->size.x = Math::Fabs( vec.x );
        }
        if( Math::Fabs( vec.y ) > this->size.y ) {
          this->size.y = Math::Fabs( vec.y );
        }
      }
      this->halfSize = this->size;
      this->size *= 2.0f;
      this->lastPosition = this->position;
      this->lastAlpha = this->alpha;
    }
  }

  void AddVertice( const Vec2& pos ) {
    this->verticesSrc.push_back( pos );
    this->vertices.push_back( pos );
    float length = pos.LengthFast();
    if( length > this->halfSize.x ) {
      this->size.Set( length, length );
      this->size *= 2.0f;
      this->halfSize.Set( length, length );
    }
  }

  void SetAlpha( float setAlpha ) {
    this->alpha = setAlpha;
  }

  virtual void Redraw() {
    glBegin( GL_TRIANGLE_FAN );

    glTexCoord2f( 0.0f, 0.0f );
    glVertex3f( this->position.x, this->position.y, 0.0f );

    for( auto &vertex: this->vertices ) {
      glTexCoord2f( ( vertex.x - this->position.x ) / this->size.x + 0.5f, ( vertex.y - this->position.y ) / this->size.y + 0.5f );
      glVertex3f( vertex.x, vertex.y, 0.0f );
    }

    glEnd();
  }
};


LightTest::LightTest( int argc, char* argv[] )
:core( NULL ), windowSize( 1024, 768 ),
lightList( NULL ), lightRenderer( NULL )
{
  Math::Init();
  char tmp[ 1024 ];
  if( argc > 2 ) {
      int width, height;
      sscanf_s( argv[ 1 ], "%d", &width );
      sscanf_s( argv[ 2 ], "%d", &height );
      if( width > 0 && height > 0 ) {
        this->windowSize.width = width;
        this->windowSize.height = height;
      }
      if( argc > 3 ) {
        this->action = argv[ 3 ];
      }
  }
  GetModuleFileName( GetModuleHandle( NULL ), tmp, 1024 );
  std::string path = tools::GetPathFromFilePath( tmp );
  tools::SetCurDirectory( path.c_str() );
  __log.Open( "logs.txt", File_mode_WRITE, Filelevel_ALL );
  __log.EnableAutoFlush( true );
  __fileManager = new FileManager();
  __fileManager->ScanAll();
  sTimer.UpdateFreq();
  sTimer.UpdateCounter();
  this->core = new Graphics( this->windowSize.width, this->windowSize.height );

  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  this->texturesList.resize( 10 );
  glGenTextures( this->texturesList.size(), &this->texturesList[ 0 ] );
  //glTexImage2D( GL_TEXTURE_2D, 0, 4, this->size.width, this->size.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->textureData.getData() );
  glEnable( GL_TEXTURE_2D );
  glBindTexture ( GL_TEXTURE_2D, this->texturesList[ LIGHTTEST_TEXTURE_BACKGROUND0 ] );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  ImageLoader image;
  if( !image.LoadFromFile( "data/8B9_green-rock2-s-2.jpg" ) ) {
    LOGE( "[ERROR] can't load texture\n" );
  } else {
    LOGD( "texture[%dx%d] id[%d]\n", image.GetImageSize()->width, image.GetImageSize()->height, this->texturesList[ LIGHTTEST_TEXTURE_BACKGROUND0 ] );
  }
  glTexImage2D( GL_TEXTURE_2D, 0, 4, image.GetImageSize()->width, image.GetImageSize()->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetImageData() );
  glBindTexture ( GL_TEXTURE_2D, this->texturesList[ LIGHTTEST_TEXTURE_BLOCK ] );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  if( !image.LoadFromFile( "data/question0.jpg" ) ) {
    LOGE( "[ERROR] can't load texture\n" );
  } else {
    LOGD( "texture[%dx%d] id[%d]\n", image.GetImageSize()->width, image.GetImageSize()->height, this->texturesList[ LIGHTTEST_TEXTURE_BLOCK ] );
  }
  glTexImage2D( GL_TEXTURE_2D, 0, 4, image.GetImageSize()->width, image.GetImageSize()->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetImageData() );
  glBindTexture ( GL_TEXTURE_2D, 0 );

  this->core->mouse.Init( windowSize );

  glShadeModel( GL_SMOOTH );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glEnable( GL_BLEND );
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

  //shaders
  memory
    dataVS,
    dataFS,
    dataRendererVS,
    dataRendererFS
    ;
  __fileManager->GetFile( "data/shader0.vs", dataVS, true );
  __fileManager->GetFile( "data/shader0.fs", dataFS, true );
  __fileManager->GetFile( "data/shader2.vs", dataRendererVS, true );
  __fileManager->GetFile( "data/shader2.fs", dataRendererFS, true );

  ShowCursor( FALSE );
  //this->camera.position.Set( 1780.0f, 50.0f );
  this->camera.position.Set( 0.0f, 0.0f );
  this->camera.movementSpeed = 100.0f;
  this->camera.scale = this->camera.scaleTarget = 0.7f;

  this->lightList = new LightList;
  this->lightRenderer = new LightRenderer(
      Vec2( float( this->windowSize.width ), float( this->windowSize.height ) ),
      "data/shader0.vs",
      "data/shader0.fs",
      "data/shader2.vs",
      "data/shader2.fs",
      this->lightList
      );
  this->lightRenderer->SetRect( this->camera.leftTop, this->camera.rightBottom );
  this->lightRenderer->GetLightManager()->lightAmbient.Set( 1.0f, 1.0f, 1.0f, 0.4f );
  this->lightRenderer->GetLightManager()->lightList->push_back( new LightMap::LightEntity( LightMap::LT_POINT, Vec2( -22.0f, 35.0f ), Vec4( 0.0f, 1.0f, 1.0f, 1.0f ), Vec2( 500.0f, 500.0f ), 0.7f, 1024 ) );
  //this->lightRenderer->GetLightManager()->lightList->push_back( new LightMap::LightEntity( LightMap::LT_POINT, Vec2( 120.0f, -120.0f ), Vec4( 1.0f, 1.0f, 0.0f, 1.0f ), Vec2( 300.0f, 300.0f ), 0.7f, 1024 ) ); //debug 2nd light
  this->lightRenderer->GetLightManager()->lightBlocks.push_back( new LightBlock( Vec2( 0.0f, 0.0f ), Vec2( 30.0f, 30.0f ) ) );
  this->lightRenderer->GetLightManager()->lightBlocks.push_back( new LightBlock( Vec2( 100.0f, 70.0f ), Vec2( 30.0f, 60.0f ) ) );
  //this->lightRenderer->GetLightManager()->lightBlocks.push_back( new LightBlock( Vec2( -30.0f, 0.0f ), Vec2( 30.0f, 30.0f ) ) );

  //this->lightRenderer->GetLightManager()->lightList->push_back( new LightMap::LightEntity( LightMap::LT_POINT, Vec2( 1684.286f, -17.0f ), Vec4( 1.0f, 1.0f, 1.0f, 1.0f ), Vec2( 400.0f, 400.0f ), 1.0f, 1024 ) );
  //this->lightRenderer->GetLightManager()->lightBlocks.push_back( new LightBlock( Vec2( 1864.0f, -32.0f ), Vec2( 32.0f, 32.0f ) ) );
  //this->lightRenderer->GetLightManager()->lightBlocks.push_back( new LightBlock( Vec2( 1864.0f, 0.0f ), Vec2( 32.0f, 32.0f ) ) );
  /*
  this->lightRenderer->GetLightManager()->lightBlocks.push_back( new LightBlock( Vec2( 1864.0f, 32.0f ), Vec2( 32.0f, 32.0f ) ) );
  this->lightRenderer->GetLightManager()->lightBlocks.push_back( new LightBlock( Vec2( 1864.0f, 64.0f ), Vec2( 32.0f, 32.0f ) ) );
  this->lightRenderer->GetLightManager()->lightBlocks.push_back( new LightBlock( Vec2( 1864.0f, 96.0f ), Vec2( 32.0f, 32.0f ) ) );
  this->lightRenderer->GetLightManager()->lightBlocks.push_back( new LightBlock( Vec2( 1864.0f, 128.0f ), Vec2( 32.0f, 32.0f ) ) );
  this->lightRenderer->GetLightManager()->lightBlocks.push_back( new LightBlock( Vec2( 1024.0f, 256.0f ), Vec2( 256.0f, 256.0f ) ) );
  this->lightRenderer->GetLightManager()->lightBlocks.push_back( new LightBlock( Vec2( 1280.0f, 256.0f ), Vec2( 256.0f, 256.0f ) ) );
  this->lightRenderer->GetLightManager()->lightBlocks.push_back( new LightBlock( Vec2( 1536.0f, 256.0f ), Vec2( 256.0f, 256.0f ) ) );
  this->lightRenderer->GetLightManager()->lightBlocks.push_back( new LightBlock( Vec2( 1792.0f, 256.0f ), Vec2( 256.0f, 256.0f ) ) );
  */

  LightBlockPoly *poly = new LightBlockPoly( Vec2( -50.0f, -50.0f ) );
  poly->AddVertice( Vec2( 20.0f, 20.0f ) );
  poly->AddVertice( Vec2( 40.0f, -50.0f ) );
  poly->AddVertice( Vec2( -20.0f, -20.0f ) );
  poly->AddVertice( Vec2( -20.0f, 20.0f ) );
  poly->AddVertice( Vec2( 20.0f, 20.0f ) );
  this->lightRenderer->GetLightManager()->lightBlocks.push_back( poly );

  Vec2 center( 0.0f, 0.0f );
  float r = 0.15f * float( this->GetWindowSize().width );
  for( float a = 0.0f; a < Math::TWO_PI; a += 0.5f ) {
    this->lightRenderer->GetLightManager()->lightBlocks.push_back( new LightBlock( center + Vec2( Math::Sin16( a ) * r, Math::Cos16( a ) * r ), Vec2( 30.0f, 30.0f ) ) );
  }
  r = 0.3f * float( this->GetWindowSize().width );
  for( float a = 0.0f; a < Math::TWO_PI; a += 0.2f ) {
    this->lightRenderer->GetLightManager()->lightBlocks.push_back( new LightBlock( center + Vec2( Math::Sin16( a + 0.1f ) * r, Math::Cos16( a + 0.1f ) * r ), Vec2( 30.0f, 30.0f ) ) );
  }
  r = 0.45f * float( this->GetWindowSize().width );
  for( float a = 0.0f; a < Math::TWO_PI; a += 0.2f ) {
    this->lightRenderer->GetLightManager()->lightBlocks.push_back( new LightBlock( center + Vec2( Math::Sin16( a + 0.3f ) * r, Math::Cos16( a + 0.3f ) * r ), Vec2( 30.0f, 30.0f ) ) );
  }
}//constructor

void LightTest::Run() {
  Dword FPS = 0, FPSCurrent = 0;
  float FPSTimer = 0.0f;
  static char text[ 1024 ];

  while( this->core->GetState() != CORE_STATE_EXIT ) {
    this->Update();
    FPSTimer += sTimer.GetDeltaF();
    if( this->core->keyboard.IsPressed( VK_ESCAPE ) ) {
      this->core->SetState( CORE_STATE_EXIT );
    }

    this->core->Redraw( LightTest::Redraw );
    if( this->action == "shot" && FPSCurrent == 1 ) {
      core->MakeScreenshot( "screenshot.bmp" );
      break;
    }
    ++FPSCurrent;
    if( FPSTimer > 1.0f ) {
      FPSTimer -= 1.0f;
      FPS = FPSCurrent;
      FPSCurrent = 0;
    }
    sprintf_s( text, sizeof( text ), "FPS[%d] objects[%d] lights[%d]", FPS, this->lightRenderer->GetLightManager()->lightBlocks.size(), this->lightRenderer->GetLightManager()->lightList->size() );
    //Vec2 &lightPos = ( *this->lightRenderer->GetLightManager()->lightList->begin() )->position; sprintf_s( text, sizeof( text ), "%3.3f; %3.3f", lightPos.x, lightPos.y );
    this->core->SetWindowTitle( text );
  }
}//Run


void LightTest::Update() {
  static float t = 0.0f;

  this->core->Update();

  t += sTimer.GetDeltaF();
  ( ( LightBlockPoly* ) this->lightRenderer->GetLightManager()->lightBlocks[ 2 ] )->SetAlpha( t );
  ( ( LightBlock* ) this->lightRenderer->GetLightManager()->lightBlocks[ 1 ] )->SetSize( Vec2( 30.0f + Math::Cos16( t * 3.0f ) * 10.0f, 30.0f + Math::Sin16( t * 4.0f ) * 10.0f  ) );

  //insert new light random
  if( this->core->mouse.IsPressed( VK_LBUTTON ) ) {
    this->lightRenderer->GetLightManager()->lightList->push_back( ( *this->lightRenderer->GetLightManager()->lightList->begin() )->Clone() );
    ( *this->lightRenderer->GetLightManager()->lightList->rbegin() )->color.Set( ( rand() % 1000 ) / 2000.0f + 0.5f, ( rand() % 1000 ) / 2000.0f + 0.5f, ( rand() % 1000 ) / 2000.0f + 0.5f, 1.0f );
  }

  //insert R-G-B lights
  if( this->core->keyboard.IsPressed( VK_NUMBER_1 ) || this->core->keyboard.IsPressed( VK_NUMBER_2 ) || this->core->keyboard.IsPressed( VK_NUMBER_3 ) || this->core->keyboard.IsPressed( VK_NUMBER_4 ) ) {
    this->lightRenderer->GetLightManager()->lightList->push_back( ( *this->lightRenderer->GetLightManager()->lightList->begin() )->Clone() );
    ( *this->lightRenderer->GetLightManager()->lightList->rbegin() )->color.Set(
      this->core->keyboard.IsPressed( VK_NUMBER_1 ) || this->core->keyboard.IsPressed( VK_NUMBER_4 ) ? 1.0f : 0.0f,
      this->core->keyboard.IsPressed( VK_NUMBER_2 ) ? 1.0f : 0.0f,
      this->core->keyboard.IsPressed( VK_NUMBER_3 ) || this->core->keyboard.IsPressed( VK_NUMBER_4 ) ? 1.0f : 0.0f,
      1.0f );
  }

  //camera zoom
  if( this->core->mouse.GetWheel() ) {
    this->camera.scaleTarget = min( max( this->camera.scaleTarget + float( this->core->mouse.GetWheel() ) / 1000.0f, 0.1f ), 10.0f );
  }

  //clear scene from light
  if( this->core->keyboard.IsPressed( VK_SPACE ) ) {
    while( this->lightRenderer->GetLightManager()->lightList->size() > 1 ) {
      this->lightRenderer->GetLightManager()->lightList->pop_back();
    }
  }

  //camera movement
  bool isRight = this->core->keyboard.IsHold( VK_RIGHT );
  bool isLeft = this->core->keyboard.IsHold( VK_LEFT );
  bool isUp = this->core->keyboard.IsHold( VK_UP );
  bool isDown = this->core->keyboard.IsHold( VK_DOWN );
  if( isRight || isLeft || isUp || isDown ) {
    this->camera.position.x += sTimer.GetDeltaF() * ( isLeft ? -100.0f : ( isRight ? 100.0f : 0.0f ) );
    this->camera.position.y += sTimer.GetDeltaF() * ( isUp ? -100.0f : ( isDown ? 100.0f : 0.0f ) );
  }

  //recalculate view rect
  this->camera.scale += ( this->camera.scaleTarget - this->camera.scale ) * sTimer.GetDeltaF() * 5.0f;
  if( this->core->keyboard.IsPressed( VK_BACK ) ) {
    this->camera.scale = 1.0f;
    this->camera.position.Set( float( this->windowSize.width >> 1 ), float( this->windowSize.height >> 1 ) );
  }
  const Vec2 halfScreen( float( this->windowSize.width >> 1 ) / this->camera.scale, float( this->windowSize.height >> 1 ) / this->camera.scale );
  this->camera.leftTop = ( this->camera.position - halfScreen );
  this->camera.rightBottom = ( this->camera.position + halfScreen );
  this->lightRenderer->SetRect( this->camera.leftTop, this->camera.rightBottom );

  //move light under mouse cursor
  if( this->action != "shot" ) {
    ( *this->lightRenderer->GetLightManager()->lightList->begin() )->SetSize( Vec2( 400.0f / this->camera.scale, 400.0f / this->camera.scale ) );
  }
  Vec2 &lightPos = ( *this->lightRenderer->GetLightManager()->lightList->begin() )->position;
  lightPos.Set(
    this->camera.position.x + ( this->core->mouse.GetCursorPosition().x - float( this->windowSize.width >> 1 ) ) / this->camera.scale,
    this->camera.position.y + ( this->core->mouse.GetCursorPosition().y - float( this->windowSize.height >> 1 ) ) / this->camera.scale
    );


  this->lightRenderer->Update();

  for( auto &light: *this->lightRenderer->GetLightManager()->lightList ) {
    light->lbuffer.__Dump();
  }
}//Update



LightTest::~LightTest() {
  glDeleteTextures( 10, &this->texturesList[ 0 ] );
  DEF_DELETE( this->core );
  DEF_DELETE( __fileManager );
  DEF_DELETE( this->lightRenderer );
  if( this->lightList ) {
    for( auto &light: *this->lightList ) {
      delete &*light;
    }
  }
  DEF_DELETE( this->lightList );
}//Destroy


GLuint LightTest::GetTexture( unsigned int id ) {
  if( id < this->texturesList.size() ) {
    return this->texturesList[ id ];
  }
  return 0;
}//GetTexture


void LightTest::Redraw() {
  Vec2 v2;
  //generate lightmap
  lightTest->lightRenderer->BeginScene();

  //render scene
  glUseProgram( 0 );

  glLoadIdentity();
  Vec2 invertSize( 1.0f / ( lightTest->GetCamera().rightBottom.x - lightTest->GetCamera().leftTop.x ), 1.0f / ( lightTest->GetCamera().rightBottom.y - lightTest->GetCamera().leftTop.y ) );
  glTranslatef( -100.0f * lightTest->GetCamera().position.x * invertSize.x + 50.0f, 100.0f * lightTest->GetCamera().position.y * invertSize.y + 50.0f, 0.0f );
  glScalef( 100.0f * invertSize.x, -100.0f * invertSize.y, 1.0f );

  glEnable( GL_TEXTURE_2D );
  //glDisable( GL_TEXTURE_2D );
  glClearColor( 0.5f, 1.0f, 1.0f, 1.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  //background
  glDisable( GL_BLEND );
  //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glDisable( GL_DEPTH_TEST );
  glActiveTexture( GL_TEXTURE0 + 0 );
  glBindTexture( GL_TEXTURE_2D, lightTest->GetTexture( LIGHTTEST_TEXTURE_BACKGROUND0 ) );
  glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
  glBegin( GL_TRIANGLE_STRIP );
    glTexCoord2f( 0.0f, 1.0f );
    glVertex3f( -float( lightTest->GetWindowSize().width >> 1 ), float( lightTest->GetWindowSize().height >> 1 ), 0.0f );

    glTexCoord2f( 1.0f, 1.0f );
    glVertex3f( float( lightTest->GetWindowSize().width >> 1 ), float( lightTest->GetWindowSize().height >> 1 ), 0.0f );

    glTexCoord2f( 0.0f, 0.0f );
    glVertex3f( -float( lightTest->GetWindowSize().width >> 1 ), -float( lightTest->GetWindowSize().height >> 1 ), 0.0f );

    glTexCoord2f( 1.0f, 0.0f );
    glVertex3f( float( lightTest->GetWindowSize().width >> 1 ), -float( lightTest->GetWindowSize().height >> 1 ), 0.0f );
  glEnd();

  //blocks
  glActiveTexture( GL_TEXTURE0 + 0 );
  glBindTexture( GL_TEXTURE_2D, lightTest->GetTexture( LIGHTTEST_TEXTURE_BLOCK ) );

  glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
  Vec2 sz;
  for( auto &block: lightTest->lightRenderer->GetLightManager()->lightBlocks ) {
    block->Redraw();
  }

  glBindTexture( GL_TEXTURE_2D, 0 );


  lightTest->lightRenderer->EndScene();

  glUseProgram( 0 );

  glActiveTexture( GL_TEXTURE0 + 0 );
  glBindTexture( GL_TEXTURE_2D, 0 );
  glActiveTexture( GL_TEXTURE0 + 1 );
  glBindTexture( GL_TEXTURE_2D, 0 );
  glActiveTexture( GL_TEXTURE0 + 2 );
  glBindTexture( GL_TEXTURE_2D, 0 );
  //end scene

  glActiveTexture( GL_TEXTURE0 + 0 );
  glEnable( GL_TEXTURE_2D );
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
  glBindTexture ( GL_TEXTURE_2D, lightTest->GetTexture( LIGHTTEST_TEXTURE_BLOCK ) );
  glBegin( GL_QUADS );

  Vec2 lightSize( 3.0f, 3.0f );
  for( auto &block: *( lightTest->lightRenderer->GetLightManager()->lightList ) ) {
    v2 = ( block->position );
    v2.y = -v2.y + lightTest->GetCamera().position.y * 2.0f;
    glColor4f( block->color.x, block->color.y, block->color.z, 1.0f );
    glVertex3f( v2.x - lightSize.x, v2.y - lightSize.y, 0.0f );
    glVertex3f( v2.x + lightSize.x, v2.y - lightSize.y, 0.0f );
    glVertex3f( v2.x + lightSize.x, v2.y + lightSize.y, 0.0f );
    glVertex3f( v2.x - lightSize.x, v2.y + lightSize.y, 0.0f );
  }
  glEnd();

  //отрисовка лучей, показывающих значения l-буффера
  if( lightTest->core->keyboard.IsHold( VK_SHIFT ) ) {
    Vec2 coeff( 2.0f / ( lightTest->GetCamera().rightBottom.x - lightTest->GetCamera().leftTop.x ), 2.0f / ( lightTest->GetCamera().rightBottom.y - lightTest->GetCamera().leftTop.y ) );
    glBegin( GL_LINES );
    for( auto &block: *( lightTest->lightRenderer->GetLightManager()->lightList ) ) {
      //float step = max( 0.01f, Math::TWO_PI / float( block->lbuffer.GetSize() ) );
      glColor4f( block->color.x, block->color.y, block->color.z, 1.0f );
      //v2 = LightTest::RecalculatePosition( block->position, lightTest->GetCamera() );
      v2.Set( block->position.x, -block->position.y );
      for( auto& vert: block->vertices ) {
        //Vec2 v20 = LightTest::RecalculatePosition( vert.position, lightTest->GetCamera() );
        Vec2 v20( vert.position.x, -vert.position.y );
        glVertex3f( v2.x, v2.y, 0.0f );
        glVertex3f( v20.x, v20.y, 0.0f );
      }
    }
    glEnd();
  }
}//Redraw


Vec2 LightTest::RecalculatePosition( const Vec2& pos, const Camera& camera ) {
  return Vec2(
    ( pos.x + 1.0f ) / ( camera.rightBottom.x - camera.leftTop.x ) * 2.0f - 1.0f,
    ( pos.y + 1.0f ) / ( camera.rightBottom.y - camera.leftTop.y ) * 2.0f - 1.0f
    );
}//RecalculatePosition

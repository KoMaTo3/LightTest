#include <gl/gl3.h>


#include "lightmap.h"
#include "file.h"


using namespace LightMap;


const Vec2 LightEntity::vecAxis( 1.0f, 0.0f );
const int LightEntity::minimalEngesCountPerLight = 100;


float LightMap::GetDegreeOfPoint( const Vec2& point ) {
  Vec2 tmp( point );
  tmp.NormalizeFast();
  float v = LightEntity::vecAxis * tmp;
  float deg = Math::ACos16( v );
  if( tmp.y > 0.0f ) {
    deg = Math::TWO_PI - deg;
  }
  deg += Math::PI;
  if( deg > Math::TWO_PI ) {
    deg -= Math::TWO_PI;
  }
  return deg;
}//GetDegreeOfPoint


void ILightBlock::AddEdgeToBuffer( const Vec2& lightPosition, LBuffer *buffer, const Vec2& point0, const Vec2& point1, LBufferCacheEntity *cache ) {
  buffer->DrawLine( cache, point0 - lightPosition, point1 - lightPosition );
}//AddEdgeToBuffer


LightEntity::LightEntity( const LightType &setType, const Vec2& setPosition, const Vec4& setColor, const Vec2& setSize, float setBrightness, int lBufferSize, float setEpsilon )
  : position( setPosition ), color( setColor ), type( setType ), size( setSize ), halfSize( setSize * 0.5f ), brightness( setBrightness ), epsilon( setEpsilon )
  ,lbuffer( lBufferSize, Math::TWO_PI ), lastCheckInRect( true ), maxRange( Math::Sqrt16( setSize.x * setSize.y ) ), _lastPosition( -setPosition ), _lastSize( -setSize )
  , _lastColor( Vec4Null ), penetration( 0.0f )
{
  //__log.PrintInfo( Filelevel_DEBUG, "+1 LightEntity %p", this );
}


LightEntity::~LightEntity() {
  //__log.PrintInfo( Filelevel_DEBUG, "~LightEntity %p", this );
}


void LightEntity::Set( const LightType &setType, const Vec2& setPosition, const Vec4& setColor, const Vec2& setSize, float setBrightness ) {
  this->position = setPosition;
  this->color = setColor;
  this->type = setType;
  this->brightness = setBrightness;
  this->SetSize( setSize );
}//Set


void LightEntity::SetSize( const Vec2& setSize ) {
  this->size = setSize;
  this->maxRange = Math::Sqrt16( this->size.x * this->size.y );
}//SetSize


void LightEntity::SetColor( const Vec4& setColor ) {
  this->color = setColor;
}//SetColor


void LightEntity::SetPosition( const Vec2& setPosition ) {
  this->position = setPosition;
}//SetSize


LightEntity* LightEntity::Clone() {
  LightEntity *light = new LightEntity( this->type, this->position, this->color, this->size, this->brightness );
  return light;
}//Clone


void LightEntity::AddVertice( const LightVertex& vertex ) {
  this->vertices.push_back( vertex );
}//AddVertice


bool LightEntity::CheckBlockDistance( const Vec2& blockPos, const Vec2& blockHalfSize ) {
  return
    !(
    blockPos.x + blockHalfSize.x < this->position.x - this->size.x * 2.0f ||
    blockPos.x - blockHalfSize.x > this->position.x + this->size.x * 2.0f ||
    blockPos.y + blockHalfSize.y < this->position.y - this->size.y * 2.0f ||
    blockPos.y - blockHalfSize.y > this->position.y + this->size.y * 2.0f
    );
}//CheckBlockDistance


bool LightEntity::CheckInRect( const Vec2& leftTop, const Vec2& rightBottom ) {
  this->lastCheckInRect = 
    this->position.x + this->size.x >= leftTop.x &&
    this->position.x - this->size.x <= rightBottom.x &&
    this->position.y + this->size.y >= leftTop.y &&
    this->position.y - this->size.y <= rightBottom.y
    ;
  return this->lastCheckInRect;
}//CheckInRect


bool LightEntity::IsChanged() {
  return
    this->position != this->_lastPosition
    || this->size != this->_lastSize
    || this->color != this->_lastColor
    ;
}//IsChanged


void LightEntity::Update( LightBlocksList &blocks ) {
  //reset cache
  if( this->IsChanged() ) {
    this->lbuffer.ClearCache();
    //this->lbuffer.IsObjectCached( this, &cache );
  }

  this->lbuffer.Clear( this->maxRange );
  LBufferCacheEntity *cache;
  for( auto &block: blocks ) {
    if( this->CheckBlockDistance( block->GetPosition(), block->GetHalfSize() ) ) {
      if( this->lbuffer.IsObjectCached( block, &cache ) ) {
        this->lbuffer.WriteFromCache( cache );
      } else {
        cache->Reset( block->GetPosition(), block->GetSize() );
        block->FillBuffer( this->position, this->size, &this->lbuffer, cache );
      }
      //this->AddBlockToBuffer( block->GetPosition(), block->GetHalfSize() );
    }
  }

  this->vertices.clear();
  this->verticesSoftEdges.clear();
  this->AddVertice( LightVertex( this->position, this->color, 1.0f ) );
  float step = max( 0.01f, Math::TWO_PI / float( this->lbuffer.GetSize() ) );
  Vec2 pos, posNormalized;
  float lastRange = this->maxRange, range, oneByMaxRange = 1.0f / this->maxRange;
  float rangeHistory[ 2 ] = { -1.0f, -1.0f };
  int
    skipsCount = 0, //число пропущенных вершин
    maxSkipsCount = this->lbuffer.GetSize() / this->minimalEngesCountPerLight, //максимально допустимое число пропущенных подряд вершин. чем больше буффер, тем больше можно пропустить
    verticesCount = 0;
  for( float degree = 0.0f; degree < Math::TWO_PI; degree += step ) {
    range = this->lbuffer.GetValue( degree );
    posNormalized.Set( Math::Cos16( degree ), Math::Sin16( degree ) );
    if( this->penetration > this->epsilon ) {
      //pos += posNormalized * this->penetration;
      range = min( this->maxRange, range + this->penetration );
    }
    pos.Set(
      this->position.x + posNormalized.x * range,
      this->position.y - posNormalized.y * range
      );
    //пропустить вертексы, идущие подряд на одном расстоянии от центра
    bool skipped = false;
    if( Math::Fabs( rangeHistory[ 1 ] - range ) < this->epsilon && Math::Fabs( rangeHistory[ 0 ] - range ) < this->epsilon && skipsCount < maxSkipsCount ) {
      ++skipsCount;
      this->vertices.pop_back();
      --verticesCount;
      skipped = true;
    } else {
      /*
      if( verticesCount > 1 ) {
        //проверка вершин на расположение на одной линии, исключение лишних
        Vec2
          v0( this->vertices[ verticesCount - 1 ].position - this->vertices[ verticesCount - 2 ].position ),
          v1( pos - this->vertices[ verticesCount - 2 ].position );
        v0.NormalizeFast();
        v1.NormalizeFast();
        if( Math::Fabs( v1.Dot( v0 ) ) > 0.99f
          && Math::Fabs( rangeHistory[ 1 ] - range ) < maxRange * 0.01f
          && skipsCount < maxSkipsCount ) {
          skipped = true;
          ++skipsCount;
          this->vertices.pop_back();
          --verticesCount;
        }
      }
      */
    }
    if( !skipped ) {
      skipsCount = 0;
    }
    this->AddVertice( LightVertex( pos, this->color, 1.0f - range * oneByMaxRange ) );
    ++verticesCount;
    rangeHistory[ 1 ] = rangeHistory[ 0 ];
    rangeHistory[ 0 ] = lastRange;
    lastRange = range;
  }
  this->AddVertice( LightVertex( this->position + Vec2( this->lbuffer.GetValueByIndex( 0 ), 0.0f ), this->color, 1.0f - range * oneByMaxRange ) );
  this->_lastPosition = this->position;
  this->_lastSize = this->size;
  this->_lastColor = this->color;
}//Update

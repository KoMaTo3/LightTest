#include "lbuffer.h"
#include "math.h"
#include "file.h"


const Vec2 LBuffer::vecAxis( 1.0f, 0.0f );


LBuffer::LBuffer( int setSize, float setFloatSize )
  :size( setSize ), buffer( new float[ setSize ] ), sizeToFloat( 1.0f / float( setSize ) ), fSize( float( setSize ) ), sizeFloat( setFloatSize ), invSizeFloat( 1.0f / setFloatSize ), lightRadius( 1000.0f ), __doDump( false )
{
}


LBuffer::~LBuffer() {
  delete [] this->buffer;
}


void LBuffer::Clear( float value ) {
  for( int q = this->size; q; ) {
    this->buffer[ --q ] = value;
  }
  this->lightRadius = value;
  this->cache.Update();
}//Clear


/*
===========
  DrawPolarLine
  проецирование линии, заданной в полярной системе координат
  Vec2 lineBegin: x - угол, y - радиус (полярные координаты)
===========
*/
void LBuffer::DrawPolarLine( const Vec2& lineBegin, const Vec2& lineEnd ) {
  Vec2  pointBegin,
        pointEnd;
  if( lineBegin.x > lineEnd.x ) {
    pointBegin = lineEnd;
    pointEnd = lineBegin;
  } else {
    pointBegin = lineBegin;
    pointEnd = lineEnd;
  }
  int xBegin = this->Round( this->FloatToSize( pointBegin.x ) ),
        xEnd = this->Round( this->FloatToSize( pointEnd.x ) );
  if( xEnd < 0 || xBegin >= this->size ) {
    return;
  }
  if( xBegin < 0 ) {
    xBegin = 0;
  }
  if( xEnd > this->size - 1 ) {
    xEnd = this->size - 1;
  }

  if( xBegin == xEnd ) { //вырожденная линия
    this->buffer[ xBegin ] = pointBegin.y;
    return;
  }

  //begin
  this->_PushValue( xBegin, pointBegin.y );
  //middle
  for( int x = xBegin + 1; x < xEnd; ++x ) {
    float t = float( x - xBegin ) / float( xEnd - xBegin );
    float value = pointBegin.y * ( 1.0f - t ) + pointEnd.y * t;
    this->_PushValue( x, value );
  }
  //end
  this->_PushValue( xEnd, pointEnd.y );
}//DrawPolarLine



void LBuffer::_PushValue( int position, float value, LBufferCacheEntity *cacheElement ) {
  while( position < 0 ) {
    position += this->size;
  }
  position %= this->size;
  if( value < 0.0f ) {
    value = 0.0f;
  }
  if( value < this->buffer[ position ] ) {
    this->buffer[ position ] = value;
  }
  if( cacheElement ) {
    cacheElement->values.push_back( LBufferCacheEntity::Value( position, value ) );
  }
}//_PushValue



void LBuffer::WriteFromCache( LBufferCacheEntity *cacheEntity ) {
  cacheEntity->WriteToBuffer( this->buffer );
}//WriteFromCache



void LBuffer::ClearCache() {
  this->cache.ClearCache();
}//ClearCache



void LBuffer::ClearCache( ILBufferProjectedObject *object ) {
  this->cache.ClearCache( object );
}//ClearCache



bool LBuffer::IsObjectCached( ILBufferProjectedObject *object, LBufferCacheEntity** outCache ) {
  return this->cache.CheckCache( object, object->GetPosition(), object->GetSize(), outCache );
}//IsObjectCached



/*
===========
  DrawLine
  проецировании линии, заданной в декартовых координатах
===========
*/
void LBuffer::DrawLine( LBufferCacheEntity *cache, const Vec2& point0, const Vec2& point1 ) {
  if( !cache ) {
    __log.PrintInfo( Filelevel_WARNING, "LBuffer::DrawLine => no cache" );
    return;
  }
  if( this->__doDump ) {
    __log.PrintInfo( Filelevel_DEBUG, "LBuffer::DrawLine => point0[%3.3f; %3.3f] point1[%3.3f; %3.3f]", point0.x, point0.y, point1.x, point1.y );
  }
  Vec2
    lineBegin( Vec2( this->GetDegreeOfPoint( point0 ), point0.LengthFast() ) ),
    lineEnd( Vec2( this->GetDegreeOfPoint( point1 ), point1.LengthFast() ) );
  Vec2 linearPointBegin, linearPointEnd;

  Vec2  pointBegin,
        pointEnd;
  pointBegin = lineBegin;
  pointEnd = lineEnd;
  linearPointBegin = point0;
  linearPointEnd = point1;
  int swapCount = 0;
  if( lineBegin.x > lineEnd.x ) {
    Math::Swap( lineBegin, lineEnd );
    ++swapCount;
    Math::Swap( pointBegin, pointEnd );
    Math::Swap( linearPointBegin, linearPointEnd );
  }
  if( lineEnd.x - lineBegin.x > Math::PI ) {
    lineBegin.x += Math::TWO_PI;
    pointBegin.x += Math::TWO_PI;
    Math::Swap( lineBegin, lineEnd );
    ++swapCount;
    Math::Swap( pointBegin, pointEnd );
    Math::Swap( linearPointBegin, linearPointEnd );
  }
  if( lineBegin.x > lineEnd.x ) {
    Math::Swap( lineBegin, lineEnd );
    ++swapCount;
    Math::Swap( pointBegin, pointEnd );
    Math::Swap( linearPointBegin, linearPointEnd );
  }

  int xBegin = this->Round( this->FloatToSize( pointBegin.x + Math::TWO_PI ) ),
        xEnd = this->Round( this->FloatToSize( pointEnd.x + Math::TWO_PI ) );

  if( xBegin == xEnd ) { //вырожденная линия
    this->_PushValue( xBegin, pointBegin.y, cache );
    return;
  }

  /*
  if( xBegin < this->size >> 2 && xEnd > this->size - ( this->size >> 2 ) ) {
    int xTmp = xBegin;
    xBegin = xEnd;
    xEnd = xBegin + ( this->size - ( xBegin - xTmp ) ) % this->size;
  }
  */

  float value;
  Vec2 intersectPoint;
  xBegin += this->size - 2;
  xEnd += this->size + 2;
  int calculationStep = -1;
  int x = xBegin;
  while( calculationStep ) {
    int xValue = x % this->size;
    float a = this->SizeToFloat( xValue, 0.001f );
    if( Vec2::TestIntersect(
      Vec2Null,
      Vec2( Math::Cos( a ) * this->lightRadius * 2.0f, -Math::Sin( a ) * this->lightRadius * 2.0f ),
      linearPointBegin,
      linearPointEnd,
      &intersectPoint,
      0.01f
    ) ) {
      value = intersectPoint.LengthFast();
      this->_PushValue( xValue, value, cache );
      if( calculationStep < 0 && x <= xBegin ) {
        calculationStep = 1;
        x = xBegin;
      } else if( calculationStep > 0 && x >= xEnd ) {
        break;
      }
    } else {
      if( calculationStep < 0 && x <= xBegin ) {
        calculationStep = 1;
        x = xBegin;
      } else if( calculationStep > 0 && x >= xEnd ) {
        break;
      }
    }
    x += calculationStep;
  }
  if( this->__doDump ) {
    __log.Print( "\n" );
  }
}//DrawLine


float LBuffer::GetDegreeOfPoint( const Vec2& point ) {
  if( point.x > 0.0f && Math::Fabs( point.y ) < 0.01f ) {
    return ( point.y < 0.0f ? 0.0f : Math::TWO_PI );
  }
  Vec2 tmp( -point );
  tmp.NormalizeFast();
  float v = this->vecAxis * tmp;
  float deg = Math::ACos16( v );
  if( point.y < 0.0f ) {
    deg = Math::TWO_PI - deg;
  }
  deg += Math::PI;
  if( deg > Math::TWO_PI ) {
    deg -= Math::TWO_PI;
  }
  if( deg < 0.01f ) {
    deg = 0.0f;
  } else if( deg > Math::TWO_PI - 0.01f ) {
    deg = Math::TWO_PI;
  }
  return deg;
}//GetDegreeOfPoint


float LBuffer::GetValue( float x ) {
  if( x < 0.0f || x > this->sizeFloat ) {
    return 0.0f;
  }
  return this->buffer[ ( int ) this->FloatToSize( x ) ];
}//GetValue


float LBuffer::GetValueByIndex( int index ) {
  if( index < 0 || index >= this->size ) {
    return this->buffer[ 0 ];
  }
  return this->buffer[ index ];
}//GetValueByIndex


void LBuffer::__Dump() {
  this->__doDump = false;
  int maxValuesCount = 0;
  int normalValuesCount = 0;
  for( int q = this->size - 10; q < this->size; ++q ) {
    if( this->buffer[ q ] > 399.0f ) {
      ++maxValuesCount;
    } else {
      ++normalValuesCount;
    }
  }
  float coeff = float( min( maxValuesCount, normalValuesCount ) ) / float( max( maxValuesCount, normalValuesCount ) );
  if( coeff > 0.5f ) {
    this->__doDump = true;
    __log.Print( "LBuffer %p size[%d]\n", this, this->size );
    for( int q = 0; q < this->size; ++q ) {
      __log.Print( "%d:%3.3f\n", q, this->buffer[ q ] );
    }
    __log.Print( "=====================\n" );
  }
}//__Dump

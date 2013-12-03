#ifndef __LBUFFER_H__
#define __LBUFFER_H__


#include "klib.h"
#include "lbuffercache.h"


class ILBufferProjectedObject {
public:
  virtual const Vec2& GetPosition() const = NULL;
  virtual const Vec2& GetSize() const = NULL;
};


class LBuffer
{
public:
  LBuffer( int setSize, float setFloatSize = 1.0f );
  virtual ~LBuffer();
  void Clear( float value );
  void DrawPolarLine( const Vec2& lineBegin, const Vec2& lineEnd );
  bool IsObjectCached( ILBufferProjectedObject *object, LBufferCacheEntity** outCache );
  void DrawLine( LBufferCacheEntity *cache, const Vec2& point0, const Vec2& point1 );
  inline float GetSizeToFloatCoefficient() const {
    return this->sizeToFloat;
  }
  inline float FloatToSize( float value ) const {
    return value * this->fSize * this->invSizeFloat;
  }
  inline float SizeToFloat( int value, float epsilon = 0.0f ) const {
    float result = float( value ) * this->sizeToFloat * this->sizeFloat;
    if( epsilon > 0.0f ) {
      if( result < epsilon ) {
        result = 0.0f;
      } else if( this->sizeFloat - result < epsilon ) {
        result = this->sizeFloat;
      }
    }
    return result;
  }
  int Round( float value ) {
    return int( floorf( value ) - 0.5f );
  }
  float GetValue( float x );
  float GetValueByIndex( int index );
  inline int GetSize() const {
    return this->size;
  }
  float GetDegreeOfPoint( const Vec2& point );
  void WriteFromCache( LBufferCacheEntity *cacheEntity );
  void ClearCache();
  void ClearCache( ILBufferProjectedObject *object );
  void __Dump();
  bool __doDump;

private:
  LBuffer();
  LBuffer( const LBuffer& );
  LBuffer& operator=( const LBuffer& );
  void _PushValue( int position, float value, LBufferCacheEntity *cacheElement = NULL );

  const int size;
  const float sizeFloat;
  const float invSizeFloat;
  const float sizeToFloat;
  const float fSize;
  float *buffer;
  float lightRadius;
  static const Vec2 vecAxis;
  LBufferCache cache;
};


#endif

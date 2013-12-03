#include "lbuffercache.h"
#include "file.h"


const int LBUFFER_CACHE_LIFE_PERIOD = 10;


LBufferCacheEntity::LBufferCacheEntity()
:lifeTime( 0 ), object( NULL )
{
}


LBufferCacheEntity::LBufferCacheEntity( void* const setObject, const Vec2& setPosition, const Vec2& setSize )
  :object( setObject ), position( setPosition ), size( setSize ), lifeTime( 0 ) {
}


LBufferCacheEntity::LBufferCacheEntity( const LBufferCacheEntity& from ) {
  this->lifeTime = from.lifeTime;
  this->object = from.object;
  this->position = from.position;
  this->size = from.size;
  //this->values = from.values;
  this->values.clear();
  for( auto &value: from.values ) {
    this->values.push_back( value );
  }
}


void LBufferCacheEntity::Reset( const Vec2& setPosition, const Vec2& setSize ) {
  this->position = setPosition;
  this->size = setSize;
  this->lifeTime = 0;
  this->values.clear();
}


LBufferCache::LBufferCache() {
}


LBufferCache::~LBufferCache() {
}


bool LBufferCacheEntity::operator==( const LBufferCacheEntity& item ) const {
  return this->object == item.object;
}


void LBufferCacheEntity::WriteToBuffer( float *buffer ) {
  for( auto &value: this->values ) {
    if( buffer[ value.index ] > value.value ) {
      buffer[ value.index ] = value.value;
    }
  }
}//WriteToBuffer


LBufferCacheEntity* LBufferCache::FindElement( void *object ) {
  for( auto &item: this->cache ) {
    if( item.object == object ) {
      return &item;
    }
  }
  return NULL;
}//FindElement


LBufferCacheEntity& LBufferCacheEntity::operator=( const LBufferCacheEntity& from ) {
  this->lifeTime = from.lifeTime;
  this->object = from.object;
  this->position = from.position;
  this->size = from.size;
  this->values = from.values;
  return *this;
}


bool LBufferCache::CheckCache( void *object, const Vec2& position, const Vec2& size, LBufferCacheEntity **outCacheElement ) {
  auto element = this->FindElement( object );
  if( !element ) {
    LBufferCacheEntity *newElement = this->AddElement( object, position, size );
    if( outCacheElement ) {
      *outCacheElement = newElement;
    }
    return false;
  }
  element->lifeTime = 0;
  if( outCacheElement ) {
    *outCacheElement = element;
  }
  return
    element->position == position &&
    element->size == size;
}//CheckCache


void LBufferCache::ClearCache() {
  this->cache.clear();
}//ClearCache


void LBufferCache::ClearCache( void* object ) {
  auto
    iter = this->cache.begin(),
    iterEnd = this->cache.end();
  bool deleted = false;
  while( iter != iterEnd ) {
    if( iter->object == object ) {
      this->cache.erase( iter );
      deleted = true;
      break;
    }
    ++iter;
  }
  if( !deleted ) {
    //__log.PrintInfo( Filelevel_WARNING, "LBufferCache::ClearCache => object[%p] not found", object );
  }
}//ClearCache


LBufferCacheEntity* LBufferCache::AddElement( void *object, const Vec2& position, const Vec2& size ) {
  this->cache.push_back( LBufferCacheEntity( object, position, size ) );
  return &*this->cache.rbegin();
}//AddElement


void LBufferCache::Update() {
  ElementList::iterator
    iter,
    iterEnd = this->cache.end(),
    iterNext;
  for( iter = this->cache.begin(); iter != iterEnd; ) {
    if( ++iter->lifeTime > LBUFFER_CACHE_LIFE_PERIOD ) {
      iterNext = iter;
      ++iterNext;
      this->cache.erase( iter );
      iter = iterNext;
    } else {
      ++iter;
    }
  }
}//Update

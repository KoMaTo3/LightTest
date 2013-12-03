#pragma once
#include "types.h"

#define CREATE_SINGLETON(TYPE) \
  template class Singleton<TYPE>; \
  template<> TYPE* Singleton<TYPE>::instance = 0;


template
<typename T>
class Singleton
{
public:
  static T& Instance();

protected:
  Singleton(){}

private:
  Singleton           ( const Singleton& );
  Singleton& operator=( const Singleton& );

  static T* instance;
};

//======================

template
<typename T>
T& Singleton<T>::Instance()
{
  if( !instance )
    instance = new T;
  return *instance;
}//Instance


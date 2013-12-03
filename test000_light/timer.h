#pragma once
#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include "types.h"
#include "singleton.h"

class Timer
{
  Int64  _freq;
  Int64  _tick;
  Double _elapsed;
  Double _invFreq;
  Double _curTime;

public:
  Timer();

  Void    UpdateFreq();
  Void    UpdateCounter();
  Double  Update    ();
  inline
  Double  GetDelta  () { return this->_elapsed; }
  inline
  Float   GetDeltaF () { return Float( this->_elapsed ); }
  inline
  Double  GetTime   () { return this->_curTime; }
};

#define sTimer Singleton<Timer>::Instance()

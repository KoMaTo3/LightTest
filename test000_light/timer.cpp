#include "timer.h"

CREATE_SINGLETON(Timer);

Timer::Timer()
:_freq( 0 ), _tick( 0 ), _elapsed( 0.0 ), _curTime( 0.0 )
{
  this->UpdateFreq();
  this->UpdateCounter();
}//constructor

Void Timer::UpdateFreq()
{
  static Int64 tmp;
  QueryPerformanceFrequency( ( PLARGE_INTEGER ) &tmp );
  this->_freq = tmp;
  this->_invFreq = 1.0 / double( tmp );
}//UpdateFreq

Void Timer::UpdateCounter()
{
  static Int64 tmp;
  QueryPerformanceCounter( ( PLARGE_INTEGER ) &tmp );
  this->_tick = tmp;
}//UpdateCounter

Double Timer::Update()
{
  Int64 oldTick = this->_tick;
  this->UpdateCounter();
  this->_elapsed = double( this->_tick - oldTick ) * this->_invFreq;
  this->_curTime += this->_elapsed;

  return this->_elapsed;
}//Update

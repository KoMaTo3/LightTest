#ifndef __LOGS_H__
#define __LOGS_H__

#ifdef WIN32
#include "stdio.h"
#define LOGD(...) printf( __VA_ARGS__ )
#define LOGW(...) printf( __VA_ARGS__ )
#define LOGE(...) printf( __VA_ARGS__ )
#else
#endif

#endif

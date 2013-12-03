#pragma once

#include <stdio.h>

#ifndef assert
#define assert(a) if( !( a ) ) printf( "Fatal error: file[%s] line[%d]\n", __FILE__, __LINE__ );
#endif

#define FU_OUT
#define FU_IN

#include "kmath.h"
#include "kvector.h"
#include "kmatrix.h"
#include "kangles.h"
//#include "quat.h"
//#include "rotation.h"

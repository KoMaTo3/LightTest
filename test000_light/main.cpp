#include <memory>
#include <string>
#include <deque>


#define CHECK_GL_ERROR core->CheckGLError( __LINE__, __FILE__ )

#include "main.h"

File __log;
LightTest *lightTest = NULL;


const float epsilon = 0.01f;


int main( int argc, char* argv[] )
{
  lightTest = new LightTest( argc, argv );
  
  lightTest->Run();
  delete lightTest;
  lightTest = NULL;
  LOGD( "\n\nDone\n" );
	return 0;
}

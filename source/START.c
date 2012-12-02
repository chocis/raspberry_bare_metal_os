#include "globalParameters.h"
//#include "framebuffer.h"
#include "uart.h"
#include "debug.h"

//#include <stdio.h>

void notmain ( void )
{
    initUART(); //should be set up before debugging with uart

  // FB_initFrameBuffer();


   DEBUG("Hey debug %d works? A: %s\r\n", 55, "YOO");



    while(1){}

}

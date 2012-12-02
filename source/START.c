#include "globalParameters.h"
#include "framebuffer.h"
#include "uart.h"
#include "debug.h"


int notmain ( void )
{


   //FB_initFrameBuffer();

   initUART();

   DEBUG("Hey debug works?\r\n");



   return(0);
}

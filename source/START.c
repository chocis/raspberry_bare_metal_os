#include "types.h"
#include "globalParameters.h"
//#include "framebuffer.h"
#include "uart.h"
#include "debug.h"
#include "atags.h"

//#include <stdio.h>

void notmain ( void )
{
    initUART(); //should be set up before debugging with uart

  // FB_initFrameBuffer();


    DEBUG("Hey debug %d works? A: %s, hex: %x\r\n", 55, "YOO", 100);

    print_all_atags();



//    int address = 0x100;
//    int i;
//    for(i=0; i < 120; i++){
//
//        char temp = POINTVAL_(address);
//
//        DEBUG("#%d, Value at address 0x%x is: %x   %c\n",i, address, temp, temp);
//
//        address+=4;
//
//
//    }

    while(1){}

}

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

//    print_all_atags();



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

void c_irq_handler ( void )
{
//    unsigned int rb,rc;
//
//    //an interrupt has occurred, find out why
//    while(1) //resolve all interrupts to uart
//    {
//        rb=GET32(AUX_MU_IIR_REG);
//        if((rb&1)==1) break; //no more interrupts
//        if((rb&6)==4)
//        {
//            //receiver holds a valid byte
//            rc=GET32(AUX_MU_IO_REG); //read byte from rx fifo
//            rxbuffer[rxhead]=rc&0xFF;
//            rxhead=(rxhead+1)&RXBUFMASK;
//        }
//    }

}

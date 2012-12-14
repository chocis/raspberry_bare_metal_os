#include "types.h"
#include "globalParameters.h"
//#include "framebuffer.h"
#include "uart.h"
#include "debug.h"
#include "atags.h"
#include "interrupts.h"
#include "ringBuffer.h"

//#include <stdio.h>
extern void enable_irq ( void );


void notmain ( void )
{
  //  initUART(); //should be set up before debugging with uart
initUARTWithInterrupts();
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

/* One way to read chars on polling pending interrupt register, doesnt use ISR */
//    u32 ra;
//    while(1)
//    {
//        ra=GET32(IRQ_PEND1); //read pending interrupts register
//        if(ra&(1<<29))      //check if Mini UARTS interrupt register is set
//        {
//            //AUX_MU_IO_REG must be read, only then IRQ_PEND1 reigster bit is cleared
//            DEBUG("Received char: %c \r\n", GET32(AUX_MU_IO_REG));
//        }
//    }

    enable_irq();

    while(1){

        u32 i;

        for(i=0; i< 5000000; i++){
            dummy();
        }

        DEBUG("Currently in buffer: %s\r\n", ringGetCurrentBufferString());

        if(ringCount > 5){
            removeFirstBufferElements(5);
        }

    }

}

void c_irq_handler ( void )
{

    u32 rb,rc;

    //an interrupt has occurred, find out why
    while(1) //resolve all interrupts to uart
    {
        rb=GET32(AUX_MU_IIR_REG); //(NOTE ERROR name in docs)
        if((rb&1)==1) break; //0th bit is clear if interrupt is pending. If 1 then no interrupt. Exit ISR.
        if((rb&0b110)==0b100) //if receiver holds valid byte
        {
            //receiver holds a valid byte
            rc=GET32(AUX_MU_IO_REG); //read byte from rx fifo
            ringAddCharToBuffer(rc);
            //DEBUG("rcvd: %c\r\n", rc);
        }
    }

}

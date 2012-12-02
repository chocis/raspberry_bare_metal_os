#include "uart.h"
#include "globalParameters.h"

void uart_putc (char c){
    //the fifth bit is set when FIFO can send data
    while((POINTVAL_(AUX_MU_LSR_REG)&(1<<5)) == 0){} //wait while FIFO is ready to send data
//    POINTVAL_(AUX_MU_IO_REG) = (volatile int)c;         //send character
    PUT32(AUX_MU_IO_REG,c);  //using PUT32 style function, because POINTVAL_ is optimized out for unknown reason :(
}

void uart_putstring(char* data){

    while(*data != '\0'){
        uart_putc(*data);
        data++;
    }

}


int initUART(){

    unsigned int ra;

    /* Firstly set up GPIO pins*/
    ra = POINTVAL_(GPFSEL1);        //get current register value
    ra &= ~(0b111<<12);             //unset registers 12-14 (for GPIO 14)
    ra |= 0b010<<12;                //set alternative function 5, which enables UART (on GPIO 14)
                                    //the same for GPIO 15
    ra &= ~(0b111<<15);             //unset registers 12-14 (for GPIO 15)
    ra |= 0b010<<15;                //set alternative function 5, which enables UART (on GPIO 15)
    POINTVAL_(GPFSEL1) = ra;        //write set register value back

    /* Configure UART */
    POINTVAL_(AUX_ENABLES) = 0b1;       //set 1st bit to 1, which enables mini UART
    POINTVAL_(AUX_MU_IER_REG) = 0;      //set to 0, because interrupts are not used
    POINTVAL_(AUX_MU_LCR_REG) = 0b11;   // first bit must set to enable 8-bit mode, 2nd bit must be set for some reason so that
                                        //normal data are sent, but IMPORTANT..that is not mentioned in documentation !!
    POINTVAL_(AUX_MU_MCR_REG) = 0;      // set to 0 - nothing used
    POINTVAL_(AUX_MU_IIR_REG) = 0;      //in other tutorials this is set to 0xC6, but I think this is needed only when interrupts are used
    POINTVAL_(AUX_MU_BAUD_REG) = MINIUART_BAUD_RATE;
    POINTVAL_(AUX_MU_CNTL_REG) = 0b11;     //enable Tx, Rx


//  DONT KNOW why these pull up/down steps are performed, thats why commented out,
//  but maybe they are needed?... anyway works without these steps
//    PUT32(GPPUD,0);                 //disable pull up/down control
//    for(ra=0;ra<150;ra++) dummy(ra);
//    PUT32(GPPUDCLK0,(1<<14)|(1<<15));
//    for(ra=0;ra<150;ra++) dummy(ra);
//    PUT32(GPPUDCLK0,0);

//    PUT32(AUX_MU_CNTL_REG,3);

    uart_putstring("UART inited\r\n");

//    while(1)
//    {
//        char readVal;
//        // 0th bit is set when data ar available in FIFO and can be read
//        while((POINTVAL_(AUX_MU_LSR_REG)&(0b1)) == 0){}
//        readVal = POINTVAL_(AUX_MU_IO_REG);
//
//        uart_putc(readVal);
//        uart_putc('\n');
//    }

    return 0;
}
//-------------------------------------------------------------------------

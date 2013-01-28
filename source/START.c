#include "types.h"
#include "globalParameters.h"
//#include "framebuffer.h"
#include "uart.h"
#include "debug.h"
#include "atags.h"
#include "interrupts.h"
#include "ringBuffer.h"
#include "mmu.h"
#include "timer.h"
#include "mmc.h"
#include "external/FatFs/ff.h"

extern void enable_irq ( void );


void notmain ( void )
{
//    initUART(); //should be set up before debugging with uart
initUARTWithInterrupts();
  // FB_initFrameBuffer();


  //  DEBUG("Hey debug %d works? A: %s, hex: 0x%x\r\n", 55, "YOO", 100);

//    print_all_atags();

    DEBUG("yeah\r\n");

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
    DEBUG("after interupts\r\n");

    mmc_init();

    static FATFS fatfs;  /* File system object */
    DEBUG("Mounting result: %d ",f_mount(0, &fatfs));  /* Register volume work area (never fails) */

    static DIR dir;   /* Directory object */
    static FRESULT rc;   /* Result code */
    static FILINFO fno;  /* File information object */
    static FIL fil;   /* File object */

    rc = f_opendir(&dir, "");

    if (!rc) {
      DEBUG("Open Dir OK.\n\r");
    }else  {
      DEBUG("Open Dir Failure...error = %u\n\r",rc);
    }

    DEBUG("\nDirectory listing...\n\r");
    for (;;) {
      rc = f_readdir(&dir, &fno); /* Read a directory item */
      if (rc || !fno.fname[0]) break; /* Error or end of dir */

      if (fno.fattrib & AM_DIR)
        DEBUG("   [dir]  %s\n\r", fno.fname); // is Directory
      else
        DEBUG("[file]  size: %d  name: %s\n\r", fno.fsize, fno.fname); // is file
    }

    if (!rc) {
      DEBUG("Listing Dir complete.\n\r");
    }else  {
      DEBUG("Listing Dir Failure...error = %u\n\r",rc);
    }


    static UINT br, numread;
    static BYTE buff[64];

    DEBUG("\n\rOpen File to read...\n\r");
    rc = f_open(&fil, "CMDLINE.TXT", FA_READ | FA_OPEN_EXISTING); /* Create a file on the drive 0 */
    if(rc)  rc = f_open(&fil, "CMDLINE.TXT", FA_READ | FA_OPEN_EXISTING);// try again once

    DEBUG("-Open File fil.fs = %d\n\r",fil.fs);
    DEBUG("-Open File fil.id = %d\n\r",fil.id);
    DEBUG("-Open File fil.dsect = %d\n\r",fil.dsect);
    DEBUG("-Open File fil.clust = %d\n\r",fil.clust);
    DEBUG("-Open File fil.fsize = %d\n\r",fil.fsize);

    if (!rc) {
      DEBUG("Open File to read successful...\n\r");
    }else  {
      DEBUG("Open File to read Failure...error = %d\n\r",rc);
    }

    DEBUG("\nRead the file content from the Card:\n\r");
    for (;;) {
      rc = f_read(&fil, buff, sizeof(buff), &br);     /* Read a chunk of file */
      if (rc || !br) break;         /* Error or end of file */

        int i;
      for (i = 0; i < br; i++)                /* Type the data */
        DEBUG("%c",buff[i]);

      numread += br ;
    }

    if (!rc) {
       DEBUG("\n\r%u bytes read.\n\r", numread);
    }else  {
       DEBUG("read file Failure...error = %u\n\r",rc);
     }

    DEBUG("\nClose the file.\n\r");
    rc = f_close(&fil); // close file

    if (!rc) {
      DEBUG("File closed.\n\r");
    }else  {
      DEBUG("File close Failure...error = %u\n\r",rc);
    }



    while(1){
        dummy();
    }
//    while(1){
//
//        u32 i;
//
//        for(i=0; i< 5000000; i++){
//            dummy();
//        }
//
//        DEBUG("Currently in buffer: %s\r\n", ringGetCurrentBufferString());
//
//        if(ringCount > 5){
//            removeFirstBufferElements(5);
//        }
//
//    }

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

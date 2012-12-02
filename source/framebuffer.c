#include "colors.h"
#include "framebuffer.h"
#include "gpio.h"
#include "globalParameters.h"
#include "timer.h"



int FB_initFrameBuffer(){



	volatile struct Bcm2835FrameBuffer *fb = (volatile struct Bcm2835FrameBuffer *)(0x400000); //set address manually, must be 16-bit aligned
	//See the comments for Bcm2835FrameBuffer
	fb->width = 1024;
	fb->height = 768;
	fb->vwidth = fb->width;
	fb->vheight = fb->height;
	fb->pitch = 0;
	fb->depth = 24;
	fb->x = 0;
	fb->y = 0;
	fb->pointer = 0;
	fb->size = 0;

	if(((unsigned int)fb & 0b1111) != 0){ //this just checks that last bits are 0, which means pointer is aligned properly
		return -1;
	}

	FB_writeMessage(fb);    //first must write message
	FB_readMessage();       //and then read response


	while(1){
		volatile unsigned int i;
		//volatile unsigned int counter = 0;
		for (i = 0; i < fb->size; i++) {


		wait(1000);
			switch(i % 3) {
				// assumes 24 bit mode.
				case 0: ((volatile unsigned char *)fb->pointer)[i] = ((COLOR24_YELLOW&0xFF0000)>>16);//COLOR24.red;
				break;
				case 1: ((volatile unsigned char *)fb->pointer)[i] = ((COLOR24_YELLOW&0xFF00)>>8);
				break;
				case 2: ((volatile unsigned char *)fb->pointer)[i] = ((COLOR24_YELLOW&0xFF));
				break;
			}
		}

	}

	return 0;

}

int FB_writeMessage(volatile struct Bcm2835FrameBuffer *fb){

	volatile unsigned int statusReg;
	//write stuff
	while(1){ //wait thile framebuffer status shows that it can be used

	   	statusReg = POINTVAL_(MAILBOX_STATUS);

	   	if(((statusReg&(1<<31)) == 0)){ //if 31st bit is 0, then can proceed
	   		break;
	   	}
	}

    //data == framebuffer structure pointer and last 4 bits are set to chanel. Merge them together.
    //Remember thats why pointer is aligned, so that changing last 4 bits, the pointer can be extracted by zeroing them.
	unsigned int data = ((unsigned int)fb+0x40000000) | 0b0001; //framebuffer channel is 1

	POINTVAL_(MAILBOX_WRITE) = data; //write to mailbox pointer+chanel

	return 0;
}

int FB_readMessage(){

			volatile unsigned int statusReg;
			volatile unsigned int readValue;
			while(1){

			   while(1){

				   	statusReg = POINTVAL_(MAILBOX_STATUS);

				   	//check if status register is set to 0, then data available
				   	if(((statusReg&(1<<30)) == 0)){
				   		break; //if 30th bit is 0 then break loop
				   	}

			   }

				readValue = POINTVAL_(MAILBOX_READ);

			   	//check if the read data was for channel 1 (this is our framebuffer chanel)
			   	if(((readValue&(0b1111)) == 0b0001)){
			   		break; //if left most bit is 0 then break loop
			   	}
		   }

	return 0;
}

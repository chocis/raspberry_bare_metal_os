#include "colors.h"
#include "framebuffer.h"

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
#define GPFSEL1 0x20200004
#define GPSET0  0x2020001C
#define GPCLR0  0x20200028

#define TIMER 	0x20003004


extern void dummy ( unsigned int );



#define POINTVAL_(pointer_address) *((int*)pointer_address)
//-------------------------------------------------------------------------





void wait(unsigned int ra);
void LED_OK();
void LED_ERROR();




int FB_initFrameBuffer(){

 	

	volatile struct Bcm2835FrameBuffer *fb = (volatile struct Bcm2835FrameBuffer *)(1 << 22);
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
	
	if(((unsigned int)fb & 0b1111) != 0){
		return -1;
	}
	
	FB_writeMessage(fb);
	FB_readMessage();
	
	
	while(1){
		volatile unsigned int i;
		//volatile unsigned int counter = 0;
		for (i = 0; i < fb->size; i++) {
		
		//	COLOR24 x = red;
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
	while(1){

	   	statusReg = POINTVAL_(MAILBOX_STATUS);	   	

	   	if(((statusReg&(1<<31)) == 0)){
	   		break;
	   	}   	
	}

	unsigned int data = ((unsigned int)fb+0x40000000) | 0b0001; //framebuffer channel is 1

	POINTVAL_(MAILBOX_WRITE) = data; //write pointer+chanel
	
	return 0;
}

int FB_readMessage(){
			
			volatile unsigned int statusReg;
			volatile unsigned int readValue;
			while(1){  
			   //read stuff
			   while(1){
			   
				   	statusReg = POINTVAL_(MAILBOX_STATUS);
				   	
				   	//check if status register is set to 0, then data available
				   	if(((statusReg&(1<<30)) == 0)){
				   		break; //if left most bit is 0 then break loop
				   	}
			   
			   }
			   
				readValue = POINTVAL_(MAILBOX_READ);
			   
			   	//check if the read data was for channel 1 (this is our framebuffer chanel)
			   	if(((readValue&(0b1111)) == 0b0001)){
			   		break; //if left most bit is 0 then break loop
			   	}
		   }
		   
		   //unsigned int readValueWithoutChanel = readValue | 0xfffffff0;
		   
		   /*if(readValueWithoutChanel == 0){
		   	LED_ERROR();
		   }
		   else{
		   	LED_OK();
		   }*/
		   
	 /*while(1){	   
			if(fb->pointer == 0){
			}
			else{
				break;
			}

	}*/

  /* if(fb->pointer == 0){
		LED_ERROR();
	}
	else{
		LED_OK();				
	} */
	
	return 0;
}

void LED_OK(){
	unsigned int ra;
   
	ra = POINTVAL_(GPFSEL1);	
    ra&=~(7<<18);
    ra|=1<<18;
    POINTVAL_(GPFSEL1) = ra; 
	
    while(1)
    {
      
      	//clearing pin, actaully sets it high
        POINTVAL_(GPCLR0) = 1<<16;
 		wait(ra);
        
    }   
}

void LED_ERROR(){
	unsigned int ra;
   
	ra = POINTVAL_(GPFSEL1);	
    ra&=~(7<<18);
    ra|=1<<18;
    POINTVAL_(GPFSEL1) = ra; 
	
    while(1)
    {
        POINTVAL_(GPSET0) = 1<<16;
 		wait(100000);
      
        POINTVAL_(GPCLR0) = 1<<16;
 		wait(100000);
        
    }   
}

void wait(unsigned int timeout){

	unsigned int waitTime = timeout; //100ms
	unsigned int startTime = POINTVAL_(TIMER);
	
	while(1){
	
		//seems like must be volatile sometimes, or it will be optimesed and not wait
		volatile unsigned int currentTime = POINTVAL_(TIMER);
		
		if(currentTime - startTime >= waitTime) break;
	}
	
	
	
}





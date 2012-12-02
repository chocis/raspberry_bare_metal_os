/*
* HOW RPI FRAMEBUFFER WORKS.
* 
* Main steps overview:
* 	1) Send message to mailbox (for supplying info for GPU)
* 	2) Read message from mailbox (for reading GPU result and FB address)
*	3) Now can write to returned framebuffer and it's represented on screen
*
* 1) SEND MESSAGE 
*	1.1) Wait until status registers top bit is 0.
*	1.2) Write to Mailbox WRITE register the message ( address of frambuffer structure and channel) 
*		 IMPORTANT!! This message has to be in SPECIFIC format - pointer to fb data structure must be
*		 16bit (or is it called 4bit?) aligned. Anyway it must be placed in an addres which last 4 bits are 0000.
*/




/* This section defines constants for framebuffer Mailbox addresses */
#define MAILBOX_READ 	0x2000B880  //base adress and also READ function address
#define MAILBOX_POLL 	0x2000B890
#define MAILBOX_SENDER 	0x2000B894
#define MAILBOX_STATUS 	0x2000B898	//status address
#define MAILBOX_CONFIG 	0x2000B89C
#define MAILBOX_WRITE	0x2000B8A0	//WRITE function address


/* structure contains information about data block that must be passed to
 GPU via Mailbox. Remember it must be 16 bit aligned.
*/
struct Bcm2835FrameBuffer
{
	unsigned int width; //Width of the frame buffer (pixels)
	unsigned int height; //Height of the frame buffer
	unsigned int vwidth; //Simplest thing to do is to set vwidth = width
	unsigned int vheight; //Simplest thing to do is to set vheight = height
	unsigned int pitch; //GPU fills this in; set to zero
	unsigned int depth; //Bits per pixel; set to 24
	unsigned int x; //Offset in x direction. Simplest thing to do is set to zero
	unsigned int y; //Offset in y direction. Simplest thing to do is set to zero
	volatile unsigned int pointer; //GPU fills this in to be a pointer to the frame buffer
		//I find this is usually (always?) 0x4F...
	unsigned int size; //GPU fills this in 
};


int FB_initFrameBuffer();
int FB_writeMessage(volatile struct Bcm2835FrameBuffer *fb);
int FB_readMessage();

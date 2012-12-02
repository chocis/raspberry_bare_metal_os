#include "globalParameters.h"
#include "gpio.h"
#include "timer.h"


void wait(unsigned int timeout){

	unsigned int waitTime = timeout; //100ms
	unsigned int startTime = POINTVAL_(TIMER);

	while(1){

		//seems like must be volatile sometimes, or it will be optimesed and not wait
		volatile unsigned int currentTime = POINTVAL_(TIMER);

		if(currentTime - startTime >= waitTime) break;
	}

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

/* LED function have nothing to do with timer, but they can be used for some error indication
* if cant use UART or it isnt set up.
*/
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

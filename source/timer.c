#include "globalParameters.h"
#include "gpio.h"
#include "timer.h"

/*  RPI has a system timer that automatically runs and can be used without initialization.
*   This function simply loops for provided period of time. Of course it isn't 100% precise.
*   timeout - time to loop in MICROseconds
*/
void timerWait(unsigned int timeout){

	unsigned int waitTime = timeout; //100ms
	unsigned int startTime = getTimerCurrentValue();

	while(1){

		//seems like must be volatile sometimes, or it will be optimesed and not wait
		volatile unsigned int currentTime = POINTVAL_(TIMER);

		if(currentTime - startTime >= waitTime) break;
	}

}

u32 getTimerCurrentValue(){
    return POINTVAL_(TIMER);
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
 		timerWait(ra);

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
 		timerWait(100000);

        POINTVAL_(GPCLR0) = 1<<16;
 		timerWait(100000);

    }
}

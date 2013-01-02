#ifndef TIMER_H
#define TIMER_H

#include "types.h"

#define TIMER 	0x20003004


void timerWait(unsigned int timeout);
u32 getTimerCurrentValue();

#endif /* TIMER_H */

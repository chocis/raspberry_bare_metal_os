#ifndef DEBUG_H
#define DEBUG_H

/*
* Functions meant for variosu debugging parameters and tecnihuqes.
* Can be attached to different devices. Now only can be set to debug to Mini UART.
*/

#include "globalParameters.h"

#ifdef GLOBAL_DEBUG //define this function only when global debug flag is enabled
    #define DEBUG debug_printf
#else
    #define DEBUG(format, args...) ((void)0)
#endif

#define DEBUG_UART_TYPE


void debug_printf( char* fmt, ...);


#endif /* DEBUG_H */

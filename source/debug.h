/*
* Functions meant for variosu debugging parameters and tecnihuqes.
* Can be attached to different devices. Now only can be set to debug to Mini UART.
*/

#include "globalParameters.h"

#ifdef GLOBAL_DEBUG //define this function only when global debug flag is enabled
    #define DEBUG(msg) debug_printf(msg)
#else
    #define DEBUG(msg)
#endif

#define DEBUG_UART_TYPE 1


void debug_printf( char* fmt, ...);

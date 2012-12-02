#include "uart.h"
#include "debug.h"
#include "printf.h"

char debugBuffer[500];

void debug_printf( char* fmt, ...){

    va_list argptr;
    va_start(argptr,fmt);


//uart_putstring(arg);
    vsprintf(debugBuffer, fmt, argptr);
    va_end(argptr);

    #ifdef DEBUG_UART_TYPE
        uart_putstring(debugBuffer);
    #endif
}


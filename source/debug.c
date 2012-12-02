#include "uart.h"
#include "debug.h"

void debug_printf( char* fmt, ...){

    #ifdef DEBUG_UART_TYPE
        uart_putstring(fmt);
    #endif
}


#ifndef UART_H
#define UART_H


#define AUX_ENABLES     0x20215004  //Auxiliary enables
#define AUX_MU_IO_REG   0x20215040  //Mini UART I/O data
#define AUX_MU_IER_REG  0x20215044  //Mini UART Interrupt Enable register
#define AUX_MU_IIR_REG  0x20215048  //Mini UART Interrupt Identify register
#define AUX_MU_LCR_REG  0x2021504C  //Mini UART Line Control register
#define AUX_MU_MCR_REG  0x20215050  //Mini UART Modem Control
#define AUX_MU_LSR_REG  0x20215054  //Mini UART Line Status register
#define AUX_MU_MSR_REG  0x20215058
#define AUX_MU_SCRATCH  0x2021505C
#define AUX_MU_CNTL_REG 0x20215060  //Mini UART extra Control register
#define AUX_MU_STAT_REG 0x20215064
#define AUX_MU_BAUD_REG 0x20215068  //Mini UART Baudrate


#define INITIAL_CLOCK_SPEED 250000000  //this is initial clock speed when RPI is turned on
#define BAUD_RATE_NEEDED 115200
#define MINIUART_BAUD_RATE ((INITIAL_CLOCK_SPEED/BAUD_RATE_NEEDED)/8)-1 //  ((clockspeed/baudrate)/8)-1
//((250,000,000/115200)/8)-1 = 270


void uart_putc (char c);
void uart_putstring(char* data);
int initUART();



#endif /* UART_H */

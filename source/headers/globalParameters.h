#ifndef GLOBALPARAMETERS_H
#define GLOBALPARAMETERS_H


/*
* This file contains global helper values and parameters.
*/

#define GLOBAL_DEBUG //enable this and all DEBUG function will be called the way you configured debug functinoality


//my implementation of getting value from defined register address
#define POINTVAL_(pointer_address) *((int*)pointer_address)

extern void PUT32 ( unsigned int, unsigned int ); //other implementations
extern unsigned int GET32 ( unsigned int );
extern void dummy ( unsigned int );







#endif /* GLOBALPARAMETERS_H */

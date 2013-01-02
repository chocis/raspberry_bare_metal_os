#include "mmu.h"
#include "debug.h"

void test_mmu(){

        volatile unsigned int *memory;
    	DEBUG("MMU tests - writing data to memory\r\n");
        memory = (unsigned int *)0x00100000;
        *memory = 0xdeadbeef;
        DEBUG("0x00100000 = 0x%x \r\n", *memory);

        memory = (unsigned int *)0x00200000;
        *memory = 0xf00dcafe;
        DEBUG("0x00200000 = 0x%x \r\n", *memory);

        /* Write to 32MB */
        memory = (unsigned int *)0x02000000;
        *memory = 0xc0ffee;
        DEBUG("0x02000000 = 0x%x \r\n", *memory);

        mmu_init();
        DEBUG("MMU on - "
        "0x00100000 and 0x00200000 swapped, 0x02000000 unmapped\r\n"
        "Reading data from memory\r\n");

        memory = (unsigned int *)0x00100000;
        DEBUG("0x00100000 = 0x%x \r\n", *memory);

        memory = (unsigned int *)0x00200000;
        DEBUG("0x00200000 = 0x%x \r\n", *memory);

        /* Write to 32MB */
        memory = (unsigned int *)0x02000000;
        DEBUG("0x02000000 = 0x%x \r\n", *memory);
}

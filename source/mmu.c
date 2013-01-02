#include "debug.h"

//  Don't know if its a good idea to assign page table dynamically. I do it in init function statically specifying address.
//  This is just backup code. REMEMBER if using this, then have to change & in ((unsigned int) pagetable)
//    unsigned int pagetable[4096]	__attribute__ ((aligned (16384)));
//    unsigned int pagetable0[32]	__attribute__ ((aligned (16384)));

/* PERFORMANCE Bemchmark read in mmu.h */

/*
    MMU - Memory Management Unit translates virtual addresses to physical addresess if it is enabled and found in processor.
    Raspeberry Pi ARM processor has this unit included. MMU is most useful in operating systems, when there is clear difference
    between user and kernel space, because then permissions and translation can be used.

    When an MMU is in use it is able to automatically convert virtual addresses to physical addresses –
    however in order to do this it extensively utilises (at a performance cost) a set of translation tables
    (sometimes known as page tables) stored in physical memory. Therefore prior to enabling the MMU these tables must
    be appropriately set up and the hardware must be told where in memory they can be found. The translation tables are
    typically set up very early on during boot by an operating system.

    The ARM MMU supports entries in the translation tables which can represent either an entire 1MB (section), 64KB (large page),
    4KB (small page) or 1KB (tiny page) of virtual memory.
    MMU has a dedicated cache for making a note of recent translations – it’s known as a Translation Lookaside Buffer (TLB).
*/


/*
    In this function assembly command MCR is used. Because MMU is an ARM
    co-processor, this is the only way to communicate with it.
    http://www.heyrick.co.uk/assembler/coprocmnd.html#mcr
    According to ARM documenatation, MMU is co-processor NR 15.

    SYNTAX:
     MCR    <co-pro>, <op>, <ARM reg>, <co-pro reg>, <co-pro reg2>, <op2>

    In my case, I will use 1MB section and map all memory one-to-one (physycal == virtual) just for testing purposes.
    There is no real gain using MMU this way, but its a good start for learning.

    MMU documentation: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0198e/Babegida.html

    Bits 31:20 - Section base address
    Bits 11:10 - Access permissions
    Bits 8:5   - Domain
    Bit  3:2   - Cachable / Bufferable
    Bits 1:0   - Always 0b10 for a section page table entry descriptor

*/

void mmu_init(void)
{

    /* MY IMPLEMENTATION */
    unsigned int x;

    //right over the stack pointer, REMEMBER, it has to be at 16k boundary
    unsigned int* pagetable = (unsigned int*)0x7000000;

    /*  Insert all addresses into page table. The bits 20:31 have to contain the physycal address aligned with 1 MB.
    *   The 0:1 bits has to contain 0b10, because I am using 1MB sections. As i said, I am mapping all therotetical RAM 4096*1MB = 4GB.
    */
    for(x=0; x<4096; x++){
        pagetable[x] = x<<20 | 0b10;
    }

    /*  Uncomment this if want to see how MMU works. It just swaps the two addresses around and if you access them after
    *   MMU is initialized then values will be swapped too.
     */
//    pagetable[1] = 2<<20 | 2;	/* 1MB -> 2MB */
//    pagetable[2] = 1<<20 | 2;	/* 2MB -> 1MB */

    /* Now we must tell the MMU where to find page tables.
    *   MMU co-processor c2 register holds the physical address of the base of the translation table maintained in main memory.
    *   This base address must be on a 16KB boundary.
    *   The assembly command would look like this (mcr p15, 0, r0, c2, c0, 0), where r0 contains base address.
    */
    asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" ((unsigned int) pagetable));

    /* MMU register c3 specifies domain access control.
    *  Domain is like permission file to which page table entries can be assigned to. This way it is easy to change configuration
    *  of many page entries in one shot. There can be used multiple domains, but for now we are using only one - 0.
    *   We are setting the permission to Manager, which value is 0b11.
    */
    asm volatile("mcr p15, 0, %[r], c3, c0, 0" : : [r] "r" (0b11));

    /*  Now we can turn on MMU, which can be done via register c1.
    *   First we read value of this register. Then enable the first bit and write it back.
    */

    register unsigned int control;
    asm volatile("mrc p15, 0, %[control], c1, c0, 0" : [control] "=r" (control));
    control |= 1;
    asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));

    /* END OF MY IMPLEMENTATION */
    //*********************************************************************************************************
    /* OLD IMPLEMENTATION - this is an example, created by brianwiddas */

//    unsigned int x;
//    unsigned int pt_addr;
//    unsigned int pt0_addr;
//    register unsigned int control;

    /* Translation table 1 - covers the entire memory
    * Map each 1MB virtual page to its physical equivalent (as if the
    * MMU wasn't enabled), except for the first 64 MB, which return
    * page faults
    */
//    for(x=0; x<4096; x++)
//    {
//        pagetable[x] = (x>64)?(x<<20 | 2):0;
//    }

    /* Translation table 0 - covers the first 32 MB (see below).
    * Map each 1MB page to its physical equivalent, except for
    * 1MB-2MB and 2MB-3MB
    * Between it and TT1, leaves a hole at 32-64MB
    */
//    for(x=0; x<32; x++)
//    {
//        pagetable0[x] = x<<20 | 2;
//    }

    /* Swap two pages round */
//    pagetable0[1] = 2<<20 | 2;	/* 1MB -> 2MB */
//    pagetable0[2] = 1<<20 | 2;	/* 2MB -> 1MB */

//    pt_addr = (unsigned int) &pagetable;
//    pt0_addr = (unsigned int) &pagetable0;



//    /* Translation table 0 - ARM1176JZF-S manual, 3-57 */
//    asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pt0_addr));
//    /* Translation table 1 */
//    asm volatile("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (pt_addr));
//    /* Use translation table 0 up to 32MB */
//    asm volatile("mcr p15, 0, %[n], c2, c0, 2" : : [n] "r" (7));
//
//    /* Set Domain 0 ACL to "Manager" - ARM1176JZF-S manual, 3-64
//    * Every page is in domain 0 at the moment
//    */
//    asm volatile("mcr p15, 0, %[r], c3, c0, 0" : : [r] "r" (0x3));
//
//    /* Turn on MMU */
//    asm volatile("mrc p15, 0, %[control], c1, c0, 0" : [control] "=r" (control));
//    control |= 1;
//    asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));


    /* END OF OLD IMPLEMENTATION */

}

/*This is really the entry point for RPI. Creating _start fot that.
* Setting up stack pointer and then branching to notmain
*/

/*.globl _start
_start:
    mov sp,#0x100000  ;@ FIXME - should think about stack pointer to different place

    bl notmain
*/


/* The first procedure in this assembler file is the first that will be called and
* located at address 0x8000 (in RPi case). When using interrupts it probably should contain
* vector table, so that it can be easily copied to address 0x0000, because thats the real place for
* interrupt vectors. When RPi starts it branches to address 0x8000 or if using bootloader it branches to 0x8000 and then
* executes first instruction, which in this case is reset_handler, which copies vector table to address 0x0000. Then if any relevant
* interrupt occurs (i.e. mini uart IRQ) it calls it at address 0x0018.
*/
.globl _start
_start:
    ldr pc,reset_handler
    ldr pc,undefined_handler
    ldr pc,swi_handler
    ldr pc,prefetch_handler
    ldr pc,data_handler
    ldr pc,unused_handler
    ldr pc,irq_handler
    ldr pc,fiq_handler

/* This intermediate table is used, because LDR instruction can't directly assign function address */
reset_handler:      .word   reset    /* only reset and irq handlers are defined for now */
undefined_handler:  .word   hang
swi_handler:        .word   hang
prefetch_handler:   .word   hang
data_handler:       .word   hang
unused_handler:     .word   hang
irq_handler:        .word   irq      /* only reset and irq handlers are defined for now */
fiq_handler:        .word   hang

/*This procedure is called on reset, at least it should be */
reset:
    mov r0,#0x8000
    mov r1,#0x0000
    /* LDMIA (load multiple, increment after) instruction takes r0 as base addres from where to load values, and loads them
    * in bracket {} provided registers. After that it is increamaneted by numberofregisters*4 bytes */
    ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9} /* so this insctruction copies all vector table from address 0x8000 in registers */
    stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9} /* and then stores them in memory provided in register r1 , which is 0x0000 */

    /* Similary to previous two instructions now the intermediate handlers table is copied right after vector table.
    * Remember that r0 adn r1 are now icremented. Because ARM mostly uses relative adressing, this simple copying will work
    * out of the box, because vector table will relatively point to intermediate table and even wont matter if they are located at 0x000+0x24
    * or 0x8000+0x24. But those handlers contain values (absoulte adresses) to interrupt functions and branch to them succesfuuly (above 0x8000)
    */
    ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}
    stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}




    /* MSR instruction allows us to set general status registers. In this case CPSR (current program status register).
    * 0th-4th bit determines processor mode, and the rest three (in this case 110) sets these flags I,F,T.
    * Now we go to each processor mode and set stack pointer for it (because each mode has its own stack register)
    */

    /* 10010 is IRQ mode */
    mov r0,#0b11010010
    msr cpsr_c,r0
    mov sp,#0x8000 /* set stack pointer for this mode */

    /* 10001 is FIQ mode*/
    mov r0,#0b11010001
    msr cpsr_c,r0
    mov sp,#0x4000 /* set stack pointer for this mode */

    /* 10011 is SUPERVISOE mode */
    mov r0,#0b11010011
    msr cpsr_c,r0
    mov sp,#0x8000000 /* set stack pointer for this mode */

    bl notmain


/* This procedure is called on IRQ interrupt */
irq:
    /* save all reigsters from previous state and branch to ISR, then restore registers and go back to executing previous code */
    push {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
    bl c_irq_handler
    pop {r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
    subs pc,lr,#4 /* in arm documentation it is said - to return to previous execution subtract 4 bytes from previous pc (which is in lr) */

.globl enable_irq
enable_irq:
    mrs r0,cpsr  /*MRS reads from global status register */
    bic r0,r0,#0x80 /* 0b10000000   BIC is AND NOT instruction which zerous out specific bit, in this case IRQ flag */
    msr cpsr_c,r0 /* write back to CPSR */
    bx lr




hang: b hang

/* ===========================================================
* dymmy can be used to overcome gcc -O2 optimisation, when something is overoptimezed and lost
* then using this function shouldn't optimize. There is of course volatile option, but that
* ends up with bigger binary result file (more commands).
*/

.globl dummy
dummy:
    bx lr


.globl PUT32
PUT32:
    str r1,[r0]
    bx lr

.globl GET32
GET32:
    ldr r0,[r0]
    bx lr



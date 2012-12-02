;@ This is really the entry point for RPI. Creating _start fot that.
;@ Setting up stack pointer and then branching to notmain


.globl _start
_start:
    mov sp,#0x8000  ;@ FIXME - should think about stack pointer to different place
    				;@ for now it has 0x8000 bytes of memory
    bl notmain

;@ ===========================================================
;@ dymmy can be used to overcome gcc -O2 optimisation, when something is overoptimezed and lost
;@ then using this function shouldn't optimize. There is of course volatile option, but that
;@ ends up with bigger binary result file (more commands).

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




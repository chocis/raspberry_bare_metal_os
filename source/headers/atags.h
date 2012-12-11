#ifndef ATAGS_H
#define ATAGS_H

/* ATAGs are parameters that should be passed to kernel by bootloader.
* ARM preferred ATAGs are described here http://www.simtec.co.uk/products/SWLINUX/files/booting_article.html.
* Memory address at which atags are located, should be set in register r2 after ARM processor reset. Most of the time
* they are located at address 0x100. RPI doesn't use all standart ATAGs, but some of them pass in as CMDLINE parameter.
*/

#include "types.h"

/* list of possible tags */
#define ATAG_NONE       0x00000000      //this is used in RPI
#define ATAG_CORE       0x54410001      //this is used in RPI
#define ATAG_MEM        0x54410002      //this is used in RPI
#define ATAG_VIDEOTEXT  0x54410003
#define ATAG_RAMDISK    0x54410004
#define ATAG_INITRD2    0x54420005
#define ATAG_SERIAL     0x54410006
#define ATAG_REVISION   0x54410007
#define ATAG_VIDEOLFB   0x54410008
#define ATAG_CMDLINE    0x54410009      //this is used in RPI


/* structures for each atag */
struct atag_header {
        u32 size; /* length of tag in words including this header */
        u32 tag;  /* tag type */
};

struct atag_core {
        struct atag_header header;
        u32 flags; //these three values are mandatory
        u32 pagesize;
        u32 rootdev;
};

struct atag_mem {
        struct atag_header header;
        u32     size;
        u32     start;
};

struct atag_cmdline {
        struct atag_header header;
        char    cmdline;
};



void print_all_atags();


#endif /* ATAGS_H */

#include "atags.h"
#include "debug.h"

static void print_atag_core(struct atag_core* data){

    //CORE atags size can be 5 containing extra info or 2(only atag header)
    DEBUG("ATAG core: ");
    if(data->header.size == 5){
        DEBUG("Flags: 0x%x, pagesize: 0x%x, rootdev: 0x%x \n", data->flags, data->pagesize, data->rootdev);
    }
    else{
        DEBUG("no additional info. \n");
    }
}

static void print_atag_mem(struct atag_mem* data){

    DEBUG("ATAG memory: ");
    DEBUG("RAM size:%d bytes (%d MB), starting at address 0x%x \n", data->size, data->size/1024/1024, data->start);
}

static void print_atag_cmdline(struct atag_cmdline* data){

    DEBUG("CMD line: ");
    DEBUG("%s\n", &data->cmdline);
}

void print_all_atags(){

    struct atag_header* atags = (struct atag_header *)0x100; //FIXME this address should be read from register r2 after ARM reset
    unsigned int tag;


    do{
        tag = atags->tag; //read tag type from header
        DEBUG("##ATAG at address 0x%x is 0x%x , size of atag: %d\n", atags, tag, atags->size);

        switch(tag){
            case ATAG_NONE:
                DEBUG("Atag NONE reached\n");
                break;
            case ATAG_CORE:
                print_atag_core((struct atag_core*)atags);
                break;
            case ATAG_MEM:
                print_atag_mem((struct atag_mem*)atags);
                break;
            case ATAG_CMDLINE:
                print_atag_cmdline((struct atag_cmdline*)atags);
                break;
            default:
                DEBUG("Unknown ATAG\n");
                return;

        }





        //move atags pointer size*4 bytes forward, to the next atag element
        atags = (struct atag_header*)((u32)atags + (atags->size*4));
    }
    while(tag != ATAG_NONE); //while isnt reached last tag


}






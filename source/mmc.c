#include "debug.h"
#include "mmc.h"
#include "timer.h"

struct Mmc_cid_data* mmc_cid_data;
const char* monthNames[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September",
                            "October", "November", "December"};

// taken from http://en.wikipedia.org/wiki/Partition_type
const char* MBRPartitionTypes[] = { [0x00] = "Empty partition entry",
                                    [0x01] = "FAT12 as primary partition",
                                    [0x02] = "XENIX root",
                                    [0x03] = "XENIX usr",
                                    [0x04] = "FAT16 with less than 65536 sectors ",
                                    [0x05] = "Extended partition with CHS addressing",
                                    [0x06] = "FAT16B with 65536 or more sectors",
                                    [0x07] = "NTFS, exFAT",
                                    [0x08] = "OS/2 or ...",
                                    [0x09] = "AIX data/boot or ...",
                                    [0x0a] = "OS/2 Boot Manager or ...",
                                    [0x0b] = "FAT32 with CHS addressing",
                                    [0x0c] = "FAT32X with LBA",
                                    [0x0e] = "FAT16X with LBA",
                                    [0x0f] = "Extended partition with LBA ",
                                    [0x11] = "Logical sectored FAT12 or FAT16[",
                                    [0x12] = "configuration partition (bootable FAT)  or ...",
                                    [0x14] = "Omega filesystem or ...",
                                    [0x15] = "Hidden extended partition with CHS addressing ",
                                    [0x83] = "Linux native partition",
                                    }; // this of course isnt full list TODO...finish this, but not really needed


struct MBRpartitionEntry{

    u8 bootInd; //boot indicator
    u8 startingHead;
    u8 startingSector;      // 6 bits really
    u16 startingCylinder;   // 10 bits really
    u8 systemId;    //Defines volume type
    u8 endingHead;
    u8 endingSector;      // 6 bits really
    u16 endingCylinder;   // 10 bits really
    u32 relativeSectors;
    u32 totalSectors;
};

struct MBRpartitionEntry* partitEntries[4]; //max there can be 4 primary partitions


u32 mbr[128];

//helper function to extract u32 value from an unaligned array
u32 extractU32fromU8(u8* startAddress){
    u32 temp = 0;
    temp |= *startAddress;
    startAddress++;
    temp |= (*startAddress)<<8;
    startAddress++;
    temp |= (*startAddress)<<16;
    startAddress++;
    temp |= (*startAddress)<<24;

    return temp;
}

//process master boot record
void processMBR(){


    //check typical "valid bootsector" signature - last two bytes
    if( ((u8*)mbr)[511] == 0xaa && ((u8*)mbr)[510] == 0x55 ){
        DEBUG("MBR signature is valid.\r\n");

        u16 partitionStart = 446;
        int n;
        for( n=0; n<4; n++ ){


            partitEntries[n]->bootInd = ((u8*)mbr)[partitionStart];
            partitEntries[n]->startingHead = ((u8*)mbr)[partitionStart+1];
            partitEntries[n]->startingSector = (((u8*)mbr)[partitionStart+2])&0b00111111; //only 6bits represent sector count
            partitEntries[n]->startingCylinder = (((((u8*)mbr)[partitionStart+2])&0b11000000)<<2) |
                                                         (((u8*)mbr)[partitionStart+3]);
            partitEntries[n]->systemId = ((u8*)mbr)[partitionStart+4];
            partitEntries[n]->endingHead = ((u8*)mbr)[partitionStart+5];
            partitEntries[n]->endingSector = (((u8*)mbr)[partitionStart+6])&0b00111111; //only 6bits represent sector count
            partitEntries[n]->endingCylinder = (((((u8*)mbr)[partitionStart+6])&0b11000000)<<2) |
                                                         (((u8*)mbr)[partitionStart+7]);

            partitEntries[n]->relativeSectors = extractU32fromU8(&(((u8*)mbr)[partitionStart+8]));
            partitEntries[n]->totalSectors = extractU32fromU8(&(((u8*)mbr)[partitionStart+12]));

            DEBUG("================= Partition entry NR: %d =================\r\n", n+1);

            if(partitEntries[n]->systemId != 0){
                DEBUG("Boot indicator: 0x%x \r\n", partitEntries[n]->bootInd);
                DEBUG("Starting CHS values: %d, %d, %d \r\n", partitEntries[n]->startingCylinder,
                                            partitEntries[n]->startingHead, partitEntries[n]->startingSector);
                DEBUG("Ending CHS values: %d, %d, %d \r\n", partitEntries[n]->endingCylinder,
                                            partitEntries[n]->endingHead, partitEntries[n]->endingSector);
                DEBUG("File system type: %s \r\n", MBRPartitionTypes[partitEntries[n]->systemId]);
                DEBUG("Relative sectors: 0x%x (%d in decimal)  \r\n", partitEntries[n]->relativeSectors, partitEntries[n]->relativeSectors);
                DEBUG("Total sectors: 0x%x (%d in decimal)  \r\n", partitEntries[n]->totalSectors, partitEntries[n]->totalSectors);
            }
            else{
                DEBUG("Partition entry is EMPTY!!\r\n");
            }
            partitionStart+=16;
        }

    }
    else{
        DEBUG("Error: MBR bootsector signature isn't correct!!!!!!\r\n");
    }


}

/*  Parse CID parameters from all 4 response registers.
*   In Raspberyy case all data must be shifted right 8bits, because there is no CRC value.
*/
void mmcReadCIDData(u32 resp0, u32 resp1, u32 resp2, u32 resp3){


    mmc_cid_data->MID = (resp3&0x00ff0000) >> 16;
    mmc_cid_data->OID[0] = (resp3&0x0000ff00) >> 8;
    mmc_cid_data->OID[1] = (resp3&0x000000ff);
    mmc_cid_data->OID[2] = '\0';
    mmc_cid_data->PNM[0] = (resp2&0xff000000) >> 24;
    mmc_cid_data->PNM[1] = (resp2&0x00ff0000) >> 16;
    mmc_cid_data->PNM[2] = (resp2&0x0000ff00) >> 8;
    mmc_cid_data->PNM[3] = (resp2&0x000000ff);
    mmc_cid_data->PNM[4] = (resp1&0xff000000) >> 24;
    mmc_cid_data->PNM[5] = '\0';
    mmc_cid_data->PRV_major = (resp1&0x00f00000) >> 20;
    mmc_cid_data->PRV_minor = (resp1&0x000f0000) >> 16;
    mmc_cid_data->PSN = ((resp1&0x0000ffff) << 16) + ((resp0&0xffff0000) >> 16);
    mmc_cid_data->MDT_year = ((resp0&0x00000ff0) >> 4) + 2000;
    mmc_cid_data->MDT_month = (resp0&0x0000000f);

}

void mmcPrintCID(){
    DEBUG("======= SD card CID data ========");
    DEBUG("Manufacturer ID: 0x%x\r\n", mmc_cid_data->MID);  //manufac 0x3 is SanDisk
    DEBUG("OEM OID: %s\r\n", mmc_cid_data->OID);
    DEBUG("Product name: %s\r\n", mmc_cid_data->PNM);
    DEBUG("Product revision: %d.%d\r\n", mmc_cid_data->PRV_major, mmc_cid_data->PRV_minor);
    DEBUG("Product serial number: 0x%x\r\n", mmc_cid_data->PSN);
    DEBUG("Manufacturing date:  %d %s\r\n", mmc_cid_data->MDT_year, monthNames[mmc_cid_data->MDT_month - 1]);
}

void mmc_init(void)
{

    DEBUG("Start MMC initialization.\r\n");

    initialMmcInit();

  //  mmc_cid_data->first = 5;

    /* GO_IDLE_STATE (CMD0) - Resets the SD Memory Card*/
    POINTVAL_(MMC_ARG1) = 0x00000000;
    POINTVAL_(MMC_CMDTM) = 0x00000010; //0b00010000 - Direction of data transfer: 1 = from card to host
    timerWait(1000);

    /* SD_SEND_IF_COND (CMD8) - Sends SD Memory Card interface condition that includes Host Supply Voltage (VHS)
    information and asks the accessed card whether card can operate in supplied voltage range.*/
    //0x00000142 binary is "00000000 00000000 00000001 01000010"
    POINTVAL_(MMC_ARG1) = 0x00000142;

    //0x08030010 binary is "00001000 00000011 00000000 00010000"
    //  4 - Direction of data transfer: 1 = from card to host
    //  16:17 - Type of expected response from card: 11 = 48 bits response using busy
    //  24:29 - (001000) Index of the command to be issued to the card (check some sd card info page for command indexes)
    POINTVAL_(MMC_CMDTM) = 0x08030010;
    timerWait(1000);

    u32 response = POINTVAL_(MMC_RESP0);
    DEBUG("Response after SD_SEND_IF_COND: 0x%x\r\n", response);

    do{
        //APP_CMD (CMD55)  - Defines to the card that the next command is an application specific command rather than a standard command.
        POINTVAL_(MMC_ARG1) = 0x00000000;
        //0x37020000 binary is "00110111 00000010 00000000 00000000"
        //  16:17 - Type of expected response from card: 10 = 48 bits response (WITHOUT BUSY)
        //  24:29 - (110111) Index of the command to be issued to the card
        POINTVAL_(MMC_CMDTM) = 0x37020000;
        timerWait(1000);

        response = POINTVAL_(MMC_RESP0);
        DEBUG("Response after APP_CMD: 0x%x\r\n", response);


        //SD_SEND_OP_COND (ACMD41)  - Sends host capacity support information and activates the card's initialization process.
        //0x40300000 binary is "01000000 00110000 00000000 00000000"
        //  20:21 --?? dont know why 11
        //  30 - HCS enabled
        POINTVAL_(MMC_ARG1) = 0x40300000;
        //0x29020000 binary is "00101001 00000010 00000000 00000000"
        //  16:17 - Type of expected response from card: 10 = 48 bits response (WITHOUT BUSY)
        //  24:29 - (101001) Index of the command to be issued to the card
        POINTVAL_(MMC_CMDTM) = 0x29020000;
        timerWait(1000);

        response = POINTVAL_(MMC_RESP0);
        DEBUG("Response after SD_SEND_OP_COND: 0x%x, anded: 0x%x\r\n", response, (response&(1<<31)));

        //timerWait(1000000);
    }while((response&(1<<31)) == 0); //loop until 31st bit is set to 1


    //ALL_SEND_CID (CMD2)  - Request all cards to send their unique CID numbers.
    POINTVAL_(MMC_ARG1) = 0x00000000;
    //0x02010000 binary is "00000010 00000001 00000000 00000000"
    //  16:17 - Type of expected response from card: 01 = 136 bits response  !! IMPROTANT - needed in this case !!!
    //  24:29 - (000010) Index of the command to be issued to the card
    POINTVAL_(MMC_CMDTM) = 0x02010010;
    timerWait(1000);


    u32 resp0; u32 resp1; u32 resp2; u32 resp3;
    response = POINTVAL_(MMC_RESP0);
    DEBUG("Response after MMC_RESP0 ALL_SEND_CID: 0x%x\r\n", response);
    resp0 = response;

    response = POINTVAL_(MMC_RESP1);
    DEBUG("Response after MMC_RESP1 ALL_SEND_CID: 0x%x\r\n", response);
    resp1 = response;

    response = POINTVAL_(MMC_RESP2);
    DEBUG("Response after MMC_RESP2 ALL_SEND_CID: 0x%x\r\n", response);
    resp2 = response;

    response = POINTVAL_(MMC_RESP3);
    DEBUG("Response after MMC_RESP3 ALL_SEND_CID: 0x%x\r\n", response);
    resp3 = response;

    mmcReadCIDData(resp0, resp1, resp2, resp3);
    mmcPrintCID();
    //SEND_RELATIVE_ADDR (CMD3)  - Asks the card to publish a new relative address
    POINTVAL_(MMC_ARG1) = 0x00000000;
    //0x031A0000 binary is "00000011 00011010 00000000 00000000"
    //  16:17 - Type of expected response from card: 10 = 48 bits response (WITHOUT BUSY)
    //  19 - Check the responses CRC
    //  20 - Check that response has same index as command
    //  24:29 - (000011) Index of the command to be issued to the card
    POINTVAL_(MMC_CMDTM) = 0x031A0000;
    timerWait(1000);

    response = POINTVAL_(MMC_RESP0);
    DEBUG("Response after SEND_RELATIVE_ADDR: 0x%x\r\n", response);

 /*   //second time
    //SEND_RELATIVE_ADDR (CMD3)  - Asks the card to publish a new relative address
    POINTVAL_(MMC_ARG1) = 0x00000000;
    //0x031A0000 binary is "00000011 00011010 00000000 00000000"
    //  16:17 - Type of expected response from card: 10 = 48 bits response (WITHOUT BUSY)
    //  19 - Check the responses CRC
    //  20 - Check that response has same index as command
    //  24:29 - (000011) Index of the command to be issued to the card
    POINTVAL_(MMC_CMDTM) = 0x031A0000;
    timerWait(1000);

    response = POINTVAL_(MMC_RESP0);
    DEBUG("Response after SEND_RELATIVE_ADDR: 0x%x\r\n", response);
*/


    DEBUG("Response after SEND_CSD  without shifting: 0x%x\r\n", response);
    DEBUG("Response after SEND_CSD  shifted: 0x%x\r\n", ((response >> 16) << 16));
    u32 rcavalue = ((response >> 16) << 16);
     //second time
    //SEND_CSD (CMD9) - Addressed card sends its card-specific data (CSD) on the CMD line.
    POINTVAL_(MMC_ARG1) = rcavalue;
    //0x031A0000 binary is "00000011 00011010 00000000 00000000"
    //  16:17 - Type of expected response from card: 10 = 48 bits response (WITHOUT BUSY)
    //  19 - Check the responses CRC
    //  20 - Check that response has same index as command
    //  24:29 - (000011) Index of the command to be issued to the card
    POINTVAL_(MMC_CMDTM) = 0x09010000;
    timerWait(1000);

    response = POINTVAL_(MMC_RESP0);
    DEBUG("Response after MMC_RESP0 SEND_CSD: 0x%x\r\n", response);
    response = POINTVAL_(MMC_RESP1);
    DEBUG("Response after MMC_RESP1 SEND_CSD: 0x%x\r\n", response);
    response = POINTVAL_(MMC_RESP2);
    DEBUG("Response after MMC_RESP2 SEND_CSD: 0x%x\r\n", response);
    response = POINTVAL_(MMC_RESP3);
    DEBUG("Response after MMC_RESP3 SEND_CSD: 0x%x\r\n", response);




    //SELECT/DESELECT_CARD  (CMD7) -Command toggles a card between the stand-by and transfer states
    POINTVAL_(MMC_ARG1) = rcavalue;
    //0x031A0000 binary is "00000011 00011010 00000000 00000000"
    //  16:17 - Type of expected response from card: 10 = 48 bits response (WITHOUT BUSY)
    //  19 - Check the responses CRC
    //  20 - Check that response has same index as command
    //  24:29 - (000011) Index of the command to be issued to the card
    POINTVAL_(MMC_CMDTM) = 0x071B0000;
    timerWait(1000);

    response = POINTVAL_(MMC_RESP0);
    DEBUG("Response after SELECT/DESELECT_CARD : 0x%x\r\n", response);


    //SET_BLOCKLEN  (CMD16)  - sets block length...512 bytes
    POINTVAL_(MMC_ARG1) = 0x0000200;
    //0x031A0000 binary is "00000011 00011010 00000000 00000000"
    //  16:17 - Type of expected response from card: 10 = 48 bits response (WITHOUT BUSY)
    //  19 - Check the responses CRC
    //  20 - Check that response has same index as command
    //  24:29 - (000011) Index of the command to be issued to the card
    POINTVAL_(MMC_CMDTM) = 0x10020010;
    timerWait(1000);

    response = POINTVAL_(MMC_RESP0);
    DEBUG("Response after SET_BLOCKLEN : 0x%x\r\n", response);


    //SET BLOCK SIZE REG
    POINTVAL_(MMC_BLKSIZECNT) = 0x00200;


    mmcReadData((u8*)mbr, 0, 1);



    processMBR();



}



void initialMmcInit(){

    //0x070F0000 binary represantation is "00000111 00001111 00000000 00000000"
    //wich sets these bits:
    //  16:19 - Data timeout unit exponent: 1111 = disabled
    //  24:26 - reset (complete host, command handling and data handling) circuits
    POINTVAL_(MMC_CONTROL1) = 0x070F0000;
    timerWait(1000);

    /******************************************************/
    //0x000F0000 binary represantation is "00000000 00001111 00000000 00000000"
    //wich sets these bits:
    //  16:19 - Data timeout unit exponent: 1111 = disabled
    POINTVAL_(MMC_CONTROL1) = 0x000F0000;
    timerWait(1000);

    /******************************************************/
    //0x00100000 binary represantation is "00000000 00010000 00000000 00000000"
    //wich sets these bits:
    //  20 - set SPI mode
    POINTVAL_(MMC_CONTROL0) = 0x00100000;

    /******************************************************/
    //0x000F0F25 binary represantation is "00000000 00001111 00001111 00100101"
    //wich sets these bits:
    //  0 - Clock enable for internal EMMC clocks for power saving
    //  2 - SD clock enable
    //  5 - Mode of clock generation: programmable
    //  8:15 - SD clock base divider LSBs (in this case value is 0b00001111)
    //  16:19 - Data timeout unit exponent: 1111 = disabled
    POINTVAL_(MMC_CONTROL1) = 0x000F0F25;
    POINTVAL_(MMC_CONTROL1) = 0x000F0F25;    //two times??!! TODO - check for stable clock bit, other use it

    /******************************************************/
    POINTVAL_(MMC_CONTROL2) = 0x00000000;
    POINTVAL_(MMC_IRPT_MASK_REG) = 0xffffffff;   //set all flags
    timerWait(5000);
}

u8 mmcReadData( u8* buffer, u32 sector, u8 numberOfSectors){

    u8 sectorNmbr;
    for(sectorNmbr = 0; sectorNmbr < numberOfSectors; sectorNmbr++){
         //read block
        u32 readcount = 0;
        //READ_SINGLE_BLOCK  (CMD17)  - read block from sd card
        POINTVAL_(MMC_ARG1) = sector+sectorNmbr;
        //0x031A0000 binary is "00000011 00011010 00000000 00000000"
        //  16:17 - Type of expected response from card: 10 = 48 bits response (WITHOUT BUSY)
        //  19 - Check the responses CRC
        //  20 - Check that response has same index as command
        //  24:29 - (000011) Index of the command to be issued to the card
        POINTVAL_(MMC_CMDTM) = 0x11380010;

        timerWait(2000); //Need to wait !! Probably this should be tested for other cards

        u32 response;// = POINTVAL_(MMC_RESP0);
        //DEBUG("Response after readblock : 0x%x\r\n", response);

        while(readcount < 128){
            response = POINTVAL_(MMC_DATA_REG);
            ((u32*)buffer)[(sectorNmbr*128)+readcount] = response;
            readcount++;
        }
    }

    return 0;
}


/*
* Sends command to SD controller.
* argument - value passed to ARG1 register
* command - value passed to CMDTM register
*/
void mmcSendCommand(u32 argument, u32 command){


}

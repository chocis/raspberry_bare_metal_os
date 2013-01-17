#include "debug.h"
#include "mmc.h"
#include "timer.h"

struct Mmc_cid_data* mmc_cid_data;
const char* monthNames[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September",
                            "October", "November", "December"};

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

    //second time
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




    DEBUG("Response after SEND_CSD  shifted: 0x%x\r\n", ((response >> 16) << 16));
    u32 rcavalue = ((response >> 16) << 16);
     //second time
    //SEND_CSD ()  - Asks the card to publish a new relative address
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




    //SELECT/DESELECT_CARD  ()  - Asks the card to publish a new relative address
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


    //SET_BLOCKLEN  ()  - Asks the card to publish a new relative address
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




    //read block
    u32 readcount = 0;
    POINTVAL_(MMC_ARG1) = readcount;
    POINTVAL_(MMC_CMDTM) = 0x11380010;

    while(readcount < 10){
        response = POINTVAL_(MMC_RESP0);
        DEBUG("Response after readblock : 0x%x\r\n", response);
        timerWait(1000);
        readcount++;
    }


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

/*
* Sends command to SD controller.
* argument - value passed to ARG1 register
* command - value passed to CMDTM register
*/
void mmcSendCommand(u32 argument, u32 command){


}

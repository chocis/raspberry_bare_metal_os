#ifndef MMC_H
#define MMC_H


#define MMC_CONTROL0 0x20300028
#define MMC_CONTROL1 0x2030002c     //This register is used to configure the EMMC module. (page 74)
#define MMC_CONTROL2 0x2030003c     //This register is used to enable the different interrupts in the INTERRUPT register to generate an interrupt on the int_to_arm output.

#define MMC_IRPT_MASK_REG 0x20300034  //This register is used to mask the interrupt flags in the INTERRUPT register.
#define MMC_ARG1    0x20300008
#define MMC_CMDTM   0x2030000c      //This register is used to issue commands to the card. (page 69)

#define MMC_RESP0   0x20300010
#define MMC_RESP1   0x20300014
#define MMC_RESP2   0x20300018
#define MMC_RESP3   0x2030001c

void mmc_init(void);

#endif /* MMC_H */

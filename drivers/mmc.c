#include "mmc.h"
#include "reg.h"
#include <stdint.h>
#include "uart.h"

#define CONTROL_MODULE 0x44E10000
#define CM_PER_BASE 0x44E00000
#define MMC_BASE 0x48060000

/* pins for mmc */
/* TI manual 9.3.1 */
/* TI manual 9.3.1.50  has pin register feilds */
#define MMC_CMD (CONTROL_MODULE + 0x904)
#define MMC_CLK (CONTROL_MODULE + 0x900)
#define MMC_DAT0 (CONTROL_MODULE + 0x8FC)
#define MMC_DAT1 (CONTROL_MODULE + 0x8F8)
#define MMC_DAT2 (CONTROL_MODULE + 0x8F4)
#define MMC_DAT3 (CONTROL_MODULE + 0x8F0)

/* mmc clock control register */
/* TI manual 8.1.12.1.13 */
/* first 2 bits are for module mode */
/* set to 0x2 for enable */
#define MMC_CLKCTRL (CM_PER_BASE + 0x3C)

/* MMC Registers TI manual 18.5.1 */

/* TI manual 18.5.1.1 */
/* Set bit 1 to 1 for Soft reset */
/* Wake up config
 *
 * set bit 2 to enable wake up capability
 *
 * bit 4-3 set to 0x2
 *
 * bit  0 to 1 prob a good idea to set
 *
 * */
#define SD_SYSCONFIG (MMC_BASE + 0x110)

/* TI manual 18.5.1.2 */
/* bit 0 when set to 1 means reset is done */
#define SD_SYSSTATUS (MMC_BASE + 0x114)

/* TI manual 18.5.1.23 */
/* reading this section
 * the only thing we need to set is the voltages
 * everything else is good at default
 * need to set bits 24 25 26 to 1
 * */
#define SD_CAPA (MMC_BASE + 0x240)

/* TI manual 18.5.1.17 */
/* gonna need to set bit 24 to enabled
 * so that card wakes up on interrupt
 *
 * host bus config
 * SDBP power off first
 * set bit 8 to 0
 * select voltage
 * set bit 11-9 to 0x7 3.3v
 * DTW bit 1 to 0
 * power on
 * set bit 8 to 1
 *
 * */
#define SD_HCTL (MMC_BASE + 0x228)

/* TI manual 18.5.1.5 */
/* says to write to specfic bits to config data and command transfer
 * need to set DW8 bit 5 to 0
 * CEATA bit 12 should also be set to 0
 * */
#define SD_CON (MMC_BASE + 0x12C)

/* TI manual 18.5.1.18 */
/*
 * Enable internal clock set bit 0 to 1
 * set the clock frequency bit 15-6
 *  - clear it
 *  - need it to be less then = to 80kHz
 *  - so its refrence clock / value in bit 15-6
 *  - refrence clock = 96Mhz
 *  - we want it to be slow but not super slow for init sequence
 *  - max divisor is 1023 which gives us a freq of 93khz
 *  - bro how do we get down to 80khz when the max divisor is 1023
 *  - im just gonna start at 0x200 and go up until it works
 *
 * enable external clock by seting bit 2 to  1
 *
 *
 * */
#define SD_SYSCTL (MMC_BASE + 0x22C)

/* TI manual 18.5.1.20 */
/* Just gonna enable all interrupts */
#define SD_IE (MMC_BASE + 0x234)

/* TI manual 18.5.1.10 */
/* this register is kinda crazy, conrtols what commands we send
 * to the sd card
 *
 * bit 29-24 command
 *
 * bit 17-16 response type
 *
 * first 6 bits are for flags
 *
 * */
#define SD_CMD (MMC_BASE + 0x20C)

/* TI manual 18.5.1.19 */
/* can use this to check status of interrupts */
/* bit 0 can be read to check if a command has completed
 * set to 1 if cmd has been completed
 * */
#define SD_STAT (MMC_BASE + 0x230)

/* TI manual 18.5.1.16 */
/* can use to get status of host controller
 * bit 16 CINS can be read to see if card is inserted
 * */
#define SD_PSTATE (MMC_BASE + 0x224)

/* TI manual 18.5.1.9 */
/* uses all 32 bits for argument for sd command */
#define SD_ARG (MMC_BASE + 0x208)

/* TI manual 18.5.1.11
 * holds response for commands
 * */
#define SD_RSP10 (MMC_BASE + 0x210)

/* TI manual 18.5.1.15 */
#define SD_DATA (MMC_BASE + 0x220)

/* TI manual 18.5.1.8 */
/* sets block size for read and write */
#define SD_BLK (MMC_BASE + 0x204)

mmc_driver mmc = {
    .init = initMMCdriver,
    .read_block = MMCreadblock,
    .write_block = MMCwriteblock
};

const int MMC_CLK_ENABLE = 0x2;
const int MMC_PINMUX = 0x30;
const int MMC_SOFT_RESET = 0x2;
const int MMC_RESET_DONE = 0x1;
const int MMC_FULL_VOLTAGE_SUPPORT = (0x7 << 24);
const int MMC_WAKECONFIG = (0x1 << 2) | (0x2 << 3) | (0x2 << 12) | (0x1);
const int MMC_WAKEINT = (0x1 << 24);
const int MMC_DW8 = ~(0x1 << 5);
const int MMC_POWEROFF = ~(0x1 << 8);
const int MMC_VOLTAGE =  (0x7 << 9);
const int MMC_DATA_TRANSFER_WIDTH = ~(0x1 << 1);
const int MMC_POWERON = (0x1 << 8);
const int MMC_ENABLEINTCLK = 0x1;
const int MMC_CLEARFREQ = ~(0x3FF << 6);
const int MMC_CLKFREQ = (0x240 << 6);
const int MMC_ENABLEEXTCLK = (0x1 << 2);
const int MMC_CLKSTABLE = 0x2;
const int MMC_ENABLE_ALL_INTERRUPTS = 0xFFFFFFFF;
const int MMC_CMD_ERROR = (0x1 << 15);
const int MMC_CLEAR_STATUS = 0xFFFFFFFF;
const int CMD_OFFSET = 24;
const int RESPONSE_TYPE_OFFSET = 16;
const int MMC_CMD_STATUS = 0x1;
const int MMC_BUSY_WAIT = 0x2;
const int RESPONSE_TYPE_BUSY = 0x3;
const int MMC_INIT_BIT = (0x1 << 1);
const int MMC_INIT_CMD = 0x00000000;
const int MMC_END_INIT = ~(0x1 << 1);
const int MMC_DETECT_CARD = (0x1 << 16);
const int MMC_PATTERN = ((0x1 << 8) | (0x55));
const int MMC_POWERUP_COMPLETE = (0x1 << 31);

extern void buddy();

typedef struct cmd {
    uint32_t cmd;
    uint32_t response_type;
    uint32_t arg;
    uint32_t flags;
} CMD;  

CMD CMD0 = {
    .cmd = 0x00,
    .response_type = 0x00,
    .arg = 0x00,
    .flags = 0x00
};

/* response length 48 bits */

    /* arg 
     *
     * bit 8-11 is voltage
     * 0x1 for 2.7 - 3.6v
     *
     * bit 0 - 7 is a check pattern 0x55
     * */

CMD CMD8 = {
    .cmd = 0x8,
    .response_type = 0x2,
    .arg = (0x1 << 8) | (0x55),
    .flags = 0x00
};

CMD CMD55 = {
    .cmd = 55,
    .response_type = 3,
    .arg = 0,
    .flags = 0
};

CMD ACMD41 = {
    .cmd = 41,
    .response_type = 3,
    .arg = (0x1 << 30) | (0x3F << 15),
    .flags = 0
};

CMD CMD2 = {
    .cmd = 2,
    .response_type = 1,
    .arg = 0,
    .flags = 0
};

CMD CMD3 = {
    .cmd = 3,
    .response_type = 3,
    .arg = 0,
    .flags = 0
};

CMD CMD9 = {
    .cmd = 9,
    .response_type = 1,
    .arg = 0,
    .flags = 0
};

CMD CMD7 = {
    .cmd = 7,
    .response_type = 3,
    .arg = 0,
    .flags = 0
};

CMD CMD24 = {
    .cmd = 24,
    .response_type = 2,
    .arg = 0,
    .flags = (0x1 << 20) | (0x1 << 19) | (0x1 << 21)
};

CMD CMD17 = {
    .cmd = 17,
    .response_type = 2,
    .arg = 0,
    .flags = (0x1 << 21) | (0x1 << 4)
};

void initMMCCLK(){

    uint32_t reg;

    reg = READ32(MMC_CLKCTRL);

    /* set first 2 bits to 0x2 to enable */
    reg |= MMC_CLK_ENABLE;

    WRITE32(MMC_CLKCTRL,reg);

}

void pinmuxMMC(){

    /* 9.3.1.50 has info on the register feilds 
     * for the pins used by MMC*/

    /* For each pin we will set the following,
     * honestly no clue what these mean or do so we will 
     * set all the cool things to enabled*/

    /*
     * bit 3 to 0 pullup/pulldown enabled
     * bit 4 to 1 pullup selected
     * bit 5 to 1 receiver enabled
     * bit 6 to 0 Fast
     *
     * 00110000 = 0x30
     * 
     */

    WRITE32(MMC_CMD, MMC_PINMUX);
    WRITE32(MMC_CLK, MMC_PINMUX);
    WRITE32(MMC_DAT0, MMC_PINMUX);
    WRITE32(MMC_DAT1, MMC_PINMUX);
    WRITE32(MMC_DAT2, MMC_PINMUX);
    WRITE32(MMC_DAT3, MMC_PINMUX);


}

void softresetMMC(){

    uint32_t reg;

    reg = READ32(SD_SYSCONFIG);

    /* set bit 1 to 1 for soft reset */
    reg |= MMC_SOFT_RESET;

    WRITE32(SD_SYSCONFIG, reg);

    /* bit 0 set to 1 means reset is done */
    while (!(READ32(SD_SYSSTATUS) & MMC_RESET_DONE)) {}

}

void sethardwareCAP(){

    uint32_t reg;

    reg = READ32(SD_CAPA);

    /*
     * need to set support for all voltages
     * bits 24 25 26
     *
     * 111 = 0x7 need to shift it 24 
     * */

    reg |= MMC_FULL_VOLTAGE_SUPPORT;

    WRITE32(SD_CAPA, reg);

}

void setwakeMMC(){

    uint32_t reg;

    reg = READ32(SD_SYSCONFIG);

    /* set MMC wake up config */
    reg |= MMC_WAKECONFIG;

    WRITE32(SD_SYSCONFIG, reg);

    reg = READ32(SD_HCTL);

    /* set bit 24 to 1 to enable wake interrupt events*/
    reg |= MMC_WAKEINT;

    WRITE32(SD_HCTL, reg);
}

void mmcCONFIG(){

    uint32_t reg;	
	
    /* set bits 5 DW8 and 12 CEATA to 0 in SD_CON */
    
    reg = READ32(SD_CON);

    reg &= MMC_DW8; 

    WRITE32(SD_CON, reg);

    /* config sd bus */

    /* power off sd bus while config*/
    /* set bit 8  to 0*/
    reg = READ32(SD_HCTL);

    reg &= MMC_POWEROFF;

    WRITE32(SD_HCTL,reg);

    /* Select voltage 3.3v */
    /* set bit  11-9 to 0x7 */
    reg = READ32(SD_HCTL);

    reg |= MMC_VOLTAGE;

    WRITE32(SD_HCTL, reg);

    /* Set data transfer width to 1 bit */
    /* set bit 1 to 0 */
    reg = READ32(SD_HCTL);

    reg &= MMC_DATA_TRANSFER_WIDTH; 

    WRITE32(SD_HCTL, reg);

    /* Power on sd bus*/
    /* set bit 8 to 1 */
    reg = READ32(SD_HCTL);

    reg |= MMC_POWERON; 

    WRITE32(SD_HCTL, reg);

    /* wait for it to power on */
    while (!(READ32(SD_HCTL) & MMC_POWERON)){}


}

void configMMCCLK(){

    uint32_t reg;

    /* enable internal clock */
    /* set bit 0 to 1 */
    reg =  READ32(SD_SYSCTL);
    
    reg |= MMC_ENABLEINTCLK;

    WRITE32(SD_SYSCTL, reg);

    /*set init clock frequnecy */
    /* 96Mhz / 512 */

    /* clear existing bits */
    reg = READ32(SD_SYSCTL);

    reg &= MMC_CLEARFREQ; 

    /* set divisor to 512 */
    reg |= MMC_CLKFREQ; 

    WRITE32(SD_SYSCTL, reg);

    /* enable external clock */
    reg = READ32(SD_SYSCTL);

    reg |= MMC_ENABLEEXTCLK; 

    WRITE32(SD_SYSCTL, reg);

    /* wait for clock to stablize */
    while (!(READ32(SD_SYSCTL) & MMC_CLKSTABLE )){}

}

/* TI manual 18.4.2 */
void initMMC(){

    /* init clock */
    initMMCCLK();

    /* pin muxing */
    pinmuxMMC();

    /* software reset */
    softresetMMC();

    /* set modules hardware capabilites */
    sethardwareCAP();

    /* Set module idle and wake up modes */
    setwakeMMC();

    /* TI manual 18.4.2.5 */

    /* MMC host and bus config */
    mmcCONFIG();

    /* config internal clock for init sequence */
    configMMCCLK();

    /* Enable all interrupts im not sure if we even have to do this
     * but will do it incase we want to use interrupts*/
    WRITE32(SD_IE, MMC_ENABLE_ALL_INTERRUPTS);
}

/*
 * https://users.ece.utexas.edu/~valvano/EE345M/SD_Physical_Layer_Spec.pdf
 *
 * has all the commands
 *
 * send command to MMC
 * */
int mmcCMD(CMD command){

    uint32_t cmd = command.cmd;
    uint32_t response = command.response_type;
    uint32_t arg = command.arg; 
    uint32_t flags = command.flags;

    uint32_t reg;

    /* set arg */	
    WRITE32(SD_ARG,arg);

    /* run command */

    reg = 0;

    reg |= (cmd << CMD_OFFSET);

    reg |= (response << RESPONSE_TYPE_OFFSET);

    reg |= flags;

    WRITE32(SD_CMD,reg);

    /* wait for command to finish*/
    while (!(READ32(SD_STAT))){}

    /* Check for Error */
    /* bit 15 of SD_STATUS indicates if an error has occured */
    if (READ32(SD_STAT) & MMC_CMD_ERROR){
    
        uart0_printf("MMC CMD Error \n");

        /* Clear status */
	WRITE32(SD_STAT, MMC_CLEAR_STATUS);

	return 0;
    }

    /* if the response type is a busy wait 0x3, we need to wait */
    if (response == RESPONSE_TYPE_BUSY){
        while (!(READ32(SD_STAT) & MMC_BUSY_WAIT)){}
	WRITE32(SD_STAT,MMC_BUSY_WAIT);
    }

    /* clear command status */
    WRITE32(SD_STAT, MMC_CMD_STATUS);

    return 1;
}

void initsequence(){

    uint32_t reg;

    /* set init bit to 1, bit 1 */
    reg = READ32(SD_CON);

    reg |= MMC_INIT_BIT; 

    WRITE32(SD_CON, reg);

    /* write to SD_CMD register */
    WRITE32(SD_CMD, MMC_INIT_CMD);

    /* wait for comand to finish need to wait 1ms */
    /* low key one cycle of the buddy function might be enough*/

    buddy();
 
    while(!(READ32(SD_STAT) & MMC_CMD_STATUS)){}

    /* clear sd status */

    WRITE32(SD_STAT, (READ32(SD_STAT) | MMC_CMD_STATUS) );

    /* end init sequence */
    reg = READ32(SD_CON);

    reg &= MMC_END_INIT; 

    WRITE32(SD_CON, reg);

    WRITE32(SD_STAT,MMC_CLEAR_STATUS);
}

/* TI manual 18.4.3.2  part 1 */
int detectSDcard(){

    /* complete init sequence */
    initsequence();

    /* change clock frequency to fit protocol */

    /* detect return 0 if no card is detected */
    if(!(READ32(SD_PSTATE) & MMC_DETECT_CARD)) return 0;

    return 1;
}

/* TI manual 18.4.3.2  pre node A */
int idCard(){

    /* send cmd 0 */	

    mmcCMD(CMD0);

    /* skiping the SDIO check */

    /* checking if card is compliant with standard 2.0 */

    /* send command 8 */ 

    if(!mmcCMD(CMD8)){
    
        uart0_printf("CMD 8 failed \n");
	return 0;
    }

    /* check if response returns the same pattern */
    if(READ32(SD_RSP10) != MMC_PATTERN){
    
      uart0_printf("Card is not compliant \n");
      return 0;
    }

    return 1;

}

/* TI manual 18.4.3.2 part 2, bro this just looks horible */
void poweronSD(){

    /* since our card is compliant we need to keep looping until powered on*/

    while(1){
    
        /* send cmd 55*/

	mmcCMD(CMD55);

	/* send ACMD41 */

	/* arg set bit 30 OCR and set voltage window */

        mmcCMD(ACMD41);

	if (READ32(SD_RSP10) & MMC_POWERUP_COMPLETE) break;
    
    }

    uart0_printf("SD card powered on \n");

}

/* TI manual 18.4.3.2 after node B*/
void selectSD(){

    uint32_t sd_addr;

    /* we need to find the addr of the sd card and select it as the one we are using*/

    /* send cmd 2 */

    mmcCMD(CMD2);

    /* send cmd 3 */
    mmcCMD(CMD3);

    /* read addr from response */

    sd_addr = READ32(SD_RSP10) >> 16;

    CMD9.arg = (sd_addr << 16);

    CMD7.arg = (sd_addr << 16);	    

    /* send cmd 9 */
    mmcCMD(CMD9);

    /* send cmd 7 */
    mmcCMD(CMD7);

    uart0_printf("SD card %d selected \n",sd_addr);

}

void initMMCdriver(){

    initMMC();

    uart0_printf("MMC controler initalized \n");

    if (detectSDcard()) uart0_putsln("SD card Detected");
    
    if (idCard()) uart0_printf("SD card is compliant with standard 2.0 \n");

    poweronSD();

    selectSD();

}

/* DO NOT TOUCH THESE 2 FUNCTIONS */

/* THEY ARE VERY UNSTABLE TO CHANGE */

/* EVEN REPlACING NUMBERS WITH CONSTANTS BREAKS IT*/

/* what block to write using LBA and a 512 byte buffer*/
void MMCwriteblock(uint32_t block, volatile uint32_t* buf){

    int i;

    uart0_printf(""); // again random print fixes the bug

    /* set block size to 512*/
    WRITE32(SD_BLK, 0x200);

    CMD24.arg = block;

    /* Write CMD*/
    mmcCMD(CMD24);

    /* Wait for write to complete */
    uart0_printf(""); // dont remove this it will break something

    while(1){
    
        if (READ32(SD_PSTATE) & (0x1 << 10)){

	   for (i = 0; i < (512/4); i++){
	        WRITE32(SD_DATA,*buf);
		buf++;
	    }
	}
	if (READ32(SD_STAT) & (0x1 << 1)) {
	
	  WRITE32(SD_STAT,READ32(SD_STAT) | (0x1 << 4) | (0x1 << 1));
	  break;
	}
    
    }

}

/* what block to read from and a 512 byte buffer*/
void MMCreadblock(uint32_t block, volatile uint32_t* buf){

    int i;

    /* set block size to 512*/
    WRITE32(SD_BLK, 0x200);

    CMD17.arg = block;

    /* read CMD*/
    mmcCMD(CMD17);

    /* Wait for read to complete */

    while(1){

        if (READ32(SD_STAT) & (0x1 << 5)){

           for (i = 0; i < (512/4); i++){
                *buf = READ32(SD_DATA);
                buf++;
            }
        }
        if (READ32(SD_STAT) & (0x1 << 1)) {

          WRITE32(SD_STAT,READ32(SD_STAT) | (0x1 << 5) | (0x1 << 1));
          break;
        }

    }

}


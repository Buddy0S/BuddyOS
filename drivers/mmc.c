#include "mmc.h"
#include "reg.h"
#include <stdint.h>
#include "uart.h"

extern void buddy();
/* check header file for TI manual sections on each register */

void initMMCCLK(){

    uint32_t reg;

    reg = READ32(MMC_CLKCTRL);

    /* set first 2 bits to 0x2 to enable */
    reg |= 0x2;

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

    WRITE32(MMC_CMD, 0x30);
    WRITE32(MMC_CLK, 0x30);
    WRITE32(MMC_DAT0, 0x30);
    WRITE32(MMC_DAT1, 0x30);
    WRITE32(MMC_DAT2, 0x30);
    WRITE32(MMC_DAT3, 0x30);


}

void softresetMMC(){

    uint32_t reg;

    reg = READ32(SD_SYSCONFIG);

    /* set bit 1 to 1 for soft reset */
    reg |= 0x2;

    WRITE32(SD_SYSCONFIG, reg);

    /* bit 0 set to 1 means reset is done */
    while (!(READ32(SD_SYSSTATUS) & 0x1)) {}

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

    reg |= (0x7 << 24);

    WRITE32(SD_CAPA, reg);

}

void setwakeMMC(){

    uint32_t reg;

    reg = READ32(SD_SYSCONFIG);

    /* set ENAWAKEUP bit */
    reg |= (0x1 << 2);

    /* set bit 4-3 to 0x2 */
    reg |= (0x2 << 3);

    /* set bit 0 to 1 */
    reg |= 0x1;

    reg |= (0x2 << 12);

    WRITE32(SD_SYSCONFIG, reg);

    reg = READ32(SD_HCTL);

    /* set bit 24 to 1 to enable wake interrupt events*/
    reg |= (0x1 << 24);

    WRITE32(SD_HCTL, reg);
}

void mmcCONFIG(){

    uint32_t reg;	
	
    /* set bits 5 DW8 and 12 CEATA to 0 in SD_CON */
    
    reg = READ32(SD_CON);

    reg &= ~(0x1 << 5);
    //reg &= ~(0x1 << 12);

    WRITE32(SD_CON, reg);

    /* config sd bus */

    /* power off sd bus while config*/
    /* set bit 8  to 0*/
    reg = READ32(SD_HCTL);

    reg &= ~(0x1 << 8);

    WRITE32(SD_HCTL,reg);

    /* Select voltage 3.3v */
    /* set bit  11-9 to 0x7 */
    reg = READ32(SD_HCTL);

    reg |= (0x7 << 9);

    WRITE32(SD_HCTL, reg);

    /* Set data transfer width to 1 bit */
    /* set bit 1 to 0 */
    reg = READ32(SD_HCTL);

    reg &= ~(0x1 << 1);

    WRITE32(SD_HCTL, reg);

    /* Power on sd bus*/
    /* set bit 8 to 1 */
    reg = READ32(SD_HCTL);

    reg |= (0x1 << 8);

    WRITE32(SD_HCTL, reg);

    /* wait for it to power on */
    while (!(READ32(SD_HCTL) & (0x1 << 8))){}


}

void configMMCCLK(){

    uint32_t reg;

    /* enable internal clock */
    /* set bit 0 to 1 */
    reg =  READ32(SD_SYSCTL);
    
    reg |= 0x1;

    WRITE32(SD_SYSCTL, reg);

    /*set init clock frequnecy */
    /* 96Mhz / 512 */

    /* clear existing bits */
    reg = READ32(SD_SYSCTL);

    reg &= ~(0x3FF << 6);

    /* set divisor to 512 */
    reg |= (0x240 << 6);

    WRITE32(SD_SYSCTL, reg);

    /* enable external clock */
    reg = READ32(SD_SYSCTL);

    reg |= (0x1 << 2);

    WRITE32(SD_SYSCTL, reg);

    /* wait for clock to stablize */
    while (!(READ32(SD_SYSCTL) & 0x2)){}

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
    WRITE32(SD_IE, 0xFFFFFFFF);
}

void initsequence(){

    uint32_t reg;

    /* set init bit to 1, bit 1 */
    reg = READ32(SD_CON);

    reg |= (0x1 << 1);

    WRITE32(SD_CON, reg);

    /* write to SD_CMD register */
    WRITE32(SD_CMD, 0x00000000);

    /* wait for comand to finish need to wait 1ms */
    /* low key one cycle of the buddy function might be enough*/

    //uart0_putsln("TESTING");

    buddy();

    //uart0_putsln("TESTING");
 
    while(!(READ32(SD_STAT) & 0x1)){}

    /* clear sd status */

    WRITE32(SD_STAT, (READ32(SD_STAT) | 0x1) );

    //uart0_putsln("TESTING");

    /* end init sequence */
    reg = READ32(SD_CON);

    reg &= ~(0x1 << 1);

    WRITE32(SD_CON, reg);

    //`uart0_putsln("EXPLAIN PLEASE");

    WRITE32(SD_STAT,0xFFFFFFFF);
}

/* TI manual 18.4.3.2  part 1 */
int detectSDcard(){

    /* complete init sequence */
    initsequence();

    /* change clock frequency to fit protocol */

    /* detect return 0 if no card is detected */
    if(!(READ32(SD_PSTATE) & (0x1 << 16))) return 0;

    return 1;
}

/*
 * https://users.ece.utexas.edu/~valvano/EE345M/SD_Physical_Layer_Spec.pdf
 *
 * has all the commands
 * */
int mmcCMD(uint32_t cmd, uint32_t response, uint32_t arg, uint32_t flags){

    uint32_t reg;

    /* set arg */	
    WRITE32(SD_ARG,arg);

    /* run command */
    reg = 0x00000000;

    reg |= (cmd << 24);

    reg |= (response << 16);

    reg |= flags;

    WRITE32(SD_CMD,reg);

    /* wait for command to finish*/
    while (!(READ32(SD_STAT))){}

    /* Check for Error */
    /* bit 15 of SD_STATUS indicates if an error has occured */
    if (READ32(SD_STAT) & (0x1 << 15)){
    
        uart0_printf("mmc CMD Error \n");

        /* Clear status */
	WRITE32(SD_STAT, 0xFFFFFFFF);

	return 0;
    }

    /* if the response type is a busy wait 0x3, we need to wait */
    if (response == 0x3){
        while (!(READ32(SD_STAT) & 0x2)){}
	WRITE32(SD_STAT,0x2);
    }

    /* clear command status */
    WRITE32(SD_STAT, 0x1);

    return 1;
}

/* TI manual 18.4.3.2  pre node A */
int idCard(){

    uint32_t cmd, response_type, arg, flags;

    /* send cmd 0 */	

    cmd = 0x00;

    response_type = 0x00;

    arg = 0x00;

    flags = 0x00;

    mmcCMD(cmd,response_type,arg,flags);

    /* skiping the SDIO check */

    /* checking if card is compliant with standard 2.0 */

    /* send command 8 */
    cmd = 0x8;

    /* response length 48 bits */
    response_type = 0x2;

    /* arg 
     *
     * bit 8-11 is voltage
     * 0x1 for 2.7 - 3.6v
     *
     * bit 0 - 7 is a check pattern 0x55
     * */

    arg = (0x1 << 8) | (0x55);

    flags = 0x00;

    if(!mmcCMD(cmd,response_type,arg,flags)){
    
        uart0_printf("CMD 8 failed \n");
	return 0;
    }


    /* check if response returns the same pattern */
    if(READ32(SD_RSP10) != ((0x1 << 8) | (0x55))){
    
      uart0_printf("Card is not compliant \n");
      return 0;
    }

    return 1;

}

/* TI manual 18.4.3.2 part 2, bro this just looks horible */
void poweronSD(){

    uint32_t arg;

    /* since our card is compliant we need to keep looping until powered on*/

    while(1){
    
        /* send cmd 55*/

	mmcCMD(55,3,0,0);

	/* send ACMD41 */

	/* arg set bit 30 OCR and set voltage window */

        arg = (0x1 << 30) | (0x3F << 15);

        mmcCMD(41,3,arg,0);

	if (READ32(SD_RSP10) & (0x1 << 31)) break;
    
    }

    uart0_printf("SD card powered on \n");

}

/* TI manual 18.4.3.2 after node B*/
void selectSD(){

    uint32_t sd_addr;

    /* we need to find the addr of the sd card and select it as the one we are using*/

    /* send cmd 2 */

    mmcCMD(2,1,0,0);

    /* send cmd 3 */
    mmcCMD(3,3,0,0);

    /* read addr from response */

    sd_addr = READ32(SD_RSP10) >> 16;

    /* send cmd 9 */
    mmcCMD(9,1,(sd_addr << 16),0);

    /* send cmd 7 */
    mmcCMD(7,3,(sd_addr << 16),0);

    uart0_printf("SD card selected at %x \n",sd_addr);

}

void initMMCdriver(){

    initMMC();

    int sd_card = detectSDcard();

    if (sd_card) uart0_putsln("SD card Detected");
    
    int sd_card_stat =  idCard();

    if (sd_card_stat) uart0_printf("SD card is compliant with standard 2.0 \n");

    poweronSD();

    selectSD();

}


/* what block to write using LBA and a 512 byte buffer*/
void MMCwriteblock(uint32_t block, uint32_t* buf){

    int i;

    /* set block size to 512*/
    WRITE32(SD_BLK, 0x200);

    /* Write CMD*/
    mmcCMD(24,2,block,(0x1 << 20) | (0x1 << 19) | (0x1 << 21) );

    /* Wait for write to complete */
    uart0_printf(""); // dont remove this it will break something

    while(1){
    
        if (READ32(SD_PSTATE) & (0x1 << 10)){

	   //uart0_printf("TESTING\n");	
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
void MMCreadblock(uint32_t block, uint32_t* buf){

    int i;

    /* set block size to 512*/
    WRITE32(SD_BLK, 0x200);

    /* read CMD*/
    mmcCMD(17,2,block, (0x1 << 21) | (0x1 << 4) );

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


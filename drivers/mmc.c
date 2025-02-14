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

/* TI manual 18.4.3.2 */
int detectSDcard(){

    /* complete init sequence */
    initsequence();

    /* change clock frequency to fit protocol */

    /* detect return 0 if no card is detected */
    if(!(READ32(SD_PSTATE) & (0x1 << 16))) return 0;

    return 1;
}

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

    /* clear command status */
    WRITE32(SD_STAT, 0x1);

    return 1;
}

/* TI manual 18.4.3.2 */
void idCard(){

    /* send cmd 0 */	
    mmcCMD(0x00,0x00,0x00,0x00);

}


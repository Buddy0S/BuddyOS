#include "mmc.h"
#include "reg.h"
#include <stdint.h>

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

    WRITE32(SD_SYSCONFIG, reg);

    reg = READ32(SD_HCTL);

    /* set bit 24 to 1 to enable wake interrupt events*/
    reg |= (0x1 << 24);

    WRITE32(SD_HCTL, reg);
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

    /* MMC host and bus config TI manual 18.4.2.5*/

}

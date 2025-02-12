#include "mmc.h"

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

/* TI manual 18.4.2 */
void initMMC(){

    /* init clock */
    InitMMCCLK();

    /* pin muxing */
    pinMUXMMC();

    /* software reset */

    /* set modules hardware capabilites */

    /* Set module idle and wake up modes */

    /* MMC host and bus config TI manual 18.4.2.5*/

}

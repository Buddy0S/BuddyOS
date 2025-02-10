#include "clock.h"
#include "reg.h"

#define CM_BASE 0x44E10000
#define VTP_CTRL 0xE0C

#define CMD0_SLAVE_RATIO 0x201C
#define CMD0_INVERT_CLKOUT 0x202C

#define CMD1_SLAVE_RATIO 0x2050
#define CMD1_INVERT_CLKOUT 0x2060

#define CMD2_SLAVE_RATIO 0x2084
#define CMD2_INVERT_CLKOUT 0x2094

#define DATA0_RD_DQS_SLAVE_RATIO 0x20C8
#define DATA0_WR_DQS_SLAVE_RATIO 0x20DC
#define DATA0_WE_FIFO_SLAVE_RATIO 0x2108
#define DATA0_WR_DATA_SLAVE_RATIO 0x2120

#define DATA1_RD_DQS_SLAVE_RATIO 0x216C
#define DATA1_WR_DQS_SLAVE_RATIO 0x2180
#define DATA1_WE_FIFO_SLAVE_RATIO 0x21AC
#define DATA1_WR_DATA_SLAVE_RATIO 0x21C4

#define CMD0_IOCTRL 0x1404
#define CMD1_IOCTRL 0x1408
#define CMD2_IOCTRL 0x140C

#define DATA0_IOCTRL 0x1440
#define DATA1_IOCTRL 0x1444

#define DDR_IOCTRL 0xE04
#define DDR_CKE_CTRL 0x131C

#define EMIF_BASE 0x4C000000
#define EMIF_DDR_PHY_CTRL_1 0xE4
#define EMIF_DDR_PHY_CTRL_1_SHDW 0xE8

#define EMIF_SDRAM_TIM_1 0x18
#define EMIF_SDRAM_TIM_1_SHDW 0x1C

#define EMIF_SDRAM_TIM_2 0x20
#define EMIF_SDRAM_TIM_2_SHDW 0x24

#define EMIF_SDRAM_TIM_3 0x28
#define EMIF_SDRAM_TIM_3_SHDW 0x2C

#define EMIF_SDRAM_REF_CTRL 0x10
#define EMIF_SDRAM_REF_CTRL_SHDW 0x14

#define EMIF_SDRAM_CONFIG 0x8
#define EMIF_SDRAM_ZQ_CONFIG 0xC8

#define EMIF_SECURE_SDRAM_CONFIG 0x110

/* sets up the physical configuration of the ddr ram */
static void init_ddr_phys(void) {

    /* 7.3.3.3.4 
     *
     * enable vtp for dynamic evaluation of ram performance */
    /* WHAT THE FUCK IS VTP MARWAN */

    /* enable VTP */
    WRITE32(CM_BASE + VTP_CTRL, READ32(CM_BASE + VTP_CTRL) | (0x40));

    /* these two writes will clear the VTP register */
    WRITE32(CM_BASE + VTP_CTRL, READ32(CM_BASE + VTP_CTRL) & ~(0x1));
    WRITE32(CM_BASE + VTP_CTRL, READ32(CM_BASE + VTP_CTRL) | 0x1);

    /* wait until VTP ready */
    while((READ32(CM_BASE + VTP_CTRL) & 0x20) != 0x20);

    /* set up slave ratios 7.3.6.1 */
    /* https://e2e.ti.com/support/processors-group/processors/f/processors-forum/757943/am3352-calibration-of-ddr3-phy-fifo-we-slave-ratio-aka-gate-leveling */
    /* these ratios seem to all be board dependent, so the beaglebone black
     * most likely has a specific value needed for all of these in order to 
     * sync up memory operations with the actual core clock */
    /* value / 0x100 */


    /* 50% slave ratio, value taken from TI starterware */
    WRITE32(CM_BASE + CMD0_SLAVE_RATIO, 0x80);
    /* disable inverted clock signal, value taken from TI starterware */
    WRITE32(CM_BASE + CMD0_INVERT_CLKOUT, 0x0);

    WRITE32(CM_BASE + CMD1_SLAVE_RATIO, 0x80);
    WRITE32(CM_BASE + CMD1_INVERT_CLKOUT, 0x0);

    WRITE32(CM_BASE + CMD2_SLAVE_RATIO, 0x80);
    WRITE32(CM_BASE + CMD2_INVERT_CLKOUT, 0x0);

    /* configure data macro registers */
    /* these are all slave ratios */
    WRITE32(CM_BASE + DATA0_RD_DQS_SLAVE_RATIO, 0x38);
    WRITE32(CM_BASE + DATA0_WR_DQS_SLAVE_RATIO, 0x44);
    WRITE32(CM_BASE + DATA0_WE_FIFO_SLAVE_RATIO, 0x94);
    WRITE32(CM_BASE + DATA0_WR_DATA_SLAVE_RATIO, 0x7D);
    WRITE32(CM_BASE + DATA1_RD_DQS_SLAVE_RATIO, 0x38);
    WRITE32(CM_BASE + DATA1_WR_DQS_SLAVE_RATIO, 0x44);
    WRITE32(CM_BASE + DATA1_WE_FIFO_SLAVE_RATIO, 0x94);
    WRITE32(CM_BASE + DATA1_WR_DATA_SLAVE_RATIO, 0x7D);

    /* there is some nuts tool we can try to use from TI to get
     * optimal slave ratios but i'm not sure we can actually use 
     * it with our beaglebone */

}

static void init_emif(void) {
    volatile unsigned int regVal;

    /* set clk control mode */
    regVal = READ32(CM_EMIF_FW_CLKCTRL) & ~(0x00000003);
    regVal |= 0x2;
    WRITE32(CM_EMIF_FW_CLKCTRL, regVal);

    regVal = READ32(CM_EMIF_CLKCTRL) & ~(0x00000003);
    regVal |= 0x2;
    WRITE32(CM_EMIF_CLKCTRL, regVal);

    while ((READ32(CM_PER_L3_CLKCTRL) & (0x14)) != 0x14);



    
}

void init_ddr(void) {

    /* set up slew rates options */
    WRITE32(CM_BASE + CMD0_IOCTRL, 0x18B);
    WRITE32(CM_BASE + CMD1_IOCTRL, 0x18B);
    WRITE32(CM_BASE + CMD2_IOCTRL, 0x18B);
    WRITE32(CM_BASE + DATA0_IOCTRL, 0x18B);
    WRITE32(CM_BASE + DATA1_IOCTRL, 0x18B);

    /* set mode select bit to ddr3 */
    WRITE32(CM_BASE + DDR_IOCTRL, READ32(CM_BASE + DDR_IOCTRL) & 0xEFFFFFFF);
    /* enable clock on ddr */
    WRITE32(CM_BASE + DDR_CKE_CTRL, READ32(CM_BASE + DDR_CKE_CTRL) | 0x1);

    /* set the read latency on in terms of ddr clock cycles - 1 */
    /* setting it to 7 here */
    /* cannot find a single reputable source for cas latency for the beaglebone
     * so this is assuming cas latency >= 5 */
    WRITE32(EMIF_BASE + EMIF_DDR_PHY_CTRL_1, 0x06);
    /* write to shadow register ??? */
    WRITE32(EMIF_BASE + EMIF_DDR_PHY_CTRL_1_SHDW, 0x06);

    /* cannot find this register in the manual but starterware writes to it
    WRITE32(EMIF_BASE + EMIF_DDR_PHY_CTRL_1 + 0x4, 0x06);
    */

    /* genuinely how would we ever figure out these numbers */
    WRITE32(EMIF_BASE + EMIF_SDRAM_TIM_1, 0x0AAAD4DB);
    WRITE32(EMIF_BASE + EMIF_SDRAM_TIM_1_SHDW, 0x0AAAD4DB);

    WRITE32(EMIF_BASE + EMIF_SDRAM_TIM_2, 0x266B7FDA);
    WRITE32(EMIF_BASE + EMIF_SDRAM_TIM_2_SHDW, 0x266B7FDA);

    WRITE32(EMIF_BASE + EMIF_SDRAM_TIM_3, 0x501F867F);
    WRITE32(EMIF_BASE + EMIF_SDRAM_TIM_3_SHDW, 0x501F867F);

    WRITE32(EMIF_BASE + EMIF_SDRAM_REF_CTRL, 0x00000C30);
    WRITE32(EMIF_BASE + EMIF_SDRAM_REF_CTRL_SHDW, 0x00000C30);


    /*
     ** termination = 1 (RZQ/4)
     ** dynamic ODT = 2 (RZQ/2)
     ** SDRAM drive = 0 (RZQ/6)
     ** CWL = 0 (CAS write latency = 5)
     ** CL = 2 (CAS latency = 5)
     ** ROWSIZE = 7 (16 row bits)
     ** PAGESIZE = 2 (10 column bits)
     */
    WRITE32(EMIF_BASE + EMIF_SDRAM_CONFIG, 0x61C04BB2);
    WRITE32(EMIF_BASE + EMIF_SDRAM_ZQ_CONFIG, 0x50074BE4);
    /* this write exports config to the emif controller */
    WRITE32(CM_BASE + EMIF_SECURE_SDRAM_CONFIG, 0x61C04BB2);


}

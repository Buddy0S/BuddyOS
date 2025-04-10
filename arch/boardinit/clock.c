/* CMPT432 - ImplementationTeam00 */

#include "clock.h"
#include "reg.h"
#include <stdint.h>

#define CORE_FREQ 500
#define MPU_FREQ 1000
#define PER_FREQ 960
#define DDR_FREQ 400

void pll_to_bypass(uint32_t clckmode, uint32_t idlest){

    uint32_t reg;    

    /* Switch PLL to bypass mode */
    reg = READ32(clckmode);

    /* clear first three bits */
    reg &= ~0x7;

    /* set to bypass mode */
    reg |= 0x4;

    WRITE32(clckmode, reg);

    /* wait for PLL to be in bypass*/

    while (!(READ32(idlest) & 0x100)){}

}

void pll_lock(uint32_t clckmode, uint32_t idlest){

    uint32_t reg;

    /* Lock PLL */
    reg = READ32(clckmode);

    reg |= 0x7;

    WRITE32(clckmode, reg);

    /* wait for PLL to be locked */

    while (!(READ32(idlest) & 0x1)){}

}

void set_clock_frequency(uint32_t clcksel, int multi, int div){

    /* 
     * TI manual 8.1.6.3 explains what these PLLs are
     * esential using an input freq we can generate a custom
     * output freq with only one input freq/clock
     * 
     * CLKOUT = (M / (N+1))*CLKINP*(1/M2)
     *
     * m is multi
     * n is div
     * - these 2 are set in CM_CLCKSEL_DPLL
     * m2 is set in m2 divider
     * CLKINP is the refrence input clock
     *
     *The standard external clock frequency source for the BeagleBone Black is 24 MHz. 
     * 
     * so we can set the freq of a PLL 2 anyhting in the multi
     * as long as it is under locking max
     *
     * (24*(multi/23+1)/M2)
     * */

    uint32_t freq = (multi << 8) | (div);	
    WRITE32(clcksel,freq);
}

void set_divider(uint32_t divider, uint32_t set, uint32_t clear){

    uint32_t reg;

    reg = READ32(divider);

    reg &= ~clear;
    reg |= set;

    WRITE32(divider,reg);
}

/* TI manual 8.1.6.7.1 */
void CORE_PLL_INIT(){

    /* Switch to bypass mode*/
    pll_to_bypass(CM_CLKMODE_DPLL_CORE, CM_IDLEST_DPLL_CORE);

    /* TI manual 8.1.6.7 for example frequencys and M2 value*/
    /* it says locking freq is 2000 */
    /* want working so i just kept trying values till one worked*/

    /* Configure Multiply and divide */
    set_clock_frequency(CM_CLKSEL_DPLL_CORE, CORE_FREQ, 23);

    /* Set dividers */

    /* Ratios for the dividers found in 8.1.6.7 Table 8-22*/
    set_divider(CM_DIV_M4_DPLL_CORE,10,0x1F);

    set_divider(CM_DIV_M5_DPLL_CORE,8,0x1F);

    set_divider(CM_DIV_M6_DPLL_CORE,4,0x1F);

    /* Lock PLL*/
    pll_lock(CM_CLKMODE_DPLL_CORE, CM_IDLEST_DPLL_CORE);
}

/* TI manual 8.1.6.9.1 */
void MPU_PLL_INIT(){

    /* Switch to bypass mode*/
    pll_to_bypass(CM_CLKMODE_DPLL_MPU, CM_IDLEST_DPLL_MPU);

    /* TI manual 8.1.6.11 for example frequencys and M2 value*/
    /* it says locking freq is 1200 */
    /* our cpu only ranges from 300 - 1000*/

    /* Configure Multiply and divide */
    set_clock_frequency(CM_CLKSEL_DPLL_MPU, MPU_FREQ, 23);

    /* Set dividers */
    set_divider(CM_DIV_M2_DPLL_MPU,1,0x1F);

    /* Lock PLL*/
    pll_lock(CM_CLKMODE_DPLL_MPU, CM_IDLEST_DPLL_MPU);
}

/* TI manual 8.1.6.8.1 */
void PER_PLL_INIT(){

    /* Switch to bypass mode*/
    pll_to_bypass(CM_CLKMODE_DPLL_PER, CM_IDLEST_DPLL_PER);

    /* TI manual 8.1.6.8 for example frequencys and M2 value*/
    /* Max freq is 960 */

    /* Configure Multiply and divide */
    set_clock_frequency(CM_CLKSEL_DPLL_PER, PER_FREQ , 23);

    /* Set dividers */
    /* want it to equal 192 */
    set_divider(CM_DIV_M2_DPLL_PER,5,0x7F);

    /* Lock PLL*/
    pll_lock(CM_CLKMODE_DPLL_PER, CM_IDLEST_DPLL_PER);
}

/* TI manual 8.1.6.11.1 */
void DDR_PLL_INIT(){

    /* Switch to bypass mode*/
    pll_to_bypass(CM_CLKMODE_DPLL_DDR, CM_IDLEST_DPLL_DDR);

    /* TI manual 8.1.6.11 for example frequencys and M2 value*/
    /* Max freq is 532 */
    /* it says that 532 is max but Table 5-5 on the datasheet 
     * says that 400 mhz is the max for ddr3 ram*/

    /* Configure Multiply and divide */
    set_clock_frequency(CM_CLKSEL_DPLL_DDR, DDR_FREQ , 23);

    /* Set dividers */
    set_divider(CM_DIV_M2_DPLL_DDR,1,0x1F);

    /* Lock PLL*/
    pll_lock(CM_CLKMODE_DPLL_DDR, CM_IDLEST_DPLL_DDR);
}

void Enable_clocks(){

  WRITE32(CM_WKUP_CONTROL_CLKCTRL,0x2);
  WRITE32(CM_PER_L4LS_CLKCTRL,0x2);
  WRITE32(CM_PER_L3_CLKKCTRL,0x2);
  WRITE32(CM_WKUP_CLKSTCTRL,0x2);
  WRITE32(CM_PER_L4LS_CLKSTCTRL,0x2);
  WRITE32(CM_PER_L3_CLKSTCTRL,0x2);
}

void initClocks(){

    CORE_PLL_INIT();	
    MPU_PLL_INIT();
    PER_PLL_INIT();
    DDR_PLL_INIT();

    Enable_clocks();
}

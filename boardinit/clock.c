#include "clock.h"
#include "reg.h"
#include <stdint.h>

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

    /* 24MHz * multi / div + 1 */

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

    /* Configure Multiply and divide */
    set_clock_frequency(CM_CLKSEL_DPLL_CORE, 500, 23);

    /* Set dividers */
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

    /* Configure Multiply and divide */
    set_clock_frequency(CM_CLKSEL_DPLL_MPU, 1000, 23);

    /* Set dividers */
    set_divider(CM_DIV_M2_DPLL_MPU,1,0x1F);

    /* Lock PLL*/
    pll_lock(CM_CLKMODE_DPLL_MPU, CM_IDLEST_DPLL_MPU);
}

/* TI manual 8.1.6.8.1 */
void PER_PLL_INIT(){

    /* Switch to bypass mode*/
    pll_to_bypass(CM_CLKMODE_DPLL_PER, CM_IDLEST_DPLL_PER);

    /* Configure Multiply and divide */
    set_clock_frequency(CM_CLKSEL_DPLL_PER, 960, 23);

    /* Set dividers */
    set_divider(CM_DIV_M2_DPLL_PER,5,0x7F);

    /* Lock PLL*/
    pll_lock(CM_CLKMODE_DPLL_PER, CM_IDLEST_DPLL_PER);
}

/* TI manual 8.1.6.11.1 */
void DDR_PLL_INIT(){

    /* Switch to bypass mode*/
    pll_to_bypass(CM_CLKMODE_DPLL_DDR, CM_IDLEST_DPLL_DDR);

    /* Configure Multiply and divide */
    set_clock_frequency(CM_CLKSEL_DPLL_DDR, 400, 23);

    /* Set dividers */
    set_divider(CM_DIV_M2_DPLL_DDR,1,0x1F);

    /* Lock PLL*/
    pll_lock(CM_CLKMODE_DPLL_DDR, CM_IDLEST_DPLL_DDR);
}

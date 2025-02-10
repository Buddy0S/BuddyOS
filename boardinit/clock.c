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

/* TI manual 8.1.6.7.1 */
void CORE_PLL_INIT(){

    /* Switch to bypass mode*/
    pll_to_bypass(CM_CLKMODE_DPLL_CORE, CM_IDLEST_DPLL_CORE);

    /* Lock PLL*/
    pll_lock(CM_CLKMODE_DPLL_CORE, CM_IDLEST_DPLL_CORE);
   
}

/* TI manual 8.1.6.9.1 */
void MPU_PLL_INIT(){

    /* Switch to bypass mode*/
    pll_to_bypass(CM_CLKMODE_DPLL_MPU, CM_IDLEST_DPLL_MPU);

    /* Lock PLL*/
    pll_lock(CM_CLKMODE_DPLL_MPU, CM_IDLEST_DPLL_MPU);
}

/* TI manual 8.1.6.8.1 */
void PER_PLL_INIT(){

    /* Switch to bypass mode*/
    pll_to_bypass(CM_CLKMODE_DPLL_PER, CM_IDLEST_DPLL_PER);

    /* Lock PLL*/
    pll_lock(CM_CLKMODE_DPLL_PER, CM_IDLEST_DPLL_PER);
}

/* TI manual 8.1.6.11.1 */
void DDR_PLL_INIT(){

    /* Switch to bypass mode*/
    pll_to_bypass(CM_CLKMODE_DPLL_DDR, CM_IDLEST_DPLL_DDR);

    /* Lock PLL*/
    pll_lock(CM_CLKMODE_DPLL_DDR, CM_IDLEST_DPLL_DDR);
}

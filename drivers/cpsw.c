#include <stdint.h>

/*
 * cpsw.c
 *
 * This file contains the driver code for the BeagleBone Black
 * AM335x Sitara™ Processors Common Platform Switch
 *
 * Initializes Ethernet Sub System:
 *  - GMII/MII Interface
 *  - Normal Priorty Mode
 *  - VLAN Unaware
 *  - Single MAC
 *  - Statistics Disabled
 *  - RX and TX DMA Completion Interrupts
 *  - Channel 0 for both RX and TX
 *  - Ports in Forward State
 *
 * Author: Marwan Mostafa 
 *
 * */

/* ----------------------DATA ACCESS MACROS----------------------- */

//*******************************************************************
// Register Access Macro
//*******************************************************************

#define REG(x) (*((volatile uint32_t *)(x)))

//*******************************************************************
// Byte Extraction Macros
//*******************************************************************

#define BYTE0(x) x & 0xFF
#define BYTE1(x) (x & 0xFF00) >> 8
#define BYTE2(x) (x & 0xFF0000) >> 16
#define BYTE3(x) (x & 0xFF000000) >> 24

//******************************************************************* 
// Bit Macro      
//*******************************************************************

#define BIT(x) 0x00000001 << x

/* ---------------------------REGISTERS--------------------------- */

//*******************************************************************
// CONTROL_MODULE REGISTERS
//*******************************************************************

#define CM_BASE 0x44E10000

#define GMII_SEL (CM_BASE + 0x650)

#define MAC_ID0_LO (CM_BASE + 0x630)
#define MAC_ID0_HI (CM_BASE + 0x634)

/* CPSW Pins */
#define CONF_MII1_COL (CM_BASE + 0x908)
#define CONF_MII1_CRS (CM_BASE + 0x90C)
#define CONF_MII1_RX_ER (CM_BASE + 0x910)
#define CONF_MII1_TX_EN (CM_BASE + 0x914)
#define CONF_MII1_RX_DV (CM_BASE + 0x918)
#define CONF_MII1_TXD3 (CM_BASE + 0x91C)
#define CONF_MII1_TXD2 (CM_BASE + 0x920)
#define CONF_MII1_TXD1 (CM_BASE + 0x924)
#define CONF_MII1_TXD0 (CM_BASE + 0x928)
#define CONF_MII1_TX_CLK (CM_BASE + 0x92C)
#define CONF_MII1_RX_CLK (CM_BASE + 0x930)
#define CONF_MII1_RXD3 (CM_BASE + 0x934)
#define CONF_MII1_RXD2 (CM_BASE + 0x938)
#define CONF_MII1_RXD1 (CM_BASE + 0x93C)
#define CONF_MII1_RXD0 (CM_BASE + 0x940)
#define CONF_MII1_REF_CLK (CM_BASE + 0x944)
#define CONF_MDIO (CM_BASE + 0x948)
#define CONF_MDC (CM_BASE + 0x94C)

//*******************************************************************
// CLOCK_MODULE REGISTERS
//*******************************************************************

#define CM_PER_BASE 0x44E00000

/* CPSW Clock registers*/
#define CM_PER_CPGMAC0_CLKCTRL (CM_PER_BASE + 0x14)
#define CM_PER_CPSW_CLKSTCTRL (CM_PER_BASE + 0x144)

//*******************************************************************
// CPDMA REGISTERS
//*******************************************************************

#define CPDMA_BASE 0x4A100800

#define CPDMA_SOFT_REST (CPDMA_BASE + 0x1C)

#define TX_CONTROL (CPDMA_BASE + 0x4)
#define RX_CONTROL (CPDMA_BASE + 0x14)

//*******************************************************************
// CPSW_SL REGISTERS
//*******************************************************************

#define PORT1_BASE 0x4A100D80
#define PORT2_BASE 0x4A100DC0

#define PORT1_SOFT_REST (PORT1_BASE + 0xC)
#define PORT2_SOFT_REST (PORT2_BASE + 0xC)

//*******************************************************************
// CPSW_SS REGISTERS
//*******************************************************************

#define CPSW_SS_BASE 0x4A100000

#define CPSW_SS_SOFT_REST (CPSW_SS_BASE + 0x8)
#define STATE_PORT_EN (CPSW_SS_BASE + 0xC)

//*******************************************************************
// CPSW_WR REGISTERS
//*******************************************************************

#define CPSW_WR_BASE 0x4A101200

#define CPSW_WR_SOFT_REST (CPSW_WR_BASE + 0x4)

//*******************************************************************
// CPDMA_STATERAM REGISTERS
//*******************************************************************

#define CPDMA_STATERAM_BASE 0x4A100A00

/* TX Channels HDPs */
#define TX0_HDP (CPDMA_STATERAM_BASE + 0x0)
#define TX1_HDP (CPDMA_STATERAM_BASE + 0x4)
#define TX2_HDP (CPDMA_STATERAM_BASE + 0x8)
#define TX3_HDP (CPDMA_STATERAM_BASE + 0xC)
#define TX4_HDP (CPDMA_STATERAM_BASE + 0x10)
#define TX5_HDP (CPDMA_STATERAM_BASE + 0x14)
#define TX6_HDP (CPDMA_STATERAM_BASE + 0x18)
#define TX7_HDP (CPDMA_STATERAM_BASE + 0x1C)

/* RX Channels HDPs */
#define RX0_HDP (CPDMA_STATERAM_BASE + 0x20)
#define RX1_HDP (CPDMA_STATERAM_BASE + 0x24)
#define RX2_HDP (CPDMA_STATERAM_BASE + 0x28)
#define RX3_HDP (CPDMA_STATERAM_BASE + 0x2C)
#define RX4_HDP (CPDMA_STATERAM_BASE + 0x30)
#define RX5_HDP (CPDMA_STATERAM_BASE + 0x34)
#define RX6_HDP (CPDMA_STATERAM_BASE + 0x38)
#define RX7_HDP (CPDMA_STATERAM_BASE + 0x3C)

/* TX Channels CPs */
#define TX0_CP (CPDMA_STATERAM_BASE + 0x40)
#define TX1_CP (CPDMA_STATERAM_BASE + 0x44)
#define TX2_CP (CPDMA_STATERAM_BASE + 0x48)
#define TX3_CP (CPDMA_STATERAM_BASE + 0x4C)
#define TX4_CP (CPDMA_STATERAM_BASE + 0x50)
#define TX5_CP (CPDMA_STATERAM_BASE + 0x54)
#define TX6_CP (CPDMA_STATERAM_BASE + 0x58)
#define TX7_CP (CPDMA_STATERAM_BASE + 0x5C)

/* RX Channels CPs */
#define RX0_CP (CPDMA_STATERAM_BASE + 0x60)
#define RX1_CP (CPDMA_STATERAM_BASE + 0x64)
#define RX2_CP (CPDMA_STATERAM_BASE + 0x68)
#define RX3_CP (CPDMA_STATERAM_BASE + 0x6C)
#define RX4_CP (CPDMA_STATERAM_BASE + 0x70)
#define RX5_CP (CPDMA_STATERAM_BASE + 0x74)
#define RX6_CP (CPDMA_STATERAM_BASE + 0x78)
#define RX7_CP (CPDMA_STATERAM_BASE + 0x7C)

//*******************************************************************
// MDIO REGISTERS
//*******************************************************************

#define MDIO_BASE 0x4A101000

#define MDIOCONTROL (MDIO_BASE + 0x4)
#define MDIOALIVE (MDIO_BASE + 0x8)

//*******************************************************************
// CPSW_ALE REGISTERS
//*******************************************************************

#define CPSW_ALE_BASE 0x4A100D00

#define CPSW_ALE_CONTROL (CPSW_ALE_BASE + 0x8)
#define TBLCTL (CPSW_ALE_BASE + 0x20)
#define TBLW2 (CPSW_ALE_BASE + 0x34)
#define TBLW1 (CPSW_ALE_BASE + 0x38)
#define TBLW0 (CPSW_ALE_BASE + 0x3C)

#define PORTCTL0 (CPSW_ALE_BASE + 0x40)
#define PORTCTL1 (CPSW_ALE_BASE + 0x44)
#define PORTCTL2 (CPSW_ALE_BASE + 0x48)

//*******************************************************************
// CPSW_PORT REGISTERS
//*******************************************************************

#define CPSW_PORT_BASE 0x4A100100

/* Port 1 */
#define P1_CONTROL (CPSW_PORT_BASE + 0x100)
#define P1_SA_LO (CPSW_PORT_BASE + 0x120)
#define P1_SA_HI (CPSW_PORT_BASE + 0x124)

/* Port 2 */
#define P2_CONTROL (CPSW_PORT_BASE + 0x200)
#define P2_SA_LO (CPSW_PORT_BASE + 0x220)
#define P2_SA_HI (CPSW_PORT_BASE + 0x224)

/* ------------------------REGISTER VALUES------------------------- */

//*******************************************************************
// Interface Selection
//*******************************************************************

#define GMII_MII_SELECT 0x0

//*******************************************************************
// Pin Muxing 
//*******************************************************************

#define DEFAULT_PIN 0x0
#define RECEIVE_ENABLE BIT(5)
#define PULLUP_ENABLE BIT(4)

//*******************************************************************
// Clocks                                                        
//*******************************************************************

#define CPGMAC0_ENABLE BIT(1)
#define CPGMAC0_NOTREADY (BIT(16) | BIT(17))

#define CPSWCLK_ENABLE BIT(1)
#define CPSWCLK_READY BIT(4)

//*******************************************************************
// Software Reset                                                     
//*******************************************************************

#define START_RESET BIT(0)
#define RESET_NOTDONE BIT(0)

//*******************************************************************
// DMA                                                            
//*******************************************************************

#define DESCRIPTOR_NULL 0x00000000

//*******************************************************************
// ALE                                                               
//*******************************************************************

#define ENABLE_ALE BIT(31)
#define CLEAR_ALE BIT(30)

/* -----------------------------CODE------------------------------- */

/*
 * cpsw_select_interface()
 *  - selects gmii/mii interface for cpsw
 *  - sets ports 1 and 2 to use selected interface
 *  
 * */
void cpsw_select_interface(){
    
    REG(GMII_SEL) = GMII_MII_SELECT;
}

/*
 * cpsw_pin_mux()
 *  - pin muxing the cpsw pins
 *  - setting pins to default of 0
 *  - setting enable recieve on pins that recieve Input
 *  - setting enable pullup on mdio pins
 *
 * */
void cpsw_pin_mux(){

    REG(CONF_MII1_COL) = DEFAULT_PIN | RECEIVE_ENABLE;
    REG(CONF_MII1_CRS) = DEFAULT_PIN | RECEIVE_ENABLE;
    REG(CONF_MII1_RX_ER) = DEFAULT_PIN | RECEIVE_ENABLE;
    REG(CONF_MII1_TX_EN) = DEFAULT_PIN;
    REG(CONF_MII1_RX_DV) = DEFAULT_PIN | RECEIVE_ENABLE;
    REG(CONF_MII1_TXD3) = DEFAULT_PIN;
    REG(CONF_MII1_TXD2) = DEFAULT_PIN;
    REG(CONF_MII1_TXD1) = DEFAULT_PIN;
    REG(CONF_MII1_TXD0) = DEFAULT_PIN;
    REG(CONF_MII1_TX_CLK) = DEFAULT_PIN | RECEIVE_ENABLE;
    REG(CONF_MII1_RX_CLK) = DEFAULT_PIN | RECEIVE_ENABLE;
    REG(CONF_MII1_RXD3) = DEFAULT_PIN | RECEIVE_ENABLE;
    REG(CONF_MII1_RXD2) = DEFAULT_PIN | RECEIVE_ENABLE;
    REG(CONF_MII1_RXD1) = DEFAULT_PIN | RECEIVE_ENABLE;
    REG(CONF_MII1_RXD0) = DEFAULT_PIN | RECEIVE_ENABLE;
    REG(CONF_MDIO) = DEFAULT_PIN | RECEIVE_ENABLE | PULLUP_ENABLE;
    REG(CONF_MDC) = DEFAULT_PIN | PULLUP_ENABLE;

}

/*
 * cpsw_enable_clocks()
 *  - enables and configures the cpsw clocks
 *  - waits for clocks to be ready
 *
 * */
void cpsw_enable_clocks(){

    REG(CM_PER_CPGMAC0_CLKCTRL) |= CPGMAC0_ENABLE;

    while ( REG(CM_PER_CPGMAC0_CLKCTRL) & CPGMAC0_NOTREADY ){}

    REG(CM_PER_CPSW_CLKSTCTRL) |= CPSWCLK_ENABLE;

    while ( !(REG(CM_PER_CPSW_CLKSTCTRL) & CPSWCLK_READY) ){}
}

/*
 * software_reset(uint32_t module_address)
 *  - resets a module given its reset register address
 *
 *  param: module_address
 *   - Software Reset Register Address
 *
 * */
void software_reset(uint32_t module_address){

    REG(module_address) |= START_RESET;

    while ( REG(module_address) & RESET_NOTDONE ){}
}

/*
 * cpsw_software_reset()
 *  - software resets the modules of cpsw
 *
 * */
void cpsw_software_reset(){

    software_reset(PORT1_SOFT_RESET);
    software_reset(PORT1_SOFT_RESET);
    software_reset(CPSW_SS_SOFT_RESET);
    software_reset(CPSW_WR_SOFT_RESET);
    software_reset(CPDMA_SOFT_RESET);    
}

/*
 * cpsw_init_cpdma_descriptors()
 *  - init cpdma descriptors to NULL after reset
 *
 * */
void cpsw_init_cpdma_descriptors(){

    REG(TX0_HDP) = DESCRIPTOR_NULL;
    REG(TX1_HDP) = DESCRIPTOR_NULL;
    REG(TX2_HDP) = DESCRIPTOR_NULL;
    REG(TX3_HDP) = DESCRIPTOR_NULL;
    REG(TX4_HDP) = DESCRIPTOR_NULL;
    REG(TX5_HDP) = DESCRIPTOR_NULL;
    REG(TX6_HDP) = DESCRIPTOR_NULL;
    REG(TX7_HDP) = DESCRIPTOR_NULL;

    REG(RX0_HDP) = DESCRIPTOR_NULL;
    REG(RX1_HDP) = DESCRIPTOR_NULL;
    REG(RX2_HDP) = DESCRIPTOR_NULL;
    REG(RX3_HDP) = DESCRIPTOR_NULL;
    REG(RX4_HDP) = DESCRIPTOR_NULL;
    REG(RX5_HDP) = DESCRIPTOR_NULL;
    REG(RX6_HDP) = DESCRIPTOR_NULL;
    REG(RX7_HDP) = DESCRIPTOR_NULL;

    REG(TX0_CP) = DESCRIPTOR_NULL;
    REG(TX1_CP) = DESCRIPTOR_NULL;
    REG(TX2_CP) = DESCRIPTOR_NULL;
    REG(TX3_CP) = DESCRIPTOR_NULL;
    REG(TX4_CP) = DESCRIPTOR_NULL;
    REG(TX5_CP) = DESCRIPTOR_NULL;
    REG(TX6_CP) = DESCRIPTOR_NULL;
    REG(TX7_CP) = DESCRIPTOR_NULL;

    REG(RX0_CP) = DESCRIPTOR_NULL;
    REG(RX1_CP) = DESCRIPTOR_NULL;
    REG(RX2_CP) = DESCRIPTOR_NULL;
    REG(RX3_CP) = DESCRIPTOR_NULL;
    REG(RX4_CP) = DESCRIPTOR_NULL;
    REG(RX5_CP) = DESCRIPTOR_NULL;
    REG(RX6_CP) = DESCRIPTOR_NULL;
    REG(RX7_CP) = DESCRIPTOR_NULL;
    
}

/*
 * cpsw_config_ale()
 *  - clears ALE table
 *  - enables ALE
 *
 * */
void cpsw_config_ale(){
    
    REG(CPSW_ALE_CONTROL) = ENABLE_ALE | CLEAR_ALE;
}

/*
 * cpsw_init()
 *  - initializes the Ethernet subsystem for the BeagleBone Black
 *  - Follow Steps outlined in Ti Manual Section 14.4.6
 *
 * */
void cpsw_init(){

    cpsw_select_interface();

    cpsw_pin_mux();

    cpsw_enable_clocks();

    cpsw_software_reset();

    cpsw_init_cpdma_descriptors();

    cpsw_config_ale();
}

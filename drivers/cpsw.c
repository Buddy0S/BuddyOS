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

//*******************************************************************
// CPSW_WR REGISTERS
//*******************************************************************

#define CPSW_WR_BASE 0x4A101200

#define CPSW_WR_SOFT_REST (CPSW_WR_BASE + 0x4)

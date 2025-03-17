#include <stdint.h>
#include "memory.h"
#include "uart.h"

/*
 * net.c
 *
 * This file contains the driver code to use Network capabilities of
 * the BeagleBone Black
 * 
 * - AM335x Sitara™ Processors Common Platform Switch Driver
 * - Microchip LAN8710A PHY Driver
 * 
 * Author: Marwan Mostafa 
 *
 * */

extern void buddy();

/* ----------------------DATA ACCESS MACROS----------------------- */

//*******************************************************************
// Register Access Macro
//*******************************************************************

#define REG(x) (*((volatile uint32_t *)(x)))

//*******************************************************************
// Byte Extraction Macros
//*******************************************************************

#define BYTE0(x) (((x) & 0xFF))
#define BYTE1(x) ((((x) & 0xFF00)) >> 8)
#define BYTE2(x) ((((x) & 0xFF0000)) >> 16)
#define BYTE3(x) ((((x) & 0xFF000000)) >> 24)

//******************************************************************* 
// Bit Macro      
//*******************************************************************

#define BIT(x) (1 << (x))

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

#define CPDMA_SOFT_RESET (CPDMA_BASE + 0x1C)

#define TX_CONTROL (CPDMA_BASE + 0x4)
#define RX_CONTROL (CPDMA_BASE + 0x14)

#define DMASTATUS (CPDMA_BASE + 0x24)

//*******************************************************************
// CPSW_SL REGISTERS
//*******************************************************************

#define PORT1_BASE 0x4A100D80
#define PORT2_BASE 0x4A100DC0

#define PORT1_SOFT_RESET (PORT1_BASE + 0xC)
#define PORT2_SOFT_RESET (PORT2_BASE + 0xC)

#define PORT1_MACCONTROL (PORT1_BASE + 0x4)

//*******************************************************************
// CPSW_SS REGISTERS
//*******************************************************************

#define CPSW_SS_BASE 0x4A100000

#define CPSW_SS_SOFT_RESET (CPSW_SS_BASE + 0x8)
#define STAT_PORT_EN (CPSW_SS_BASE + 0xC)

//*******************************************************************
// CPSW_WR REGISTERS
//*******************************************************************

#define CPSW_WR_BASE 0x4A101200

#define CPSW_WR_SOFT_RESET (CPSW_WR_BASE + 0x4)

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

#define MDIO_CONTROL (MDIO_BASE + 0x4)
#define MDIOALIVE (MDIO_BASE + 0x8)
#define MDIOLINK (MDIO_BASE + 0xC)

#define MDIOUSERACCESS0 (MDIO_BASE + 0x80)

//*******************************************************************
// PHY REGISTERS
//*******************************************************************

#define PHY_BCR 0
#define PHY_BSR 1
#define PHY_AUTONEG_ADV 4
#define PHY_PARTNER_CAP 5

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

//*******************************************************************
// INTERRUPT REGISTERS
//*******************************************************************

#define INTERRUPTC_BASE 0x48200000
#define INTC_MIR_CLEAR1 (INTERRUPTC_BASE + 0xA8)

#define TX_INTMASK_SET (CPDMA_BASE + 0x88)
#define RX_INTMASK_SET (CPDMA_BASE + 0xA8)

#define C0_RX_EN (CPSW_WR_BASE + 0x14)
#define C0_TX_EN (CPSW_WR_BASE + 0x18)

#define CPDMA_EOI_VECTOR (CPDMA_BASE + 0x94)

//*******************************************************************
// GPIO REGISTERS
//*******************************************************************

#define GPIO1_BASE      0x4804C000
#define GPIO_OE         (GPIO1_BASE + 0x134)
#define GPIO_DATAOUT    (GPIO1_BASE + 0x13C)

/* ------------------------REGISTER VALUES------------------------- */

//*******************************************************************
// Interface Selection
//*******************************************************************

#define GMII_MII_SELECT 0x0
#define GMII_ENABLE BIT(5)
#define OH_MBPS (BIT(16) | BIT(15))

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

#define TX_INIT_FLAGS 0x0
#define RX_INIT_FLAGS BIT(29)

#define CPDMA_ENABLE BIT(0)

//*******************************************************************
// ALE                                                               
//*******************************************************************

#define MAX_ALE_ENTRIES 1024
#define ALE_ENTRY_WORDS 3

#define ENABLE_ALE BIT(31)
#define CLEAR_ALE BIT(30)

#define ALE_ENTRY_TYPE (BIT(5) | BIT(4))
#define ALE_ENTRY_FREE 0
#define ALE_PORT_MASK_SHIFT 2

#define MULTICAST_PORTMASK (BIT(0) | BIT(1) | BIT(2))
#define ALE_MULTICAST_ENTRY (BIT(7) | BIT(6) | BIT(4))

#define UNICAST_PORTMASK 0
#define ALE_UNICAST_ENTRY BIT(4)

#define TBLCTL_WRITE_READ BIT(31)

//*******************************************************************
// MDIO                                                               
//*******************************************************************

#define MDIO_ENABLE BIT(30)
#define MDIO_CLKDIV 124
#define MDIO_PREAMBLE BIT(20)
#define MDIO_FAULTENB BIT(18)
#define MDIO_CLKDIV_MASK 0xFFFF

//*******************************************************************
// STATS                                                              
//*******************************************************************

#define STATS_DISABLE 0x00000000

//*******************************************************************
// Ports                                                             
//*******************************************************************

#define PORT_FORWARD (BIT(0) | BIT(1))

//*******************************************************************
// ETH                                                             
//*******************************************************************

#define MAC_ADDR_LEN 6
#define MAX_PACKET_SIZE 1520

//*******************************************************************
// CPPI
//*******************************************************************

#define CPPI_RAM 0x4A102000
#define CPPI_SIZE 0x2000

#define NUM_DESCRIPTORS 50

//*******************************************************************
// INTERRUPTS
//*******************************************************************

#define CPSW_INTMASK_CLEAR (BIT(10) | BIT(9))

#define CPDMA_CHANNEL_INT BIT(0)

#define EOI_TX BIT(1)
#define EOI_RX BIT(0)

//*******************************************************************
// PHY
//*******************************************************************

#define GO_BIT BIT(31)
#define PHY_READ 0x00000000
#define PHY_WRITE BIT(30)
#define PHY_ADDR_SHIFT 16
#define REG_ADDR_SHIFT 21
#define READ_ACK BIT(29)
#define PHY_DATA 0xFFFF

#define PHY1 0
      
#define PHY_RESET_BIT BIT(8)    

#define PHY100 BIT(7)
#define PHY100_FD BIT(8)

#define PHY10 BIT(5)
#define PHY10_FD BIT(6)

#define AUTO_NEG_ENABLE BIT(12)
#define AUTO_NEG_CLEAR (BIT(5) | BIT(6) | BIT(7) | BIT(8))
#define AUTO_NEG_RESTART BIT(9)
#define AUTO_NEG_COMPLETE BIT(5)

#define FULL_DUPLEX BIT(0)
#define HALF_DUPLEX 0x0
#define IN_BAND BIT(18)
#define CLEAR_TRANSFER (BIT(7) | BIT(0))

/* ----------------------------STRUCTS----------------------------- */

/* CPDMA header discriptors */
typedef struct hdp {

    volatile struct hdp* next_descriptor;
    volatile uint32_t* buffer_pointer;
    volatile uint32_t buffer_length;
    volatile uint32_t flags;

} cpdma_hdp;

/* CPDMA Channels */
typedef struct cpdma_channel {

    cpdma_hdp* head;
    cpdma_hdp* tail;
    cpdma_hdp* free;
    int num_descriptors;

} cpdma_ch;

/* Ethernet Interface */
typedef struct cpsw_interface {

    uint8_t mac_addr[MAC_ADDR_LEN];
    cpdma_ch txch;
    cpdma_ch rxch;

} ethernet_interface;

ethernet_interface eth_interface;

/* --------------------------CPSW CODE----------------------------- */

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
    
    REG(CPSW_ALE_CONTROL) = ENABLE_ALE | CLEAR_ALE | BIT(4) | BIT(8);
}

/*
 * cpsw_config_mdio()
 *  - enables mdio
 *  - disables preamble
 *  - enables fault detection
 *  - sets clk div
 *
 * */
void cpsw_config_mdio(){
   
    REG(MDIO_CONTROL) = MDIO_ENABLE | MDIO_PREAMBLE | MDIO_FAULTENB | (MDIO_CLKDIV & MDIO_CLKDIV_MASK);
}

/*
 * cpsw_config_stats()
 *  - disables stats
 *
 * */
void cpsw_config_stats(){
    
    REG(STAT_PORT_EN) = STATS_DISABLE;
}

/*
 * cpsw_set_ports_state()
 *  - sets the 3 ports state to forward
 *
 * */
void cpsw_set_ports_state(){

    REG(PORTCTL0) |= PORT_FORWARD;
    REG(PORTCTL1) |= PORT_FORWARD;
    REG(PORTCTL2) |= PORT_FORWARD;
}

/*
 * get_ale_entry()
 *  - reads ale entry at index into entry buffer
 *
 *  param: index
 *   - index in ale table
 *
 *  param: entrybuffer
 *   - pointer to buffer that will store ale entry
 *
 * */
void get_ale_entry(uint32_t index, uint32_t* entrybuffer){
	
    REG(TBLCTL) = index;

    //REG(TBLCTL) &= ~TBLCTL_WRITE_READ;

    entrybuffer[0] = REG(TBLW0);
    entrybuffer[1] = REG(TBLW1);
    entrybuffer[2] = REG(TBLW2);

}

/*
 * get_ale_index()
 *  - returns index of first free ale entry
 *
 * */
int get_ale_index(){

    uint32_t ale_entry_buffer[ALE_ENTRY_WORDS];
    int index;
    uint8_t et;

    for (index = 0; index < MAX_ALE_ENTRIES; index++){
   
        uart0_printf("index %d\n",index);       

	get_ale_entry(index, ale_entry_buffer);

	et = (((uint8_t*)(&(ale_entry_buffer[1])))[3] & (uint8_t) ALE_ENTRY_TYPE);

	uart0_printf("ET: %d\n",et);

	if ( et == ALE_ENTRY_FREE) return index;	
    }

    return -1;

}

/*
 * ale_set_entry()
 *  - writes the ale entry to the given index
 *
 *  param: e_wn
 *   - each individual word of the entry
 *
 *  param: i
 *   - index to write in ale table
 *
 * */
void ale_set_entry(uint32_t e_w0, uint32_t e_w1, uint32_t e_w2,int i){

    uart0_printf("writing words\n");	
    REG(TBLW0) = e_w0;
    REG(TBLW1) = e_w1;
    REG(TBLW2) = e_w2;

    REG(TBLCTL) = i | TBLCTL_WRITE_READ;
    
}

/*
 * multicast_ale_entry()
 *  - creates multicast ale entry 
 *
 *  param: portmask
 *   - value to be writen to port mask in word 2
 *
 *  param: multicast mac addr
 *
 * */
void multicast_ale_entry(uint32_t portmask, uint8_t* mac_addr){

    int index = get_ale_index();

    uint32_t ale_entry_w0 = 0x0;
    uint32_t ale_entry_w1 = 0x0;
    uint32_t ale_entry_w2 = 0x0;

    ((uint8_t*) &ale_entry_w0)[0] = mac_addr[MAC_ADDR_LEN - 1];
    ((uint8_t*) &ale_entry_w0)[1] = mac_addr[MAC_ADDR_LEN - 2];
    ((uint8_t*) &ale_entry_w0)[2] = mac_addr[MAC_ADDR_LEN - 3];
    ((uint8_t*) &ale_entry_w0)[3] = mac_addr[MAC_ADDR_LEN - 4];

    ((uint8_t*) &ale_entry_w1)[0] = mac_addr[MAC_ADDR_LEN - 5];
    ((uint8_t*) &ale_entry_w1)[1] = mac_addr[MAC_ADDR_LEN - 6];

    ((uint8_t*) &ale_entry_w1)[3] = (uint8_t) ALE_MULTICAST_ENTRY;

    ale_entry_w2 = portmask << ALE_PORT_MASK_SHIFT; 

    ale_set_entry(ale_entry_w0, ale_entry_w1, ale_entry_w2, index);
}

/*
 *  multicast_ale_entry()
 *  - creates unicast ale entry
 *
 *  param: portmask
 *   - value to be writen to port mask in word 2
 *
 *  param: unicast mac addr
 * */
void unicast_ale_entry(uint32_t portmask, uint8_t* mac_addr){

    int index = get_ale_index();

    uint32_t ale_entry_w0 = 0x0;
    uint32_t ale_entry_w1 = 0x0;
    uint32_t ale_entry_w2 = 0x0;

    ((uint8_t*) &ale_entry_w0)[0] = mac_addr[MAC_ADDR_LEN - 1];
    ((uint8_t*) &ale_entry_w0)[1] = mac_addr[MAC_ADDR_LEN - 2];
    ((uint8_t*) &ale_entry_w0)[2] = mac_addr[MAC_ADDR_LEN - 3];
    ((uint8_t*) &ale_entry_w0)[3] = mac_addr[MAC_ADDR_LEN - 4];

    ((uint8_t*) &ale_entry_w1)[0] = mac_addr[MAC_ADDR_LEN - 5];
    ((uint8_t*) &ale_entry_w1)[1] = mac_addr[MAC_ADDR_LEN - 6];

    ((uint8_t*) &ale_entry_w1)[3] = (uint8_t) ALE_UNICAST_ENTRY;

    ale_entry_w2 = portmask << ALE_PORT_MASK_SHIFT;

    ale_set_entry(ale_entry_w0, ale_entry_w1, ale_entry_w2, index);
}

/*
 * get_mac()
 *  - reads mac address from registers and stores in interface
 *  - mac address is stored in reverse order
 *
 * */
void get_mac(){

    eth_interface.mac_addr[0] = BYTE1(REG(MAC_ID0_LO));
    eth_interface.mac_addr[1] = BYTE0(REG(MAC_ID0_LO));
    eth_interface.mac_addr[2] = BYTE3(REG(MAC_ID0_HI));
    eth_interface.mac_addr[3] = BYTE2(REG(MAC_ID0_HI));
    eth_interface.mac_addr[4] = BYTE1(REG(MAC_ID0_HI));
    eth_interface.mac_addr[5] = BYTE0(REG(MAC_ID0_HI));

}

/*
 * cpsw_create_ale_entries()
 *  - creates ale entries for ports
 *  - multicast
 *  - unicast
 *
 * */
void cpsw_create_ale_entries(){

    uint8_t mc_addr[MAC_ADDR_LEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    get_mac();

    multicast_ale_entry(MULTICAST_PORTMASK, mc_addr);
    uart0_printf("multicast entry created\n");

    unicast_ale_entry(UNICAST_PORTMASK, eth_interface.mac_addr);
    uart0_printf("unicast entry created\n");
}

/*
 * cpsw_set_port_addrs()
 *  - sets the addrs for port 1 and 2 as the mac address
 *
 * */
void cpsw_set_port_addrs(){

   uint32_t mac_hi = 0;
   uint32_t mac_low = 0;

   get_mac();

   ((uint8_t*)&mac_hi)[0] = eth_interface.mac_addr[0];
   ((uint8_t*)&mac_hi)[1] = eth_interface.mac_addr[1];
   ((uint8_t*)&mac_hi)[2] = eth_interface.mac_addr[2];
   ((uint8_t*)&mac_hi)[3] = eth_interface.mac_addr[3];

   ((uint8_t*)&mac_low)[0] = eth_interface.mac_addr[4];
   ((uint8_t*)&mac_low)[1] = eth_interface.mac_addr[5];

   REG(P1_SA_HI) = mac_hi;
   REG(P1_SA_LO) |= mac_low;

   REG(P2_SA_HI) = mac_hi;
   REG(P2_SA_LO) |= mac_low;

}

/*
 * cpsw_setup_cpdma_descriptors()
 *  - sets up cpdma descriptors in CPPI state ram
 *  - half of state ram for tx other half for rx
 *  - allocates buffers for rx channel
 *  - tx buffers will be allocated on send
 *
 * */
void cpsw_setup_cpdma_descriptors(){

    cpdma_hdp* tx_start;
    cpdma_hdp* rx_start;
    int num_descriptors;
    cpdma_hdp* tx_cur;
    cpdma_hdp* rx_cur;

    tx_start = (cpdma_hdp*) CPPI_RAM;
    rx_start = (cpdma_hdp*) (CPPI_RAM + (NUM_DESCRIPTORS * sizeof(cpdma_hdp)));

    uart0_printf("TEST1 %x | %x ", (NUM_DESCRIPTORS * sizeof(cpdma_hdp)),rx_start );

    num_descriptors = NUM_DESCRIPTORS;

    tx_cur = tx_start;
    rx_cur = rx_start;

    /* Create Descriptor Chains */
    for (int i = 0; i < num_descriptors - 1; i++){
    
        /* TX */

	/* Set Next Descriptor */    
        tx_cur = (cpdma_hdp*)((uint32_t) tx_cur + i * sizeof(cpdma_hdp));
	tx_cur->next_descriptor = (cpdma_hdp*)((uint32_t) tx_cur + (i + 1)*sizeof(cpdma_hdp));

        /* Set Flags */
        tx_cur->flags = TX_INIT_FLAGS;

        /* RX */

	/* Set Next Descriptor */
	rx_cur = (cpdma_hdp*)((uint32_t) rx_cur + i * sizeof(cpdma_hdp));
        rx_cur->next_descriptor = (cpdma_hdp*)((uint32_t) rx_cur + (i + 1)*sizeof(cpdma_hdp));

	/* Set Flags */
        rx_cur->flags = RX_INIT_FLAGS;

	/* Allocate Packet Buffers */
	rx_cur->buffer_pointer = kmalloc(MAX_PACKET_SIZE);
	rx_cur->buffer_length = MAX_PACKET_SIZE;

    }

    eth_interface.txch.head = tx_start;
    eth_interface.txch.num_descriptors = num_descriptors;
    
    eth_interface.txch.tail = (cpdma_hdp*)((uint32_t) tx_start + (num_descriptors - 1) * sizeof(cpdma_hdp));
    eth_interface.txch.tail->next_descriptor = 0;
    eth_interface.txch.tail->flags = TX_INIT_FLAGS;

    eth_interface.txch.free = eth_interface.txch.head;

    eth_interface.rxch.head = rx_start;
    eth_interface.rxch.num_descriptors = num_descriptors;
    
    eth_interface.rxch.tail = (cpdma_hdp*)((uint32_t) rx_start + (num_descriptors - 1) * sizeof(cpdma_hdp));
    eth_interface.rxch.tail->next_descriptor = 0;
    eth_interface.rxch.tail->flags = RX_INIT_FLAGS;
    eth_interface.rxch.tail->buffer_pointer = kmalloc(MAX_PACKET_SIZE);
    eth_interface.rxch.tail->buffer_length = MAX_PACKET_SIZE;

    eth_interface.rxch.free = eth_interface.rxch.head;
}

/*
 * cpsw_enable_cpdma_controller()
 *  - enables the controllers for rx and tx
 * */
void cpsw_enable_cpdma_controller(){

    REG(TX_CONTROL) = CPDMA_ENABLE;
    REG(RX_CONTROL) = CPDMA_ENABLE;
}

/*
 * cpsw_config_interrupts()
 *  - configs interrupts
 *  - channel 0 rx dma completion
 *  - channel 0 tx dma completion
 *  
 * */
void cpsw_config_interrupts(){

    REG(INTC_MIR_CLEAR1) = CPSW_INTMASK_CLEAR;

    REG(TX_INTMASK_SET) = CPDMA_CHANNEL_INT;
    REG(RX_INTMASK_SET) = CPDMA_CHANNEL_INT;

    REG(C0_RX_EN) = CPDMA_CHANNEL_INT;
    REG(C0_TX_EN) = CPDMA_CHANNEL_INT;

    /* Ack Interrupts */
    REG(CPDMA_EOI_VECTOR) = EOI_TX | EOI_RX;

}

/*
 * cpsw_start_recieption()
 *  - starts the recieption of packets
 *  - write start of rx chain to register
 *
 * */
void cpsw_start_recieption(){

    REG(RX0_HDP) = (uint32_t) eth_interface.rxch.head;
}

/*
 * print_mac()
 * - prints ethernet interfaces mac address
 *
 * */
void print_mac(uint8_t* mac_addr){

    get_mac();
    uart0_printf("MAC ADDR ID 0: %x:%x:%x:%x:%x:%x\n",
		    mac_addr[0],
		    mac_addr[1],
		    mac_addr[2],
		    mac_addr[3],
		    mac_addr[4],
		    mac_addr[5]);
}

/*
 * cpsw_set_transfer
 *  - sets transfer mode for port 1
 *
 * */
void cpsw_set_transfer(uint32_t transfer){

    REG(PORT1_MACCONTROL) &= ~CLEAR_TRANSFER;

    REG(PORT1_MACCONTROL) |= transfer;
}

/*
 * cpsw_enable_gmii()
 *  - enables gmii
 *  - must be done before and packets can be sent or recieved
 *
 * */
void cpsw_enable_gmii(){
  
    REG(PORT1_MACCONTROL) |= GMII_ENABLE | OH_MBPS;

}

void phy_reset();

/*
 * cpsw_init()
 *  - initializes the Ethernet subsystem for the BeagleBone Black
 *  - Follow Steps outlined in Ti Manual Section 14.4.6
 *
 * */
void cpsw_init(){

    uart0_printf("Enabling Power to PHY, this will take some time ...\n");	
    phy_reset();

    uart0_printf("Starting initialization of Common Port Switch\n");

    cpsw_select_interface();
    uart0_printf("GMII/MII Interface Selected\n");

    cpsw_pin_mux();
    uart0_printf("CPSW Pins Muxed\n");

    cpsw_enable_clocks();
    uart0_printf("CPSW Clocks Enabled\n");

    cpsw_software_reset();
    uart0_printf("CPSW Finished Software Reset\n");

    cpsw_init_cpdma_descriptors();
    uart0_printf("CPDMA Descriptors Initialized\n");

    cpsw_config_ale();
    uart0_printf("CPSW ALE Configured\n");

    uart0_printf("Configuring MDIO this will take some time ...");
    cpsw_config_mdio();
    buddy();
    uart0_printf("CPSW MDIO Configured\n");

    cpsw_config_stats();
    uart0_printf("CPSW STATS Configured\n");

    cpsw_set_ports_state();
    uart0_printf("CPSW Ports Set to FORWARD\n");

    cpsw_create_ale_entries();
    uart0_printf("CPSW ALE Entries Created for Ports\n");

    cpsw_set_port_addrs();
    uart0_printf("CPSW Ports MAC Addresses Set\n");
    print_mac(eth_interface.mac_addr);

    cpsw_setup_cpdma_descriptors();
    uart0_printf("CPDMA Descriptors Setup\n");

    cpsw_enable_cpdma_controller();
    uart0_printf("CPDMA Controller enabled\n");

    cpsw_start_recieption();
    uart0_printf("CPSW Packet Reception Started\n");
  
}

/* --------------------------PHY CODE----------------------------- */

/*
 * phy_reset()
 *  - secret function
 *  - enables power/resets the PHY
 *  - this took forever to figure out
 *  - GPIO1_8 pin can be used to reset PHY
 *  - this was found in am335x-bone-common.dtsi from the linux kernel
 *  - needs to wait a max amount of time after aserting and deasserting PHY reset
 *  - buddy function can handle this
 *
 * */
void phy_reset(void)
{
    // Configure GPIO1_8 as an output by clearing its bit in the OE register.
    REG(GPIO_OE) &= ~PHY_RESET_BIT;

    // Assert PHY reset 
    REG(GPIO_DATAOUT) &= ~PHY_RESET_BIT;
    buddy();  

    // Deassert PHY reset
    REG(GPIO_DATAOUT) |= PHY_RESET_BIT;
    buddy();  
}

/*
 * phy_readreg()
 *  - reads a phy reg and returns its value
 *
 * */
uint16_t phy_readreg(uint8_t phy_addr, uint8_t reg_addr){
	
    while ( REG(MDIOUSERACCESS0) & GO_BIT){}

    REG(MDIOUSERACCESS0) = (PHY_READ | GO_BIT | (reg_addr << REG_ADDR_SHIFT) | (phy_addr << PHY_ADDR_SHIFT));

    while ( REG(MDIOUSERACCESS0) & GO_BIT){}

    if ( REG(MDIOUSERACCESS0) & READ_ACK) {
        return REG(MDIOUSERACCESS0) & PHY_DATA;
    }

    return 2;
}

/*
 * phy_writereg()
 *  - writes data to a phy reg
 *
 * */
void phy_writereg(uint8_t phy_addr, uint8_t reg_addr, uint16_t data){

    while ( REG(MDIOUSERACCESS0) & GO_BIT){}

    REG(MDIOUSERACCESS0) = (PHY_WRITE | GO_BIT | (reg_addr << REG_ADDR_SHIFT) | (phy_addr << PHY_ADDR_SHIFT) | data);

    while ( REG(MDIOUSERACCESS0) & GO_BIT){}
}

/*
 *  phy_get_autoneg_status()
 *   - returns status of auto negotiation
 *
 * */
int phy_get_autoneg_status(uint8_t phy_addr){

    return phy_readreg(phy_addr,PHY_BSR) & AUTO_NEG_COMPLETE;
}

/*
 * phy_autonegotiate()
 *  - sets transfer capabilites of port 1
 *  - advertizes our capabilites 
 *  - sets transfer mode based on capablities of partner
 *
 * */
int phy_autonegotiate(uint8_t phy_addr){

    uint16_t data;
    uint16_t data_a;
    uint16_t data_p;    
    uint32_t transfer;
    uint16_t advert = PHY100 | PHY100_FD | PHY10 | PHY10_FD;

    data = phy_readreg(phy_addr, PHY_BCR);

    data |= AUTO_NEG_ENABLE;

    phy_writereg(phy_addr, PHY_BCR, data);

    data = phy_readreg(phy_addr, PHY_BCR);

    data_a = phy_readreg(phy_addr, PHY_AUTONEG_ADV);

    data_a &= ~AUTO_NEG_CLEAR;

    data_a |= advert;

    phy_writereg(phy_addr, PHY_AUTONEG_ADV, data_a);

    data |= AUTO_NEG_RESTART;

    phy_writereg(phy_addr, PHY_BCR, data);

    for (int i = 0; i < 5; i++){
    
	uart0_printf("PHY attempting auto negotiation, this will take a while..\n"); 
        buddy();
	if ( phy_get_autoneg_status(phy_addr) ) break;

	if (i == 4){
	
	    uart0_printf("AUTO NEG FAILED\n");
		
	    return -1;
	}
    }

    uart0_printf("AUTO NEG SUCCESSFUL\n");

    data_p = phy_readreg(phy_addr, PHY_PARTNER_CAP);

    if ( data_p & PHY100_FD ){
        uart0_printf("Setting transfer Mode to 100mbps Full Duplex\n");
	transfer = FULL_DUPLEX;

    }else if ( data_p & PHY100 ){
        uart0_printf("Setting transfer Mode to 100mbps \n");	    
        transfer = HALF_DUPLEX;

    }else if ( data_p & PHY10_FD ){
        uart0_printf("Setting transfer Mode to 10mbps Full Duplex\n");
        transfer = IN_BAND | FULL_DUPLEX;

    }else if ( data_p & PHY10 ){
        uart0_printf("Setting transfer Mode to 10mbps\n");
	transfer = IN_BAND | HALF_DUPLEX;

    }else {
    
        uart0_printf("No valid Transfer\n");
	return -1;
    }

    cpsw_set_transfer(transfer);

}

/*
 * phy_alive()
 *  - returns if Ethernet PHY is Alive
 *
 * */
int phy_alive(){

    return REG(MDIOALIVE);
}

/*
 * phy_link()
 *  - returns if Ethernet PHY link is up
 *
 * */
int phy_link(){

    return REG(MDIOLINK);
}

/*
 * phy_init()
 *  - initializes the Ethernet PHY 
 *
 * */
int phy_init(){

    uart0_printf("Checking if PHY is Alive\n");

    if (phy_alive()){
        uart0_printf("Ethernet PHY Alive\n");
    }else {
        uart0_printf("Ethernet PHY Not Alive\n");
	return -1;
    }

    phy_autonegotiate(PHY1);

    if (phy_link()){
        uart0_printf("Ethernet Cable Linked\n");
    }else {
        uart0_printf("Ethernet Cable Not Linked\n");
        return -1;
    }

    cpsw_enable_gmii();
    uart0_printf("Enabling Frame Sending and Recieving\n");

    uart0_printf("RX_HDP FLAGS %x\n",((cpdma_hdp*)REG(RX0_HDP))->flags);

    uart0_printf("ALECONTROL %x\n",REG(CPSW_ALE_CONTROL));

    uart0_printf("RXCH flags %x\n",eth_interface.rxch.head->flags);

    uart0_printf("RXCONTROL %x\n",REG(RX_CONTROL));

    uart0_printf("DAMSTATUS %x\n",REG(DMASTATUS));

    while(1){
        buddy();
	uart0_printf("RXCH flags %x\n",eth_interface.rxch.head->flags);
        uart0_printf("DAMSTATUS %x\n",REG(DMASTATUS));
	uart0_printf("DMA INT RAW STATUS %x\n",REG(CPDMA_BASE + 0xA0));
	uart0_printf("DMA INT MASKED STATUS %x\n",REG(CPDMA_BASE + 0xA4));
    } 

    return 0;
}

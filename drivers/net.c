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

/********************************************************************
* Register Access Macro
********************************************************************/

#define REG(x) (*((volatile uint32_t *)(x)))

/********************************************************************
* Byte Extraction Macros
********************************************************************/

#define BYTE0(x) (((x) & 0xFF))
#define BYTE1(x) ((((x) & 0xFF00)) >> 8)
#define BYTE2(x) ((((x) & 0xFF0000)) >> 16)
#define BYTE3(x) ((((x) & 0xFF000000)) >> 24)

/******************************************************************** 
* Bit Macro      
********************************************************************/

#define BIT(x) (1 << (x))

/* ---------------------------REGISTERS--------------------------- */

/********************************************************************
* CONTROL_MODULE REGISTERS
********************************************************************/

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

/********************************************************************
* CLOCK_MODULE REGISTERS
********************************************************************/

#define CM_PER_BASE 0x44E00000

/* CPSW Clock registers*/
#define CM_PER_CPGMAC0_CLKCTRL (CM_PER_BASE + 0x14)
#define CM_PER_CPSW_CLKSTCTRL (CM_PER_BASE + 0x144)

/********************************************************************
* CPDMA REGISTERS
********************************************************************/

#define CPDMA_BASE 0x4A100800

#define CPDMA_SOFT_RESET (CPDMA_BASE + 0x1C)

#define TX_CONTROL (CPDMA_BASE + 0x4)
#define RX_CONTROL (CPDMA_BASE + 0x14)

#define DMASTATUS (CPDMA_BASE + 0x24)

/********************************************************************
* CPSW_SL REGISTERS
********************************************************************/

#define PORT1_BASE 0x4A100D80
#define PORT2_BASE 0x4A100DC0

#define PORT1_SOFT_RESET (PORT1_BASE + 0xC)
#define PORT2_SOFT_RESET (PORT2_BASE + 0xC)

#define PORT1_MACCONTROL (PORT1_BASE + 0x4)

/********************************************************************
* CPSW_SS REGISTERS
********************************************************************/

#define CPSW_SS_BASE 0x4A100000

#define CPSW_SS_SOFT_RESET (CPSW_SS_BASE + 0x8)
#define STAT_PORT_EN (CPSW_SS_BASE + 0xC)

/********************************************************************
* CPSW_WR REGISTERS
********************************************************************/

#define CPSW_WR_BASE 0x4A101200

#define CPSW_WR_SOFT_RESET (CPSW_WR_BASE + 0x4)

/********************************************************************
* CPDMA_STATERAM REGISTERS
********************************************************************/

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

/********************************************************************
* MDIO REGISTERS
********************************************************************/

#define MDIO_BASE 0x4A101000

#define MDIO_CONTROL (MDIO_BASE + 0x4)
#define MDIOALIVE (MDIO_BASE + 0x8)
#define MDIOLINK (MDIO_BASE + 0xC)

#define MDIOUSERACCESS0 (MDIO_BASE + 0x80)

/********************************************************************
* PHY REGISTERS
********************************************************************/

#define PHY_BCR 0
#define PHY_BSR 1
#define PHY_AUTONEG_ADV 4
#define PHY_PARTNER_CAP 5

/********************************************************************
* CPSW_ALE REGISTERS
********************************************************************/

#define CPSW_ALE_BASE 0x4A100D00

#define CPSW_ALE_CONTROL (CPSW_ALE_BASE + 0x8)
#define TBLCTL (CPSW_ALE_BASE + 0x20)
#define TBLW2 (CPSW_ALE_BASE + 0x34)
#define TBLW1 (CPSW_ALE_BASE + 0x38)
#define TBLW0 (CPSW_ALE_BASE + 0x3C)

#define PORTCTL0 (CPSW_ALE_BASE + 0x40)
#define PORTCTL1 (CPSW_ALE_BASE + 0x44)
#define PORTCTL2 (CPSW_ALE_BASE + 0x48)

/********************************************************************
* CPSW_PORT REGISTERS
********************************************************************/

#define CPSW_PORT_BASE 0x4A100100

/* Port 1 */
#define P1_CONTROL (CPSW_PORT_BASE + 0x100)
#define P1_SA_LO (CPSW_PORT_BASE + 0x120)
#define P1_SA_HI (CPSW_PORT_BASE + 0x124)

/* Port 2 */
#define P2_CONTROL (CPSW_PORT_BASE + 0x200)
#define P2_SA_LO (CPSW_PORT_BASE + 0x220)
#define P2_SA_HI (CPSW_PORT_BASE + 0x224)

/********************************************************************
* INTERRUPT REGISTERS
********************************************************************/

#define INTERRUPTC_BASE 0x48200000
#define INTC_MIR_CLEAR1 (INTERRUPTC_BASE + 0xA8)

#define TX_INTMASK_SET (CPDMA_BASE + 0x88)
#define RX_INTMASK_SET (CPDMA_BASE + 0xA8)

#define C0_RX_EN (CPSW_WR_BASE + 0x14)
#define C0_TX_EN (CPSW_WR_BASE + 0x18)

#define CPDMA_EOI_VECTOR (CPDMA_BASE + 0x94)

#define RX_INT_STATUS_RAW (CPDMA_BASE + 0xA0)
#define TX_INT_STATUS_RAW (CPDMA_BASE + 0x80)

#define TX_INTMASK_CLEAR (CPDMA_BASE + 0x8C)

/********************************************************************
* GPIO REGISTERS
********************************************************************/

#define GPIO1_BASE      0x4804C000
#define GPIO_OE         (GPIO1_BASE + 0x134)
#define GPIO_DATAOUT    (GPIO1_BASE + 0x13C)

/* ------------------------REGISTER VALUES------------------------- */

/********************************************************************
* Interface Selection
********************************************************************/

#define GMII_MII_SELECT 0x0
#define GMII_ENABLE BIT(5)
#define OH_MBPS (BIT(16) | BIT(15))

/********************************************************************
* Pin Muxing 
********************************************************************/

#define DEFAULT_PIN 0x0
#define RECEIVE_ENABLE BIT(5)
#define PULLUP_ENABLE BIT(4)

/********************************************************************
* Clocks                                                        
********************************************************************/

#define CPGMAC0_ENABLE BIT(1)
#define CPGMAC0_NOTREADY (BIT(16) | BIT(17))

#define CPSWCLK_ENABLE BIT(1)
#define CPSWCLK_READY BIT(4)

/********************************************************************
* Software Reset                                                     
********************************************************************/

#define START_RESET BIT(0)
#define RESET_NOTDONE BIT(0)

/********************************************************************
* DMA                                                            
********************************************************************/

#define DESCRIPTOR_NULL 0x00000000

#define TX_INIT_FLAGS (BIT(29) | BIT(30) | BIT(31))
#define RX_INIT_FLAGS BIT(29)

#define CPDMA_ENABLE BIT(0)

/********************************************************************
* ALE                                                               
********************************************************************/

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

/********************************************************************
* MDIO                                                               
********************************************************************/

#define MDIO_ENABLE BIT(30)
#define MDIO_CLKDIV 124
#define MDIO_PREAMBLE BIT(20)
#define MDIO_FAULTENB BIT(18)
#define MDIO_CLKDIV_MASK 0xFFFF

/********************************************************************
* STATS                                                              
********************************************************************/

#define STATS_DISABLE 0x00000000

/********************************************************************
* Ports                                                             
********************************************************************/

#define PORT_FORWARD (BIT(0) | BIT(1))

/********************************************************************
* ETH                                                             
********************************************************************/

#define MAC_ADDR_LEN 6
#define MAX_PACKET_SIZE 1520
#define ETH_HEADER_SIZE 14

/********************************************************************
* CPPI
********************************************************************/

#define CPPI_RAM 0x4A102000
#define CPPI_SIZE 0x2000

#define NUM_DESCRIPTORS 50

/********************************************************************
* INTERRUPTS
********************************************************************/

#define CPSW_INTMASK_CLEAR (BIT(10) | BIT(9))

#define CPDMA_CHANNEL_INT BIT(0)

#define EOI_TX BIT(1)
#define EOI_RX BIT(0)

/********************************************************************
* PHY
********************************************************************/

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

//*******************************************************************
// ETHERTYPES
//*******************************************************************

#define IPV4 0x0800
#define ARP 0x0806

//*******************************************************************
// ARP
//*******************************************************************

#define ARP_REQUEST 0x0001
#define ARP_REPLY 0x0002
#define ARP_HARDWARE_SIZE 6
#define ARP_PROTOCOL_SIZE 4
#define HARDWARE_TYPE 0x0001

//*******************************************************************
// IPV4                                                               
//*******************************************************************

#define STATIC_IP 0xC0A80114
#define IPV4_HEADER_SIZE 20
#define IP_VERSION 4
#define IP_VERSION_HEADER 0x45
#define DHSP_ECN 0
#define IP_ID 1
#define IPV4_FLAGS 0
#define TTL 0x80

#define ICMP 1 
#define TCP 4 
#define UDP 17

//*******************************************************************
// ICMP                                                               
//*******************************************************************

#define ECHO_REPLY 0 
#define ECHO_REPLY_CODE 0 

#define ECHO_REQUEST 8 
#define ECHO_REQUEST_CODE 0

#define ICMP_PACKET_SIZE 74

//*******************************************************************
// UDP                                                               
//*******************************************************************

#define UDP_HEADER_SIZE 8
#define PSUEDO_HEADER_SIZE 20

//*******************************************************************
// SOCKETS                                                               
//*******************************************************************

#define MAX_SOCKETS 8
#define MAX_PENDING_PACKETS 10

#define PING_REQ 1 
#define SOCKET_REQ 2
#define MAX_TRANSMIT_REQUEST 10

/* ----------------------------STRUCTS----------------------------- */

/* CPDMA header discriptors */
typedef struct hdp {

    struct hdp* next_descriptor;
    uint32_t* buffer_pointer;
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

    uint32_t ip_addr;	
    uint8_t mac_addr[MAC_ADDR_LEN];
    cpdma_ch txch;
    cpdma_ch rxch;

} ethernet_interface;

ethernet_interface eth_interface;

typedef struct ethernet {

    uint8_t destination_mac[MAC_ADDR_LEN];
    uint8_t source_mac[MAC_ADDR_LEN];
    uint16_t type;

} ethernet_header;

typedef struct arp {

    uint16_t hardware_type;
    uint16_t protocol_type;
    uint8_t hardware_length;
    uint8_t protocol_length;
    uint16_t operation;
    uint8_t src_mac[MAC_ADDR_LEN];
    uint32_t src_ip;
    uint8_t dest_mac[MAC_ADDR_LEN];
    uint32_t dest_ip;

} arp_header;

typedef struct ipv4 {

    uint8_t version;
    uint8_t ihl;
    uint8_t dhsp_ecn;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags;
    uint8_t time_to_live;
    uint8_t protocol;
    uint16_t header_checksum;
    uint32_t src_ip;
    uint32_t dest_ip;

} ipv4_header;

typedef struct icmp {

  uint8_t type;
  uint8_t code;
  uint8_t header_checksum; // should be called checksum realisticly
  uint32_t data;
  uint8_t* payload;

} icmp_header;

typedef struct udp {

  uint16_t src_port;
  uint16_t dest_port;
  uint16_t length;
  uint16_t checksum;
  uint32_t* payload;

} udp_header;

struct payload {

  uint8_t* data;
  int size;

};

struct socket {
  
  uint8_t free; // is the socket free?
  uint32_t pid;
  uint16_t src_port;
  uint16_t dest_port;
  uint32_t dest_ip;
  uint8_t* dest_mac;
  uint8_t protocol;
  uint16_t buddy_protocol;
  uint8_t waiting; // is it waiting for data?
  struct payload data[MAX_PENDING_PACKETS];
  uint8_t packets_pending;

};

struct socket socket_table[MAX_SOCKETS];

struct transmit_req {

  int req_type;
  int socket_num;
  uint8_t* frame;
  int size;
  // for icmp
  uint32_t ip;
  uint8_t* mac;

};

struct transmit_queue {

  struct transmit_req req_queue[MAX_TRANSMIT_REQUEST];
  int req_pending;

};

struct transmit_queue transmit_que;

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
    
    /*REG(CPSW_ALE_CONTROL) = ENABLE_ALE | CLEAR_ALE | BIT(4) | BIT(8);*/
    REG(CPSW_ALE_CONTROL) = ENABLE_ALE | CLEAR_ALE;
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
    /*REG(PORTCTL2) |= PORT_FORWARD;*/
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

    /*REG(TBLCTL) &= ~TBLCTL_WRITE_READ;*/

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

    eth_interface.mac_addr[5] = BYTE1(REG(MAC_ID0_LO));
    eth_interface.mac_addr[4] = BYTE0(REG(MAC_ID0_LO));
    eth_interface.mac_addr[3] = BYTE3(REG(MAC_ID0_HI));
    eth_interface.mac_addr[2] = BYTE2(REG(MAC_ID0_HI));
    eth_interface.mac_addr[1] = BYTE1(REG(MAC_ID0_HI));
    eth_interface.mac_addr[0] = BYTE0(REG(MAC_ID0_HI));

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

    num_descriptors = NUM_DESCRIPTORS;

    tx_cur = tx_start;
    rx_cur = rx_start;

    uart0_printf("sizeof cpdma_hdp %x\n",sizeof(cpdma_hdp));

    /* Create Descriptor Chains */
    for (int i = 0; i < num_descriptors - 1; i++){
    
        /* TX */

	/* Set Next Descriptor */    
        /*tx_cur = (cpdma_hdp*)((uint32_t) tx_start + (i * sizeof(cpdma_hdp)));*/
	/*tx_cur->next_descriptor = (cpdma_hdp*)((uint32_t) tx_cur + sizeof(cpdma_hdp));*/

        /* Set Flags */
        /*tx_cur->flags = TX_INIT_FLAGS;*/

        /* RX */

	/* Set Next Descriptor */
	rx_cur = (cpdma_hdp*)((uint32_t) rx_start + (i * sizeof(cpdma_hdp)));
        rx_cur->next_descriptor = (cpdma_hdp*)((uint32_t) rx_cur + sizeof(cpdma_hdp));

	/* Set Flags */
        rx_cur->flags = RX_INIT_FLAGS;

	/* Allocate Packet Buffers */
	rx_cur->buffer_pointer = kmalloc(MAX_PACKET_SIZE);
	rx_cur->buffer_length = MAX_PACKET_SIZE;

	/*uart0_printf("RX_CUR  %x -> next %x\n",rx_cur,rx_cur->next_descriptor);*/
	/*uart0_printf("RX_CUR flags %x\n", rx_cur->flags);*/
	/*uart0_printf("index %d\n",i);*/

    }

    tx_start->next_descriptor = 0;
    tx_start->flags = TX_INIT_FLAGS;

    eth_interface.txch.head = tx_start;
    eth_interface.txch.num_descriptors = 1;

    
    /*eth_interface.txch.tail = (cpdma_hdp*)((uint32_t) tx_start + (num_descriptors - 1) * sizeof(cpdma_hdp));*/
    /*eth_interface.txch.tail->next_descriptor = 0;*/
    /*eth_interface.txch.tail->flags = TX_INIT_FLAGS;*/

    /*eth_interface.txch.free = eth_interface.txch.head;*/

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

    REG(CPDMA_EOI_VECTOR) = EOI_TX | EOI_RX;

    REG(TX_INTMASK_SET) = CPDMA_CHANNEL_INT;
    REG(RX_INTMASK_SET) = CPDMA_CHANNEL_INT;

    REG(C0_RX_EN) = CPDMA_CHANNEL_INT;
    REG(C0_TX_EN) = CPDMA_CHANNEL_INT;

    /* Ack Interrupts */
    /*REG(CPDMA_EOI_VECTOR) = EOI_TX | EOI_RX;*/

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
    uart0_printf("MAC ADDR: %x:%x:%x:%x:%x:%x\n",
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

    uart0_printf("RX HDP %x\n",REG(RX0_HDP));
  
}

/* --------------------------HELPER----------------------------- */

void debug(){

    buddy();
    uart0_printf("RXCH head %x\n",eth_interface.rxch.head);
    uart0_printf("RXCH head flags %x\n",eth_interface.rxch.head->flags);
    uart0_printf("DMASTATUS %x\n",REG(DMASTATUS));
    uart0_printf("DMA INT RAW STATUS %x\n",REG(CPDMA_BASE + 0xA0));
    uart0_printf("DMA INT MASKED STATUS %x\n",REG(CPDMA_BASE + 0xA4));
    uart0_printf("RX CP %x\n",REG(RX0_CP));
    uart0_printf("RX CP flags %x\n",((cpdma_hdp*)REG(RX0_CP))->flags);
    uart0_printf("RX CP next %x\n",((cpdma_hdp*)REG(RX0_CP))->next_descriptor);
    //uart0_printf("RX CP next flags %x\n",((cpdma_hdp*)REG(RX0_CP))->next_descriptor->flags);
    uart0_printf("RXCH tail %x\n",eth_interface.rxch.tail);
    uart0_printf("RXCH tail flags %x\n",eth_interface.rxch.tail->flags);

}

/*
 * ntohl()
 *  - returns input network word to host word
 *
 * */
uint32_t ntohl(uint32_t net) {
    return ((net & 0x000000FF) << 24) |
           ((net & 0x0000FF00) << 8)  |
           ((net & 0x00FF0000) >> 8)  |
           ((net & 0xFF000000) >> 24);
}

/*
 * htonl()
 *  - returns input host word to network word
 *
 * */
uint32_t htonl(uint32_t host) {
    return ((host & 0x000000FF) << 24) |
           ((host & 0x0000FF00) << 8)  |
           ((host & 0x00FF0000) >> 8)  |
           ((host & 0xFF000000) >> 24);
}

/*
 * dumps the hex of a buffer
 *  - used to dump packet data
 *
 * */
void hex_dump(uint32_t* data, int len){

    len = len / sizeof(uint32_t);

    uart0_printf("HEX DUMP\n");	
    for (int i = 0; i < len; i++){
    
        uart0_printf("%x\n",ntohl(data[i])); 
    }
}

/*
 * normal byte copy
 * */
void net_memcopy(uint8_t* dest, uint8_t* src, int size){

  for (int i = 0; i < size ; i++){
    dest[i] = src[i];
  }
}

/* --------------------------PACKETS----------------------------- */

/*
 * cpsw_transmit()
 *  - transmits a packet given the buffer containg the packet
 *  - lowkey packets need to be aligned to 4 bytes
 *
 * */
int cpsw_transmit(uint32_t* packet, uint32_t size){

    cpdma_hdp* tx_desc = eth_interface.txch.head;
    uint32_t len = size / sizeof(uint32_t);

    /*hex_dump(packet,size);*/

    tx_desc->next_descriptor = 0;
    tx_desc->buffer_pointer = packet;
    tx_desc->buffer_length = size;
    tx_desc->flags = TX_INIT_FLAGS;
    tx_desc->flags |= (size & 0xFFF);

    REG(TX0_HDP) = (uint32_t) tx_desc;

    //uart0_printf("Transmiting Packet\n");

    // TX INT STAT RAW
    while (!REG(TX_INT_STATUS_RAW)){}

    //uart0_printf("Packet Transmited\n"); 

    REG(TX_INTMASK_CLEAR) = CPDMA_CHANNEL_INT;

    REG(TX0_CP) = (uint32_t) tx_desc;

    REG(CPDMA_EOI_VECTOR) = EOI_TX;

    REG(TX_INTMASK_SET) = CPDMA_CHANNEL_INT;

    // change design and make caller free packet
    //kfree(packet);

    /* HARD WARE BUG TRYING TO FIX THIS */
    
    /*software_reset(CPDMA_SOFT_RESET);*/
    /*cpsw_init_cpdma_descriptors();*/
    /*cpsw_setup_cpdma_descriptors();*/
    /*cpsw_enable_cpdma_controller();*/
    /*cpsw_config_interrupts();*/
    /*cpsw_start_recieption();*/

    return REG(TX_INT_STATUS_RAW);

}

void eth_recv(uint32_t* frame, int size);

/*
 *
 * */
void process_packet(uint8_t* packet, int size){

     //uart0_printf("Packet Addr %x | Packet Size %d \n",packet,size);

     /*hex_dump((uint32_t*)packet,size);*/

     if(size) eth_recv((uint32_t*)packet,size);


}

/*
 * cpsw_recv()
 *  - checks DMA RX channel for any packets
 *  - Processes Packets
 *
 * */
int cpsw_recv(){

    cpdma_hdp* start = eth_interface.rxch.free;
    cpdma_hdp* end = (cpdma_hdp* )REG(RX0_CP);

    int eoq = 0;

    // int status raw need to replace this with macro
    uint32_t status = REG(RX_INT_STATUS_RAW);

    //uart0_printf("\n\nStarting Packet Processing\n");

    if (!status){
        //uart0_printf("No Packets\n");
	      return -1;
    }

    while (!(start->flags & BIT(29))){
    
        //uart0_printf("\nPacket Recieved\n");

	      process_packet((uint8_t*)start->buffer_pointer,start->buffer_length & 0xFFF);

	      start->flags = RX_INIT_FLAGS;
	      start->buffer_length = MAX_PACKET_SIZE;
	
        REG(RX0_CP) = (uint32_t) start;

	      start = start->next_descriptor;

	// End of queue
	      if (start == 0){   
	        eoq = 1;
          //uart0_printf("End of queue reached\n");
          cpsw_start_recieption();	    
	        break;
	      }
    }

    if (eoq) eth_interface.rxch.free = eth_interface.rxch.head;
    else eth_interface.rxch.free = start;

    REG(CPDMA_EOI_VECTOR) = EOI_RX;

    return 0;


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
    /* Configure GPIO1_8 as an output by clearing its bit in the OE register. */
    REG(GPIO_OE) &= ~PHY_RESET_BIT;

    /* Assert PHY reset  */
    REG(GPIO_DATAOUT) &= ~PHY_RESET_BIT;
    buddy();  

    /* Deassert PHY reset */
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

    return 0;
}

/* --------------------------ETHERNET----------------------------- */

/*
 * eth_transmit()
 *  - takes in packet and adds ethernet frame header
 *  - first 14 bytes of buffer must be free for header to be added
 *
 * */
void eth_transmit(uint8_t* frame, int size, uint8_t* dest, uint16_t type){

    uint8_t* src = eth_interface.mac_addr;

    //uart0_printf("\nCrafting Ethernet Frame\n");	
 
    frame[0] = dest[0];
    frame[1] = dest[1];
    frame[2] = dest[2];
    frame[3] = dest[3];
    frame[4] = dest[4];
    frame[5] = dest[5];

    //uart0_printf("Destination MAC\n");
    //print_mac(dest);

    frame[6] = src[0];
    frame[7] = src[1];
    frame[8] = src[2];
    frame[9] = src[3];
    frame[10] = src[4];
    frame[11] = src[5];

    //uart0_printf("Source MAC\n");
    //print_mac(src);

    frame[12] = (type & 0xFF00) >> 8;
    frame[13] = type & 0xFF;

    //uart0_printf("Frame Type %x\n", type);

    cpsw_transmit((uint32_t*)frame,size);
}

void arp_recv(ethernet_header frame_header, uint32_t* frame, int size);
void ipv4_recv(ethernet_header frame_header,uint32_t* frame, int size, uint8_t* frame_ptr);

/*
 * eth_recv()
 *  - takes raw frame and extracts ethernet header
 *  
 * */
void eth_recv(uint32_t* frame, int size){
    
    ethernet_header frame_header;
    uint8_t* frame_ptr = (uint8_t*) frame; // for packets that will be reperposed on transmit

    uint32_t word1 = 0;
    uint32_t word2 = 0;
    uint32_t word3 = 0;
    uint32_t word4 = 0;

    //uart0_printf("Processing Frame\n");

    word1 = ntohl(frame[0]);
    word2 = ntohl(frame[1]);
    word3 = ntohl(frame[2]);
    word4 = ntohl(frame[3]);

    frame_header.destination_mac[0] = BYTE3(word1);
    frame_header.destination_mac[1] = BYTE2(word1);
    frame_header.destination_mac[2] = BYTE1(word1);
    frame_header.destination_mac[3] = BYTE0(word1);
    frame_header.destination_mac[4] = BYTE3(word2);
    frame_header.destination_mac[5] = BYTE2(word2);

    //uart0_printf("Destination MAC: \n");
    //print_mac(frame_header.destination_mac);

    frame_header.source_mac[0] = BYTE1(word2);
    frame_header.source_mac[1] = BYTE0(word2);
    frame_header.source_mac[2] = BYTE3(word3);
    frame_header.source_mac[3] = BYTE2(word3);
    frame_header.source_mac[4] = BYTE1(word3);
    frame_header.source_mac[5] = BYTE0(word3);

    //uart0_printf("Source MAC: \n");
    //print_mac(frame_header.source_mac);

    frame_header.type = (BYTE3(word4) << 8) | (BYTE2(word4));

    //uart0_printf("Frame Type: %x \n",frame_header.type);

    switch (frame_header.type) {
    
    case ARP:
	  {
	    //uart0_printf("Packet Type ARP\n");

      // skip frame header but include ether type for alignment
		
		  frame = (uint32_t*)((uint32_t) frame + 3*sizeof(uint32_t)); 
      size = size - 3*sizeof(uint32_t);

	    arp_recv(frame_header,frame,size);
	  }
	  break;

	  case IPV4:
	  {

		  //uart0_printf("Packet Type IPV4\n");

		  // skip frame header but include ether type for alignment

      frame = (uint32_t*)((uint32_t) frame + 3*sizeof(uint32_t));
      size = size - 3*sizeof(uint32_t);

		  ipv4_recv(frame_header,frame,size, frame_ptr);
	  }
	  break;

	  default:
	  {
	    //uart0_printf("Unsupported Packet Type\n");
	  }
    }
}

/* --------------------------ARP----------------------------- */

/*
 *
 * */
void print_ip(uint32_t ip){


    uint8_t ip1 = BYTE3(ip);
    uint8_t ip2 = BYTE2(ip);
    uint8_t ip3 = BYTE1(ip);
    uint8_t ip4 = BYTE0(ip);

    uart0_printf("IP: %d.%d.%d.%d\n", ip1,ip2,ip3,ip4);

}

/*
 *
 * */
void arp_transmit(uint8_t* frame, int size, uint8_t* dest_mac, uint8_t* dether_mac, uint32_t dest_ip, uint16_t opcode){

    frame[14] = (HARDWARE_TYPE & 0xFF00) >> 8;
    frame[15] = HARDWARE_TYPE & 0x00FF;

    frame[16] = (IPV4 & 0xFF00) >> 8;
    frame[17] = IPV4 & 0x00FF;

    frame[18] = ARP_HARDWARE_SIZE;
    frame[19] = ARP_PROTOCOL_SIZE;

    frame[20] = (opcode & 0xFF00) >> 8;
    frame[21] = opcode & 0x00FF;

    frame[22] = eth_interface.mac_addr[0];
    frame[23] = eth_interface.mac_addr[1];
    frame[24] = eth_interface.mac_addr[2];
    frame[25] = eth_interface.mac_addr[3];
    frame[26] = eth_interface.mac_addr[4];
    frame[27] = eth_interface.mac_addr[5];

    frame[28] = BYTE3(eth_interface.ip_addr);
    frame[29] = BYTE2(eth_interface.ip_addr);
    frame[30] = BYTE1(eth_interface.ip_addr);
    frame[31] = BYTE0(eth_interface.ip_addr);

    frame[32] = dest_mac[0];
    frame[33] = dest_mac[1];
    frame[34] = dest_mac[2];
    frame[35] = dest_mac[3];
    frame[36] = dest_mac[4];
    frame[37] = dest_mac[5];

    frame[38] = BYTE3(dest_ip);
    frame[39] = BYTE2(dest_ip);
    frame[40] = BYTE1(dest_ip);
    frame[41] = BYTE0(dest_ip);

    eth_transmit(frame,size,dether_mac,ARP);

}

/*
 *
 * */
void arp_anounce(){

    uint8_t multicast[6] = { 0xFF, 0xFF , 0xFF, 0xFF, 0xFF, 0xFF};

    uint8_t anoun[6] = { 0, 0 , 0, 0, 0, 0};

    uint8_t* packet = (uint8_t*) kmalloc(128);

    uart0_printf("Broadcasting ARP Anouncement packet\n");

    arp_transmit(packet,128, anoun, multicast, eth_interface.ip_addr, ARP_REQUEST);

    kfree(packet);
}

/*
 *
 * */
void arp_garp(){

    int8_t multicast[6] = { 0xFF, 0xFF , 0xFF, 0xFF, 0xFF, 0xFF};

    uint8_t* packet = (uint8_t*) kmalloc(128);

    uart0_printf("Broadcasting ARP Gratuitous packet\n");

    arp_transmit(packet,128, multicast, multicast, eth_interface.ip_addr, ARP_REQUEST);

    kfree(packet);
}

/*
 *
 * */
int arp_reply(arp_header arp_request){

    uint8_t* packet = (uint8_t*) kmalloc(128);

    if (arp_request.dest_ip != STATIC_IP) return -1;

    uart0_printf("\nReplying to Arp Request\n");
    arp_transmit(packet,128,arp_request.src_mac,arp_request.src_mac,arp_request.src_ip,ARP_REPLY);

    kfree(packet);

}

/*
 * arp_recv
 *  - pass in frame with ethernet header removed + 2 bytes for align
 * */
void arp_recv(ethernet_header frame_header, uint32_t* frame, int size){

    arp_header frame_arp;

    uint32_t word1 = 0;
    uint32_t word2 = 0;
    uint32_t word3 = 0;
    uint32_t word4 = 0;
    uint32_t word5 = 0;
    uint32_t word6 = 0;
    uint32_t word7 = 0;
    uint32_t word8 = 0;

    //uart0_printf("Handling ARP Request\n");

    word1 = ntohl(frame[0]);
    word2 = ntohl(frame[1]);
    word3 = ntohl(frame[2]);
    word4 = ntohl(frame[3]);
    word5 = ntohl(frame[4]);
    word6 = ntohl(frame[5]);
    word7 = ntohl(frame[6]);
    word8 = ntohl(frame[7]);

    frame_arp.hardware_type = word1 & 0x0000FFFF;

    //uart0_printf("Hardware Type %x\n",frame_arp.hardware_type);

    frame_arp.protocol_type = (word2 & 0xFFFF0000) >> 16;

    //uart0_printf("Protocol Type %x\n",frame_arp.protocol_type);

    frame_arp.hardware_length = (word2 & 0x0000FF00) >> 8;

    frame_arp.protocol_length = word2 & 0x000000FF;

    frame_arp.operation = (word3 & 0xFFFF0000) >> 16;

    frame_arp.src_mac[0] = BYTE1(word3);
    frame_arp.src_mac[1] = BYTE0(word3);
    frame_arp.src_mac[2] = BYTE3(word4);
    frame_arp.src_mac[3] = BYTE2(word4);
    frame_arp.src_mac[4] = BYTE1(word4);
    frame_arp.src_mac[5] = BYTE0(word4);

    //uart0_printf("Source MAC\n");
    //print_mac(frame_arp.src_mac);

    frame_arp.src_ip = word5;

    //uart0_printf("Source IP\n");
    //print_ip(frame_arp.src_ip);

    frame_arp.dest_mac[0] = BYTE3(word6);
    frame_arp.dest_mac[1] = BYTE2(word6);
    frame_arp.dest_mac[2] = BYTE1(word6);
    frame_arp.dest_mac[3] = BYTE0(word6);
    frame_arp.dest_mac[4] = BYTE3(word7);
    frame_arp.dest_mac[5] = BYTE2(word7);

    //uart0_printf("Destination MAC\n");
    //print_mac(frame_arp.dest_mac);

    frame_arp.dest_ip = (word7 & 0x0000FFFF) << 16 | (word8 & 0xFFFF0000) >> 16;

    //uart0_printf("Destination IP\n");
    //print_ip(frame_arp.dest_ip);

    switch (frame_arp.operation){
    
        case ARP_REQUEST:
	    {
	        arp_reply(frame_arp);
	    }
	    break;

	case ARP_REPLY:
	    {
	    
	    }
	    break;

	default:
	    {
	    
	    }
    }

}

/* --------------------------IPV4----------------------------- */

/*
 * https://tools.ietf.org/html/rfc1071
 * modified to work for 4 byte aligned Access
 * */
uint16_t ipv4_checksum(uint8_t* ipv4_header, int size){

  uint32_t sum = 0;
  uint8_t* temp;

  while (size > 1) {

    temp = (uint8_t*)((uint32_t)ipv4_header + sizeof(uint8_t));
    sum += ((uint16_t)(*ipv4_header)) << 8 | *temp;
    size -= sizeof(uint16_t);
    ipv4_header = (uint8_t*)((uint32_t)ipv4_header + sizeof(uint16_t));
  }

  if (size > 0) sum += *(uint8_t*) ipv4_header;

  while (sum>>16) sum = (sum & 0xFFFF) + (sum >> 16);

  return ~sum;
}

void icmp_recv(ethernet_header eth_header, ipv4_header ip_header, uint32_t* frame, int size, uint8_t* frame_ptr);
void udp_recv(ethernet_header eth_header, ipv4_header ip_header, uint32_t* frame, int size);

/*
 *
 * */
void ipv4_recv(ethernet_header frame_header,uint32_t* frame, int size, uint8_t* frame_ptr){

  ipv4_header ip_header;

  uint32_t word1 = 0;
  uint32_t word2 = 0;
  uint32_t word3 = 0;
  uint32_t word4 = 0;
  uint32_t word5 = 0;
  uint32_t word6 = 0;

  //uart0_printf("Handling IPV4 Packet\n");

  word1 = ntohl(frame[0]);
  word2 = ntohl(frame[1]);
  word3 = ntohl(frame[2]);
  word4 = ntohl(frame[3]);
  word5 = ntohl(frame[4]);
  word6 = ntohl(frame[5]); 

  ip_header.version = BYTE1(word1) & 0xF0;
  ip_header.ihl = BYTE1(word1) & 0x0F;

  ip_header.dhsp_ecn = BYTE0(word1);

  ip_header.total_length = (BYTE3(word2) << 8) & BYTE2(word2);
  ip_header.identification = (BYTE1(word2) << 8) & BYTE0(word2);

  ip_header.flags = (BYTE3(word3) << 8) & BYTE2(word3);

  ip_header.time_to_live = BYTE1(word3);
  ip_header.protocol = BYTE0(word3);

  //uart0_printf("Protocol %x\n",ip_header.protocol);

  ip_header.header_checksum = (BYTE3(word4) << 8) & BYTE2(word4); 

  ip_header.src_ip = ((word4 & 0x0000FFFF) << 16) | ((word5 & 0xFFFF0000) >> 16);
 
  //uart0_printf("Source IP\n");
  //print_ip(ip_header.src_ip);
 
  ip_header.dest_ip = ((word5 & 0x0000FFFF) << 16) | ((word6 & 0xFFFF0000) >> 16);

  //uart0_printf("Dest IP\n");
  //print_ip(ip_header.dest_ip);

  switch (ip_header.protocol){
    case ICMP:
      {
        //uart0_printf("ICMP Packet \n");
        
        // removing ipv4 header but keeping in last word for alignment
        frame = (uint32_t*)((uint32_t) frame + 5*sizeof(uint32_t));
        size = size - 5*sizeof(uint32_t);

        icmp_recv(frame_header, ip_header, frame, size, frame_ptr);
      }
      break;

    case UDP:
      {
        //uart0_printf("UDP Packet \n");
      
        // removing ipv4 header but keeping in last word for alignment
        frame = (uint32_t*)((uint32_t) frame + 5*sizeof(uint32_t));
        size = size - 5*sizeof(uint32_t);

        udp_recv(frame_header, ip_header, frame, size);
      }
      break;
      
    case TCP:
      {
        //uart0_printf("TCP Packet \n");
      } 
      break;

    default:
      {
        //uart0_printf("Unsupported IPV4 Protocol\n");
      }
  }

}

/*
 *
 * */
void ipv4_transmit(uint8_t* frame, uint16_t size, uint8_t protocol, uint32_t dest_ip, uint8_t* dest_mac){

  uint16_t checksum = 0;
  uint16_t len = size - ETH_HEADER_SIZE;

  frame[14] = IP_VERSION_HEADER;
  frame[15] = DHSP_ECN;

  frame[16] = (len & 0xFF00) >> 8;
  frame[17] = len & 0x00FF;

  // only supporting datagrams of 1 packet size
  frame[18] = (IP_ID & 0xFF00) >> 8;
  frame[19] = IP_ID & 0x00FF;
  frame[20] = (IPV4_FLAGS & 0xFF00) >> 8;
  frame[21] = IPV4_FLAGS & 0x00FF;
  
  frame[22] = TTL;

  frame[23] = protocol;

  frame[24] = (checksum & 0xFF00) >> 8;
  frame[25] = checksum & 0x00FF;

  frame[26] = BYTE3(STATIC_IP);
  frame[27] = BYTE2(STATIC_IP);
  frame[28] = BYTE1(STATIC_IP);
  frame[29] = BYTE0(STATIC_IP);

  frame[30] = BYTE3(dest_ip);
  frame[31] = BYTE2(dest_ip);
  frame[32] = BYTE1(dest_ip);
  frame[33] = BYTE0(dest_ip);

  checksum = ipv4_checksum(&(frame[14]),IPV4_HEADER_SIZE);
  frame[24] = (checksum & 0xFF00) >> 8;
  frame[25] = checksum & 0x00FF;

  eth_transmit(frame, size, dest_mac, IPV4);

}

/* --------------------------ICMP----------------------------- */

void icmp_echo_reply(ethernet_header eth_header, ipv4_header ip_header, icmp_header icmp ,uint8_t* frame, int size);

void icmp_recv(ethernet_header eth_header, ipv4_header ip_header, uint32_t* frame, int size, uint8_t* frame_ptr){
 
  icmp_header icmp;

  uint32_t word1 = 0;
  uint32_t word2 = 0;
  uint32_t word3 = 0;

  //uart0_printf("Handling ICMP Packet\n");

  word1 = ntohl(frame[0]);
  word2 = ntohl(frame[1]);
  word3 = ntohl(frame[2]); 

  icmp.type = BYTE1(word1);
  icmp.code = BYTE0(word1);

  icmp.header_checksum = (word2 & 0xFFFF0000) >> 16;
  icmp.data = ((word2 & 0x0000FFFF) << 16) | ((word3 & 0xFFFF0000) >> 16);

  // remember that last 2 bytes of icmp header is included
  icmp.payload = (uint8_t*) &(frame[2]);

  switch (icmp.type) {  

    case ECHO_REQUEST:
      {
        uart0_printf("Ping from ");
        print_ip(ip_header.src_ip);
        icmp_echo_reply(eth_header,ip_header,icmp,frame_ptr,size);
      }
      break;

    case ECHO_REPLY:
      {
        uart0_printf("Recieved Echo Reply from: ");
        print_ip(ip_header.src_ip);
      }
      break;

    default:
      {
        //uart0_printf("Unsupported ICMP type\n");
      }
      break;

  }

}

void icmp_transmit(uint8_t* frame, int size, uint8_t type, uint8_t code, uint32_t data, uint32_t dest_ip, uint8_t* dest_mac){

  uint16_t checksum = 0;
  int len = size - ETH_HEADER_SIZE - IPV4_HEADER_SIZE;

  frame[34] = type;
  frame[35] = code;
  
  frame[36] = (checksum & 0xFF00) >> 8;
  frame[37] = checksum & 0x00FF;

  frame[38] = BYTE3(data);
  frame[39] = BYTE2(data);
  frame[40] = BYTE1(data);
  frame[41] = BYTE0(data);

  checksum = ipv4_checksum((&(frame[34])),len);
  frame[36] = (checksum & 0xFF00) >> 8;
  frame[37] = checksum & 0x00FF;

  ipv4_transmit(frame,size,ICMP,dest_ip,dest_mac);

}

void icmp_echo_reply(ethernet_header eth_header, ipv4_header ip_header, icmp_header icmp ,uint8_t* frame, int size){

  size = size + IPV4_HEADER_SIZE + ETH_HEADER_SIZE;

  icmp_transmit(frame, size, ECHO_REPLY, ECHO_REPLY_CODE, icmp.data, ip_header.src_ip, eth_header.source_mac);

}

void icmp_echo_request(uint32_t ip, uint8_t* mac){

  uint8_t* packet = (uint8_t*) kmalloc(ICMP_PACKET_SIZE);

  uart0_printf("Pinging ");
  print_ip(ip);
  icmp_transmit(packet, ICMP_PACKET_SIZE, ECHO_REQUEST, ECHO_REQUEST_CODE, 0, ip, mac);

  kfree(packet);
}

/* --------------------------UDP------------------------------ */

int socket_waiting(int socket_num, uint16_t dest_port, uint16_t bp){

  struct socket soc = socket_table[socket_num];

  if (!soc.waiting){
    return 0;
  }

  if (soc.src_port != dest_port){
    return 0;
  }

  if (soc.buddy_protocol != bp){
    return 0;
  }

  return 1;

}

/*
 * drops packets if pending packet buffer is full
 *
 * */
int socket_store(int socket_num, uint32_t* payload, int size){

  uint32_t* buffer;
  struct socket soc = socket_table[socket_num];

  if (soc.packets_pending >= MAX_PENDING_PACKETS){
    return -1;
  }

  buffer = (uint32_t*) kmalloc(size);

  for (int i = 0; i < (size / sizeof(uint32_t)); i++){
    buffer[i] = ntohl(payload[i]); 
  }

  socket_table[socket_num].data[soc.packets_pending].data = (uint8_t*) buffer;
  socket_table[socket_num].data[soc.packets_pending].size = size;

  socket_table[socket_num].packets_pending += 1; 
  
  uart0_printf("Socket %d Recieved Packet\n", socket_num);

  return 1;

}

void udp_recv(ethernet_header eth_header, ipv4_header ip_header, uint32_t* frame, int size){

  udp_header udp;
  uint16_t buddy_protocol;
  int payload_length;

  uint32_t word1 = 0;
  uint32_t word2 = 0;
  uint32_t word3 = 0;

  //uart0_printf("Handling UDP packet\n");

  word1 = ntohl(frame[0]);
  word2 = ntohl(frame[1]);
  word3 = ntohl(frame[2]);

  udp.src_port = word1 & 0x0000FFFF;
  udp.dest_port = (word2 & 0xFFFF0000) >> 16;

  //uart0_printf("Source port: %d | Destination port %d\n",udp.src_port,udp.dest_port);

  udp.length = word2 & 0x0000FFFF;
  udp.checksum = (word3 & 0xFFFF0000 ) >> 16;

  // remember that last 2 bytes of header are included in payload due to alignment issues
  // payload needs to still be converted from network order
  // can lowkey use first 2 bytes of payload as small header for our own udp protocols
  
  buddy_protocol = word3 & 0x0000FFFF;

  udp.payload = &(frame[3]);
  payload_length = size - 3*sizeof(uint32_t);

  // check if socket is waiting for this packet
  
  for (int i = 0; i < MAX_SOCKETS; i++){
    if (socket_waiting(i,udp.dest_port,0)){
      socket_store(i,udp.payload,payload_length);
    }
  }

}

/*
 * udp checksum uses pseudo header to compute checksum
 * */
uint16_t udp_checksum(uint8_t* frame, uint32_t src_ip, uint32_t dest_ip, uint16_t size, uint16_t src_port, uint16_t dest_port){

  uint16_t pseudo_len = size - UDP_HEADER_SIZE + PSUEDO_HEADER_SIZE;

  uint8_t* pseudo_packet = (uint8_t*)kmalloc(pseudo_len);

  pseudo_packet[0] = BYTE3(src_ip);
  pseudo_packet[1] = BYTE2(src_ip);
  pseudo_packet[2] = BYTE1(src_ip);
  pseudo_packet[3] = BYTE0(src_ip);

  pseudo_packet[4] = BYTE3(dest_ip);
  pseudo_packet[5] = BYTE2(dest_ip);
  pseudo_packet[6] = BYTE1(dest_ip);
  pseudo_packet[7] = BYTE0(dest_ip);

  pseudo_packet[8] = 0;
  pseudo_packet[9] = UDP;

  pseudo_packet[10] = (pseudo_len & 0xFF00) >> 8;
  pseudo_packet[11] = pseudo_len & 0x00FF;

  pseudo_packet[12] = (src_port & 0xFF00) >> 8;
  pseudo_packet[13] = src_port & 0x00FF;

  pseudo_packet[14] = (dest_port & 0xFF00) >> 8;
  pseudo_packet[15] = dest_port & 0x00FF;

  pseudo_packet[16] = (pseudo_len & 0xFF00) >> 8;
  pseudo_packet[17] = pseudo_len & 0x00FF;

  pseudo_packet[18] = 0;
  pseudo_packet[19] = 0;

  net_memcopy(&(pseudo_packet[20]),&(frame[42]), size - UDP_HEADER_SIZE);

  return ipv4_checksum(pseudo_packet, pseudo_len);
}

void udp_transmit(uint8_t* frame, uint16_t size, uint16_t src_port, uint16_t dest_port, uint32_t dest_ip, uint8_t* dest_mac){

  uint16_t length = size - ETH_HEADER_SIZE - IPV4_HEADER_SIZE;
  uint16_t checksum = udp_checksum(frame,STATIC_IP,dest_ip,length,src_port,dest_port); 
  
  frame[34] = (src_port & 0xFF00) >> 8;
  frame[35] = src_port & 0x00FF;

  frame[36] = (dest_port & 0xFF00 ) >> 8;
  frame[37] = dest_port & 0x00FF;

  frame[38] = (length & 0xFF00) >> 8;
  frame[39] = length & 0x00FF;

  frame[40] = (checksum & 0xFF00) >> 8;
  frame[41] = checksum & 0x00FF;

  ipv4_transmit(frame,size,UDP,dest_ip, dest_mac);
}

/* --------------------------SOCKETS-------------------------- */

void init_sockets(){

  for(int i = 0; i < MAX_SOCKETS; i++){
    socket_table[i].free = 1;
    socket_table[i].pid = 0;
    socket_table[i].src_port = 0;
    socket_table[i].dest_port = 0;
    socket_table[i].dest_ip = 0;
    socket_table[i].dest_mac = 0;
    socket_table[i].protocol = 0;
    socket_table[i].buddy_protocol = 0;
    socket_table[i].waiting = 0;
    socket_table[i].packets_pending = 0;
  }

}

int find_free_socket(){

  for (int i = 0; i < MAX_SOCKETS; i++){
    if(socket_table[i].free) return i;
  }

  return -1;
}

/*
 * Allocates Socket and returns index in socket table
 *
 * */
int socket(uint32_t pid, uint16_t src_port, uint16_t dest_port,uint8_t* dest_mac,uint32_t dest_ip, uint8_t protocol, uint8_t bp){

  int socket_num = find_free_socket();

  if (socket_num == -1) return -1;

  socket_table[socket_num].free = 0;

  socket_table[socket_num].pid = pid;
  socket_table[socket_num].src_port = src_port;
  socket_table[socket_num].dest_port = dest_port;
  socket_table[socket_num].dest_ip = dest_ip;
  socket_table[socket_num].dest_mac = dest_mac;
  socket_table[socket_num].protocol = protocol;
  socket_table[socket_num].buddy_protocol = bp;

  return socket_num;

}

/*
 * set socket as waiting for data
 *
 * */
void socket_bind(int socket_num){

  socket_table[socket_num].waiting = 1;

}

/*
 * set socket as not waiting for data
 *
 * */
void socket_unbind(int socket_num){

  socket_table[socket_num].waiting = 0;

}

/*
 * returns pointer to data 
 *
 * */
struct payload socket_recv(int socket_num){

  uint8_t data_index = socket_table[socket_num].packets_pending - 1;

  struct payload data = socket_table[socket_num].data[data_index];

  socket_table[socket_num].packets_pending -= 1;

  return data;

}

void socket_send(int socket_num, uint8_t* frame, int size){

  struct socket soc = socket_table[socket_num];

  switch(soc.protocol){
    case UDP:
      {
        udp_transmit(frame,size,soc.src_port,soc.dest_port,soc.dest_ip,soc.dest_mac);
      }
      break;
  }
}
/* --------------------------FUSION--------------------------- */

/*
 * creates and ands transmit request to queue
 *
 * */
int transmit_request(uint8_t* frame, int size, int req_type, int socket_num, uint32_t ip, uint8_t* mac){

  int index = transmit_que.req_pending;

  if (index >= MAX_TRANSMIT_REQUEST){
    return -1;
  }

  transmit_que.req_queue[index].req_type = req_type;
  transmit_que.req_queue[index].socket_num = socket_num;
  transmit_que.req_queue[index].frame = frame;
  transmit_que.req_queue[index].size = size;
  transmit_que.req_queue[index].ip = ip;
  transmit_que.req_queue[index].mac = mac;

  transmit_que.req_pending += 1;

  return 1;

}

int socket_transmit_request(int socket_num, uint8_t* frame, int size){

  return transmit_request(frame, size, SOCKET_REQ, socket_num, 0, 0);

}

int ping_request(uint32_t ip, uint8_t* mac){

  uint8_t* mac_copy = (uint8_t*) kmalloc(MAC_ADDR_LEN);

  net_memcopy(mac_copy, mac, MAC_ADDR_LEN);

  return transmit_request(0,0,PING_REQ,0,ip,mac_copy);

}

void handle_transmit_request(struct transmit_req* req){

  int req_type = req->req_type;

  switch (req_type) {

    case PING_REQ:
      {
        icmp_echo_request(req->ip,req->mac);
        kfree(req->mac);
      }
      break;

    case SOCKET_REQ:
      {
        socket_send(req->socket_num,req->frame,req->size);
        kfree(req->frame);
      }
      break;
  }

}

int transmit(){

  int index;
  int req_pending = transmit_que.req_pending;
  struct transmit_req* cur_req;

  if (req_pending == 0){
    return 0;
  }

  while ( req_pending > 0){
    index = req_pending - 1;
    cur_req = &(transmit_que.req_queue[index]);
    handle_transmit_request(cur_req); 
    req_pending -= 1;
  }

  transmit_que.req_pending = req_pending;

  return 1;

}

/* --------------------------INIT----------------------------- */
/*
 *
 * */
void init_network_stack(){

    uint32_t gateway_ip = 0xC0A8010A;
    uint8_t gateway_mac[MAC_ADDR_LEN] = {0xD8,0xBB,0xC1,0xF7,0xD0,0xD3};

    cpsw_init();

    phy_init();

    init_sockets();

    transmit_que.req_pending = 0;

    eth_interface.ip_addr = STATIC_IP;

    buddy();

    // clear recv cache
    cpsw_recv();

    arp_anounce();
    arp_garp();

    ping_request(gateway_ip,gateway_mac);

    int soc = socket(0,80,80,gateway_mac,gateway_ip,UDP,0);

    socket_bind(soc);

    while(1){
      cpsw_recv();
      buddy();
      uint8_t* frame = (uint8_t*) kmalloc(128);
      socket_transmit_request(soc,frame,128);
      transmit();
    }

}


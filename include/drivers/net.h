#ifndef NET_H
#define NET_H

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
#define INTC_MIR_CLEAR1_ (INTERRUPTC_BASE + 0xA8)

#define TX_INTMASK_SET (CPDMA_BASE + 0x88)
#define RX_INTMASK_SET (CPDMA_BASE + 0xA8)

#define C0_RX_EN (CPSW_WR_BASE + 0x14)
#define C0_TX_EN (CPSW_WR_BASE + 0x18)

#define CPDMA_EOI_VECTOR (CPDMA_BASE + 0x94)

#define RX_INT_STATUS_RAW (CPDMA_BASE + 0xA0)
#define TX_INT_STATUS_RAW (CPDMA_BASE + 0x80)

#define TX_INTMASK_CLEAR (CPDMA_BASE + 0x8C)

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

#define TX_INIT_FLAGS (BIT(29) | BIT(30) | BIT(31))
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
#define ETH_HEADER_SIZE 14

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

#define BUDDY_HEADER 2

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
  uint16_t src_port; // used for binding and what port to send packet form
  uint16_t dest_port;
  uint32_t dest_ip;
  uint8_t* dest_mac;
  uint8_t protocol;
  uint8_t waiting; // is it waiting for data?
  struct payload data[MAX_PENDING_PACKETS];
  uint8_t packets_pending;

};

typedef struct socket_info {
  
  uint16_t src_port; // used for binding and what port to send packet form
  uint16_t dest_port;
  uint32_t dest_ip;

} socket_info;

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

void buddy();

void net_memcopy(uint8_t* dest, uint8_t* src, int size);
void hex_dump(uint32_t* data, int len);
uint32_t htonl(uint32_t host);
uint32_t ntohl(uint32_t net);
void get_mac();
void print_ip(uint32_t ip);
void print_mac(uint8_t* mac_addr);
uint32_t strtoip(const char* str);

int init_network_stack();

void cpsw_init();
void cpsw_set_transfer(uint32_t transfer);
void cpsw_enable_gmii();
int cpsw_recv();
int cpsw_transmit(uint32_t* packet, uint32_t size);

void phy_reset();
int phy_init();

void eth_recv(uint32_t* frame, int size);
void eth_transmit(uint8_t* frame, int size, uint8_t* dest, uint16_t type);

void arp_recv(ethernet_header frame_header, uint32_t* frame, int size);
void arp_transmit(uint8_t* frame, int size, uint8_t* dest_mac, uint8_t* dether_mac, uint32_t dest_ip, uint16_t opcode);
void arp_anounce();
void arp_garp();  

uint16_t ipv4_checksum(uint8_t* ipv4_header, int size);
void ipv4_recv(ethernet_header frame_header,uint32_t* frame, int size, uint8_t* frame_ptr);
void ipv4_transmit(uint8_t* frame, uint16_t size, uint8_t protocol, uint32_t dest_ip, uint8_t* dest_mac);

void icmp_recv(ethernet_header eth_header, ipv4_header ip_header, uint32_t* frame, int size, uint8_t* frame_ptr);
void icmp_transmit(uint8_t* frame, int size, uint8_t type, uint8_t code, uint32_t data, uint32_t dest_ip, uint8_t* dest_mac);
void icmp_echo_reply(ethernet_header eth_header, ipv4_header ip_header, icmp_header icmp ,uint8_t* frame, int size);
void icmp_echo_request(uint32_t ip, uint8_t* mac);

void udp_recv(ethernet_header eth_header, ipv4_header ip_header, uint32_t* frame, int size);
void udp_transmit(uint8_t* frame, uint16_t size, uint16_t src_port, uint16_t dest_port, uint32_t dest_ip, uint8_t* dest_mac);

int socket_waiting(int socket_num, uint16_t dest_port, uint16_t bp);
int socket_store(int socket_num, uint8_t* payload, int size);
void init_sockets();  
int socket(uint32_t pid, uint8_t* dest_mac, uint8_t protocol);
int socket_bind(int socket_num, socket_info *soc_info);
int socket_free(int socket_num);
struct payload* socket_recv(int socket_num);
int socket_poll(int socket_num);
void socket_send(int socket_num, uint8_t* frame, int size);

int socket_transmit_request(int soc, uint8_t* frame, int size);

#endif

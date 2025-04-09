#include <stdint.h>
#include "memory.h"
#include "uart.h"
#include "net.h"

/*
 * cpsw.c
 *
 * This file contains the cpsw driver code to use 
 * Network capabilities of the BeagleBone Black
 * 
 * - AM335x Sitara™ Processors Common Platform Switch Driver
 * 
 * Author: Marwan Mostafa 
 *
 * */



extern ethernet_interface eth_interface;

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
  uint8_t et; // entry type

  for (index = 0; index < MAX_ALE_ENTRIES; index++){
         
	  get_ale_entry(index, ale_entry_buffer);

	  et = (((uint8_t*)(&(ale_entry_buffer[1])))[3] & (uint8_t) ALE_ENTRY_TYPE);

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
  //uart0_printf("multicast entry created\n");

  unicast_ale_entry(UNICAST_PORTMASK, eth_interface.mac_addr);
  //uart0_printf("unicast entry created\n");

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
 *  - half of state ram for rx
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

  /* Create Descriptor Chains */
  for (int i = 0; i < num_descriptors - 1; i++){
    
 	  /* Set Next Descriptor */
	  rx_cur = (cpdma_hdp*)((uint32_t) rx_start + (i * sizeof(cpdma_hdp)));
    rx_cur->next_descriptor = (cpdma_hdp*)((uint32_t) rx_cur + sizeof(cpdma_hdp));

	  /* Set Flags */
    rx_cur->flags = RX_INIT_FLAGS;

	  /* Allocate Packet Buffers */
	  rx_cur->buffer_pointer = kmalloc(MAX_PACKET_SIZE);
	  rx_cur->buffer_length = MAX_PACKET_SIZE;

  }

  tx_start->next_descriptor = 0;
  tx_start->flags = TX_INIT_FLAGS;

  eth_interface.txch.head = tx_start;
  eth_interface.txch.num_descriptors = 1;

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

  REG(INTC_MIR_CLEAR1_) = CPSW_INTMASK_CLEAR;

  REG(CPDMA_EOI_VECTOR) = EOI_TX | EOI_RX;

  REG(TX_INTMASK_SET) = CPDMA_CHANNEL_INT;
  REG(RX_INTMASK_SET) = CPDMA_CHANNEL_INT;

  REG(C0_RX_EN) = CPDMA_CHANNEL_INT;
  REG(C0_TX_EN) = CPDMA_CHANNEL_INT;

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
 * cpsw_set_transfer
 *  - sets transfer mode for port 1
 *  - use by phy driver to set link transfer speed
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

/*
 * cpsw_init()
 *  - initializes the Ethernet subsystem for the BeagleBone Black
 *  - Follow Steps outlined in Ti Manual Section 14.4.6
 *
 * */
void cpsw_init(){

  uart0_printf("Configuring CPSW this will take some time...\n");	
  phy_reset();

    //uart0_printf("Starting initialization of Common Port Switch\n");

  cpsw_select_interface();
  //uart0_printf("GMII/MII Interface Selected\n");

  cpsw_pin_mux();
  //uart0_printf("CPSW Pins Muxed\n");

  cpsw_enable_clocks();
  //uart0_printf("CPSW Clocks Enabled\n");

  cpsw_software_reset();
  //uart0_printf("CPSW Finished Software Reset\n");

  cpsw_init_cpdma_descriptors();
  //uart0_printf("CPDMA Descriptors Initialized\n");

  cpsw_config_ale();
  //uart0_printf("CPSW ALE Configured\n");

  //uart0_printf("Configuring MDIO this will take some time ...");
  cpsw_config_mdio();
  buddy();
  //uart0_printf("CPSW MDIO Configured\n");

  cpsw_config_stats();
  //uart0_printf("CPSW STATS Configured\n");

  cpsw_set_ports_state();
  //uart0_printf("CPSW Ports Set to FORWARD\n");

  cpsw_create_ale_entries();
  //uart0_printf("CPSW ALE Entries Created for Ports\n");

  cpsw_set_port_addrs();
  uart0_printf("BeagleBones MAC addr: ");
  print_mac(eth_interface.mac_addr);

  cpsw_setup_cpdma_descriptors();
  //uart0_printf("CPDMA Descriptors Setup\n");

  cpsw_enable_cpdma_controller();
  //uart0_printf("CPDMA Controller enabled\n");

  cpsw_start_recieption();
  //uart0_printf("CPSW Packet Reception Started\n");

}

/*
 * cpsw_transmit()
 *  - transmits a packet given the buffer containg the packet
 *  - lowkey packets need to be aligned to 4 bytes
 *
 * */
int cpsw_transmit(uint32_t* packet, uint32_t size){

  cpdma_hdp* tx_desc = eth_interface.txch.head;
  uint32_t len = size / sizeof(uint32_t);

  //hex_dump(packet,size);

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

  return REG(TX_INT_STATUS_RAW);

} 

/*
 * process_packet()
 * - called in cpsw_recv() to process a packet
 *
 * */
void process_packet(uint8_t* packet, int size){

  //hex_dump((uint32_t*)packet,size);

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



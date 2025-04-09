#include <stdint.h>
#include "memory.h"
#include "uart.h"
#include "net.h"

/*
 * arp.c
 *
 * This file contains the ARP code for BuddyOS 
 * Network Stack
 * 
 * - BuddyOS Network Stack: ARP 
 * 
 * Author: Marwan Mostafa 
 *
 * */


extern ethernet_interface eth_interface;

/* --------------------------ARP----------------------------- */

/*
 * arp_transmit()
 * - adds ARP header to frame
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
 * arp_anounce()
 *  - abstraction that transmits an arp broadcast packet
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
 * arp_garp()
 * - abstraction that transmits and ARP gratuitous packet
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
 * arp_reply()
 *  - transmits an arp reply given an arp request has been recieved
 *
 * */
int arp_reply(arp_header arp_request){

    uint8_t* packet = (uint8_t*) kmalloc(128);

    if (arp_request.dest_ip != STATIC_IP) return -1;

    //uart0_printf("\nReplying to Arp Request\n");
    arp_transmit(packet,128,arp_request.src_mac,arp_request.src_mac,arp_request.src_ip,ARP_REPLY);

    kfree(packet);

}

/*
 * arp_recv()
 * - extracts ARP header from frame
 *  
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
	      // TODO: Create an ARP Table
	    }
	    break;

	    default:
	    {
	    
	    }
    }

}



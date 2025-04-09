#include <stdint.h>
#include "memory.h"
#include "uart.h"
#include "net.h"

/*
 * ethernet.c
 *
 * This file contains the Ethernet Layer code for BuddyOS 
 * Network Stack
 * 
 * - BuddyOS Network Stack: Ethernet Layer
 * 
 * Author: Marwan Mostafa 
 *
 * */



extern ethernet_interface eth_interface;

/* --------------------------ETHERNET----------------------------- */

/*
 * eth_transmit()
 *  - Adds Ethernet II header to frame
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

/*
 * eth_recv()
 *  - takes raw frame and extracts Ethernet II header
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

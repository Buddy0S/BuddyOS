#include <stdint.h>
#include "memory.h"
#include "uart.h"
#include "net.h"

/*
 * icmp.c
 *
 * This file contains the ICMP code for BuddyOS 
 * Network Stack
 * 
 * - BuddyOS Network Stack: ICMP 
 * 
 * Author: Marwan Mostafa 
 *
 * */



extern ethernet_interface eth_interface;

/* --------------------------ICMP----------------------------- */

/*
 * icmp_recv()
 * - extracts icmp header from frame
 *
 * */
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

/*
 * icmp_transmit()
 *  - adds icmp header to frame
 *
 * */
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

/*
 * icmp_echo_reply()
 * - abstraction to transmit an icmp echo reply
 *
 * */
void icmp_echo_reply(ethernet_header eth_header, ipv4_header ip_header, icmp_header icmp ,uint8_t* frame, int size){

  size = size + IPV4_HEADER_SIZE + ETH_HEADER_SIZE;

  icmp_transmit(frame, size, ECHO_REPLY, ECHO_REPLY_CODE, icmp.data, ip_header.src_ip, eth_header.source_mac);

}

/*
 * icmp_echo_request()
 *  - abstraction to transmit icmp echo request
 *
 * */
void icmp_echo_request(uint32_t ip, uint8_t* mac){

  uint8_t* packet = (uint8_t*) kmalloc(ICMP_PACKET_SIZE);

  uart0_printf("Pinging ");
  print_ip(ip);
  icmp_transmit(packet, ICMP_PACKET_SIZE, ECHO_REQUEST, ECHO_REQUEST_CODE, 0, ip, mac);

  kfree(packet);
}



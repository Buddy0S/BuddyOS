#include <stdint.h>
#include "memory.h"
#include "uart.h"
#include "net.h"

/*
 * ipv4.c
 *
 * This file contains the IPV4 code for BuddyOS 
 * Network Stack
 * 
 * - BuddyOS Network Stack: IPV4 
 * 
 * Author: Marwan Mostafa 
 *
 * */



extern ethernet_interface eth_interface;

/* --------------------------IPV4----------------------------- */

/*
 * https://tools.ietf.org/html/rfc1071
 * modified to work for 4 byte aligned Access
 *
 * ipv4_checksum()
 *  - computes checksum given a header
 *
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

/*
 * ipv4-recv()
 *  - extracts ipv4 header from frame
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
 * ipv4_transmit()
 *  - adds ipv4 header to frame
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



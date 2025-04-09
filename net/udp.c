#include <stdint.h>
#include "memory.h"
#include "uart.h"
#include "net.h"

/*
 * udp.c
 *
 * This file contains the UDP code for BuddyOS 
 * Network Stack
 * 
 * - BuddyOS Network Stack: UDP 
 * 
 * Author: Marwan Mostafa 
 *
 * */



extern ethernet_interface eth_interface;

/* --------------------------UDP------------------------------ */

/*
 * udp_recv()
 *  - extracts udp header from frame
 *
 * */
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
      socket_store(i, (uint8_t*) udp.payload,payload_length);
    }
  }

}

/*
 * udp checksum uses pseudo header to compute checksum
 * udp_checksum()
 * - computes checksum for udp header
 *
 * */
uint16_t udp_checksum(uint8_t* frame, uint32_t src_ip, uint32_t dest_ip, uint16_t size, uint16_t src_port, uint16_t dest_port){

  uint16_t pseudo_len = size;

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

  pseudo_packet[10] = (size & 0xFF00) >> 8;
  pseudo_packet[11] = size & 0x00FF;

  pseudo_packet[12] = (src_port & 0xFF00) >> 8;
  pseudo_packet[13] = src_port & 0x00FF;

  pseudo_packet[14] = (dest_port & 0xFF00) >> 8;
  pseudo_packet[15] = dest_port & 0x00FF;

  pseudo_packet[16] = (size & 0xFF00) >> 8;
  pseudo_packet[17] = size & 0x00FF;

  pseudo_packet[18] = 0;
  pseudo_packet[19] = 0;

  net_memcopy(&(pseudo_packet[20]),&(frame[42]), size - UDP_HEADER_SIZE);

  return ipv4_checksum(pseudo_packet, pseudo_len);
}

/*
 * udp_transmit()
 * - adds udp header to frame
 *
 * */
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



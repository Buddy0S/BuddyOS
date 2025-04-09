#include <stdint.h>
#include "memory.h"
#include "uart.h"
#include "net.h"

/*
 * socket.c
 *
 * This file contains the socket code for BuddyOS 
 * Network Stack
 * 
 * - BuddyOS Network Stack: Sockets
 * 
 * Author: Marwan Mostafa 
 *
 * */



extern ethernet_interface eth_interface;
extern struct socket socket_table[MAX_SOCKETS];

/* --------------------------SOCKETS-------------------------- */

/*
 * socket_waiting()
 * - checks if a socket is waiting for packets on a port
 *
 * */
int socket_waiting(int socket_num, uint16_t dest_port, uint16_t bp){

  struct socket soc = socket_table[socket_num];

  if (!soc.waiting){
    return 0;
  }

  if (soc.src_port != dest_port){
    return 0;
  }

  return 1;

}

/*
 * socket_store()
 *  - stores packet in sockets recv queue
 *  - drops packets if pending packet buffer is full
 *
 * */
int socket_store(int socket_num, uint8_t* payload, int size){

  uint8_t* buffer;
  struct socket soc = socket_table[socket_num];

  if (soc.packets_pending >= MAX_PENDING_PACKETS){
    return -1;
  }

  // adjusting to regain first 2 bytes of payload
  payload = (uint8_t*)((uint32_t) payload - sizeof(uint8_t) * 2);

  buffer = (uint8_t*) kmalloc(size);

  for (int i = 0; i < size; i++){
    buffer[i] = payload[i]; 
  }

  socket_table[socket_num].data[soc.packets_pending].data = (uint8_t*) buffer;
  socket_table[socket_num].data[soc.packets_pending].size = size;

  socket_table[socket_num].packets_pending += 1; 
  
  //uart0_printf("Socket %d Recieved Packet\n", socket_num);

  return 1;

}

/*
 * init socket table
 *
 * */
void init_sockets(){

  for(int i = 0; i < MAX_SOCKETS; i++){
    socket_table[i].free = 1;
    socket_table[i].pid = 0;
    socket_table[i].src_port = 0;
    socket_table[i].dest_port = 0;
    socket_table[i].dest_ip = 0;
    socket_table[i].dest_mac = 0;
    socket_table[i].protocol = 0;
    socket_table[i].waiting = 0;
    socket_table[i].packets_pending = 0;
  }

}

/*
 * finds free socket in socket table
 *
 * */
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
int socket(uint32_t pid, uint8_t* dest_mac, uint8_t protocol){

  int socket_num = find_free_socket();

  if (socket_num == -1) return -1;

  socket_table[socket_num].free = 0;

  uint8_t* copymac = (uint8_t*) kmalloc(MAC_ADDR_LEN);
  net_memcopy(copymac,dest_mac,MAC_ADDR_LEN);

  socket_table[socket_num].pid = pid;
  socket_table[socket_num].dest_mac = copymac;
  socket_table[socket_num].protocol = protocol;

  return socket_num;

}


/*
 * frees socket in socket table
 *
 * */
int socket_free(int socket_num){
  
  kfree(socket_table[socket_num].dest_mac);

  socket_table[socket_num].free = 1;
  socket_table[socket_num].pid = 0;
  socket_table[socket_num].src_port = 0;
  socket_table[socket_num].dest_port = 0;
  socket_table[socket_num].dest_ip = 0;
  socket_table[socket_num].dest_mac = 0;
  socket_table[socket_num].protocol = 0;
  socket_table[socket_num].waiting = 0;
  socket_table[socket_num].packets_pending = 0;

}

/*
 * set socket as waiting for data on a port
 *
 * */
int socket_bind(int socket_num, socket_info *soc_info){

  socket_table[socket_num].waiting = 1;
  socket_table[socket_num].src_port = soc_info->src_port;

  return 1;

}

/*
 * checks to see if socket has a pending packet
 *
 * */
int socket_poll(int socket_num){

  int stat = socket_table[socket_num].packets_pending;

  if (stat) return 1;

  return 0;

}

/*
 * returns pointer first payload in sockets recv queue
 *
 * */
struct payload* socket_recv(int socket_num){

  uint8_t data_index = socket_table[socket_num].packets_pending - 1;

  struct payload *data = (struct payload *) kmalloc(sizeof(struct payload));

  data->data = socket_table[socket_num].data[data_index].data;
  data->size = socket_table[socket_num].data[data_index].size;

  socket_table[socket_num].packets_pending -= 1;

  return data;

}

/*
 * uses socket info to transmit a packet
 *
 * */
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

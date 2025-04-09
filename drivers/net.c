#include <stdint.h>
#include "memory.h"
#include "uart.h"
#include "net.h"

/*
 * net.c
 *
 * This file contains the upper half driver code to use 
 * Network capabilities of the BeagleBone Black
 * 
 * - BuddyOS Upper Half Network Driver
 * 
 * Author: Marwan Mostafa 
 *
 * */

ethernet_interface eth_interface;

struct socket socket_table[MAX_SOCKETS];

struct transmit_queue transmit_que;

int net_ready = 0;

/* --------------------------UPPERHALF--------------------------- */

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

/*
 * abstraction to create a socket transmit request
 *
 * */
int socket_transmit_request(int socket_num, uint8_t* frame, int size){

  return transmit_request(frame, size, SOCKET_REQ, socket_num, 0, 0);

}

/*
 * abstraction to create "icmp echo request" request
 * */
int ping_request(uint32_t ip, uint8_t* mac){

  uint8_t* mac_copy = (uint8_t*) kmalloc(MAC_ADDR_LEN);

  net_memcopy(mac_copy, mac, MAC_ADDR_LEN);

  return transmit_request(0,0,PING_REQ,0,ip,mac_copy);

}

/*
 * handles request based on request type
 *
 * */
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

/*
 * processes transmit request queue
 *
 * */
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

/*
 * Run this on every timer interrupt
 * */
void timer_net_isr(){
  if (net_ready){
    cpsw_recv();
    transmit();
  }
}

/* --------------------------INIT----------------------------- */
/*
 *
 * */
int init_network_stack(){

    // 192.168.1.10
    uint32_t gateway_ip = 0xC0A8010A;
    uint8_t gateway_mac[MAC_ADDR_LEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    cpsw_init();

    if (phy_init() == -1) return -1; 

    init_sockets();

    transmit_que.req_pending = 0;

    // 192.168.1.20
    eth_interface.ip_addr = STATIC_IP;

    uart0_printf("Beagle Bones ");
    print_ip(STATIC_IP);

    buddy();

    // clear recv cache
    cpsw_recv();

    arp_anounce();
    arp_garp();

    net_ready = 1;

    ping_request(gateway_ip,gateway_mac);

}


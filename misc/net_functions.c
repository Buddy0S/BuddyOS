#include <stdint.h>
#include "memory.h"
#include "uart.h"
#include "net.h"

extern ethernet_interface eth_interface;

/* --------------------------HELPER----------------------------- */

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
 * print_mac()
 * - prints ethernet interfaces mac address
 *
 * */
void print_mac(uint8_t* mac_addr){

    get_mac();
    uart0_printf("%x:%x:%x:%x:%x:%x\n",
		    mac_addr[0],
		    mac_addr[1],
		    mac_addr[2],
		    mac_addr[3],
		    mac_addr[4],
		    mac_addr[5]);
}

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

uint32_t str_to_byte(const char* str, int start, int end) {
    int index, multiplier;
    uint32_t byte;

    multiplier = 1;
    byte = 0;

    for (index = end; index >= start; index--) {
        byte += (str[index] - 48) * multiplier;
        multiplier *= 10;
    }

    return byte;
}

uint32_t strtoip(const char* str) {
    int p1, p2, p3, end;
    uint32_t ip;

    ip = 0;

    /* finding all the periods */
    for (p1 = 0; str[p1] != '.'; p1++) {}
    for (p2 = p1+1; str[p2] != '.'; p2++) {}
    for (p3 = p2+1; str[p3] != '.'; p3++) {}
    for (end = p3+1; str[end] != '\0'; end++) {}

    ip += str_to_byte(str, 0, p1-1) << 24;
    ip += str_to_byte(str, p1+1, p2-1) << 16;
    ip += str_to_byte(str, p2+1, p3-1) << 8;
    ip += str_to_byte(str, p3+1, end -1);

    return ip;
}

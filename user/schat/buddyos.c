#include "socketAB.h"
#include "syscall.h"

#define SYS_PID 0
#define SEND_PORT 8888

int INIT_Sockets(){
  return 0;
}

int CLOSE_Socket(int socket){
  return __closesocket(socket);
}

int CLEANUP_Sockets(){
  return 0;
}

int UDP_Socket(){

  uint8_t gateway_mac[MAC_ADDR_LEN] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

  return __socket(SYS_PID, gateway_mac, UDP);

}

int Poll_Socket(int socket_num){
  return __poll(SOCKET, socket_num);
}

void Clear_Stdin(){
  // do nothing
}

int Poll_Stdin(){
  return __poll(STD, STDIN);
}

// only works on non negative
int atoi(char *s) {
    int sum = 0;
    while((*s >= '0')&&(*s <= '9')) {
      sum = sum * 10;
      sum = sum + (*s - 48);
      s++;
    }
    return (sum);
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

int UDP_Sendto(int socket, int destport, char* destip, char* sendbuf, int sendsize){

  uint32_t dest_ip = strtoip(destip);
  socket_info soc_info;

  soc_info.src_port = SEND_PORT;
  soc_info.dest_port = destport;
  soc_info.dest_ip = dest_ip;

  return __sendto(socket, sendbuf, sendsize, &soc_info);

}

int UDP_Bind(int socket, int port, char* interface){

  socket_info soc_info;

  soc_info.src_port = port;

  return __bind(socket, &soc_info);

}

int UDP_Recvfrom(int socket, char* buff, int buffsize){

  return __recvfrom(socket, (uint8_t*) buff);

}



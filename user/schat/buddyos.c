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
int _atoi(char *s) {
    int sum = 0;
    while((*s >= '0')&&(*s <= '9')) {
      sum = sum * 10;
      sum = sum + (*s - 48);
      s++;
    }
    return (sum);
}

uint32_t strtoip(char* ip) {
    uint8_t iph[4] = {0};
    int ip_index = 0;
    int temp = 0;
    char buf[4] = {0};  

    for (int i = 0; ip[i] != '\0'; i++) {
        if (ip[i] == '.') {
            buf[temp] = '\0';
            iph[ip_index] = (uint8_t) _atoi(buf);
            ip_index++;
            temp = 0;
        } else {
            buf[temp++] = ip[i];
        }
    }

    buf[temp] = '\0';
    if (ip_index < 4) {
        iph[ip_index] = (uint8_t) _atoi(buf);
    }

    uint32_t result = ((uint32_t)iph[0] << 24) |
                      ((uint32_t)iph[1] << 16) |
                      ((uint32_t)iph[2] << 8)  |
                      (uint32_t)iph[3];
    return result;
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



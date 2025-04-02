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


uint32_t strtoip(char* ip){

  char buf[5];
  uint8_t iph[4];
  int temp = 0;
  int ip_index = 0;

  for (int i = 0; i < 18; i++){
    if (ip[i] == '.'){
      buf[temp] = '\0';
      temp = 0;
      iph[ip_index] = (uint8_t) _atoi(buf);
      ip_index++;
    }else{
      buf[temp] = ip[i];
      temp++;
    }

    if (ip_index == 4) break;

  }

  return (uint32_t) iph;

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



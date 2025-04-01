#include "socketAB.h"
#include "syscalls.h"

#define SYS_PID 0
#define SEND_PORT 8888

int INIT_Sockets(
  return 0;
)

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

int UDP_Sendto(int socket, int destport, void* destip, char* sendbuf, int sendsize){

  uint32_t dest_ip = *((uint32_t*) destip);
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



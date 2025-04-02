#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

int INIT_Sockets(){
  return 0;
}

int CLOSE_Socket(int socket){

  if (close(socket) == -1){
    perror("Socket Closing Error");
    return -1;
  }

  return 0;

}

int CLEANUP_Sockets(){
  return 0;
}

int UDP_Socket(){
  int soc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  
  if (soc == -1){
    perror("Socket Creation Error");
    return -1;
  }
   
  return soc;

}

int Poll_Socket(int socket_num){

  struct pollfd pfd;
  
  pfd.fd = socket_num;
  pfd.events = POLLIN;

  int ret = poll(&pfd, 1, 100);

  if (ret > 0 && (pfd.revents & POLLIN)) return 1;

  return 0;

}

int Poll_Stdin(){

  struct pollfd pfd;

  pfd.fd = STDIN_FILENO;
  pfd.events = POLLIN;

  int ret = poll(&pfd, 1, 100);

  if (ret > 0) return 1;

  return 0;
}

int UDP_Sendto(int socket, int destport, char* destip, char* sendbuf, int sendsize){
    
  struct sockaddr_in outinfo;
    
  memset(&outinfo, 0, sizeof(outinfo));
    
  outinfo.sin_family = AF_INET;
  outinfo.sin_port = htons(destport);
  outinfo.sin_addr.s_addr = inet_addr(destip);
    
  return sendto(socket, sendbuf, sendsize, 0, (struct sockaddr *) &outinfo, sizeof(outinfo));

}

int UDP_Bind(int socket, int port){
    
  struct sockaddr_in srcinfo;
    
  memset(&srcinfo, 0, sizeof(srcinfo));
    
  srcinfo.sin_family = AF_INET;
  srcinfo.sin_port = htons(port);
  srcinfo.sin_addr.s_addr = htonl(INADDR_ANY);
    
  if (bind(socket, (struct sockaddr *) &srcinfo, sizeof(srcinfo)) == -1){
    perror("Socket Bind Error");
    return -1;
  }
    
  return 0;

}

int UDP_Recvfrom(int socket, char* buff, int buffsize){
    
  struct sockaddr_in srcinfo;
  socklen_t srcinfosize = sizeof(srcinfo);
  
  return recvfrom(socket, buff, buffsize, 0, (struct sockaddr *)&srcinfo, &srcinfosize);

}


#include "socketAB.h"
#include <winsock2.h>
#include <stdio.h>

WSADATA wsaData;

int INIT_Sockets(){
  
  int iResult;

  iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != 0) {
    printf("WSAStartup failed: %d\n", iResult);
    WSACleanup();
    return -1;
  }

  return 0;

}

int CLOSE_Socket(int socket){

  int status = closesocket((SOCKET) socket);

  if (status != 0){
    printf("Socket Closing Error %ld\n", WSAGetLastError());
    return -1;
  }

  return 0;

}

int CLEANUP_Sockets(){

  int status = WSACleanup();

  if (status != 0){
    printf("Socket Cleanup Error %ld\n", WSAGetLastError());
    return -1;
  }

  return 0;

}

int UDP_Socket(){

  SOCKET soc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (soc == INVALID_SOCKET){
    printf("Socket Creation Error %ld\n", WSAGetLastError());
    WSACleanup();
    return -1;
  }

  return (int) soc;
}

int UDP_Sendto(int socket, int destport, void* destip, char* sendbuf, int sendsize){

  SOCKADDR_IN outinfo;

  outinfo.sin_family = AF_INET;
  outinfo.sin_port = htons(destport);
  outinfo.sin_addr.s_addr = inet_addr((char*)destip);

  return sendto((SOCKET) socket, sendbuf, sendsize, 0 , (SOCKADDR *) &outinfo, sizeof(outinfo));

}

int UDP_Bind(int socket, int port){

  SOCKADDR_IN srcinfo;
  int status = sizeof(srcinfo);

  srcinfo.sin_family = AF_INET;
  srcinfo.sin_port = htons(port);
  srcinfo.sin_addr.s_addr = htonl(INADDR_ANY);

  status = bind((SOCKET) socket, (SOCKADDR *) &srcinfo, sizeof(srcinfo));

  if (status == SOCKET_ERROR){
    printf("Socket Bind Error %ld\n", WSAGetLastError());
    
    return -1;
  }

  return 0;

}

int UDP_Recvfrom(int socket, char* buff, int buffsize){

  SOCKADDR_IN srcinfo;
  int srcinfosize;

  return recvfrom((SOCKET) socket, buff, buffsize, 0,(SOCKADDR *)&srcinfo, &srcinfosize);

}




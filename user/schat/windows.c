#define _WIN32_WINNT 0x0A00  // Windows 10/11

#include "socketAB.h"
#include <winsock2.h>
#include <stdio.h>
#include <ws2tcpip.h>
#include <windows.h>

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

int Poll_Socket(int socket_num){

  WSAPOLLFD pfd;
  pfd.fd = (SOCKET) socket_num;
  pfd.events = POLLRDNORM;

  int ret = WSAPoll(&pfd, 1, 100);

  if (ret > 0 && (pfd.revents & POLLRDNORM)) return 1;

  return 0;

}

void Clear_Stdin(){

  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

  FlushConsoleInputBuffer(hStdin);

}

int Poll_Stdin(){

  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
  DWORD numEvents;
  
  GetNumberOfConsoleInputEvents(hStdin, &numEvents);

  return numEvents > 0;

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

int UDP_Sendto(int socket, int destport, char* destip, char* sendbuf, int sendsize){

  SOCKADDR_IN outinfo;

  outinfo.sin_family = AF_INET;
  outinfo.sin_port = htons(destport);
  outinfo.sin_addr.s_addr = inet_addr(destip);

  return sendto((SOCKET) socket, sendbuf, sendsize, 0 , (SOCKADDR *) &outinfo, sizeof(outinfo));

}

int UDP_Bind(int socket, int port, char* intface){

  SOCKADDR_IN srcinfo;
  int status;

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
  int srcinfosize = sizeof(srcinfo);

  return recvfrom((SOCKET) socket, buff, buffsize, 0,(SOCKADDR *)&srcinfo, &srcinfosize);

}




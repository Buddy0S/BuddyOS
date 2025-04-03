#include "socketAB.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_USERNAME 12
#define TEMP_BUF_SIZE 20
#define MAX_IP 18
#define STDIN_BUFF 50
#define BUDDY_HEADER 2

char username[MAX_USERNAME];

void sendmsg(int send_socket, int dest_port, char* destip);
void recvmsg(int recv_socket);
char *strcopy(char *dest, char *src);
void rmnewline(char *str);

int main(){
 
  uint16_t bind_port;
  uint16_t dest_port;
  char ip[MAX_IP];
  char buf[TEMP_BUF_SIZE];
  int recv_socket;
  int send_socket;

  printf("Enter Username: ");
  fgets(username, MAX_USERNAME, stdin);
  rmnewline(username);

  printf("What Port do you want to bind on: ");
  fgets(buf, TEMP_BUF_SIZE, stdin);
  bind_port = (uint16_t) atoi(buf);

  printf("What Port do you want to send to: ");
  fgets(buf, TEMP_BUF_SIZE, stdin);
  dest_port = (uint16_t) atoi(buf);

  printf("What IP do you want to send to: ");
  fgets(ip, MAX_IP, stdin);

  INIT_Sockets();

  recv_socket = UDP_Socket();
  send_socket = UDP_Socket();

  UDP_Bind(recv_socket, bind_port, "0.0.0.0");

  Clear_Stdin();

  while(1){

    if(Poll_Stdin()){
      sendmsg(send_socket, dest_port, ip);
    }

    if(Poll_Socket(recv_socket)){
      recvmsg(recv_socket);
    }

  }

  CLOSE_Socket(recv_socket);
  CLOSE_Socket(send_socket);

  CLEANUP_Sockets();

  return 0;

}

void net_memcopy(char* dest, char* src, int size){

  for (int i = 0; i < size ; i++){
    dest[i] = src[i];
  }
}

char *strcopy(char *dest, char *src) {
	char *dst = dest;
	
	while (*src != '\0') {
		*dst++ = *src++;
	}
	*dst = '\0';

	return dest;
}

void rmnewline(char *str){
	
	char *dst = str;
	
	while (*dst != '\0') {
		dst++;
		if (*dst == '\n') *dst = '\0'; 
	}
}

void sendmsg(int send_socket, int dest_port, char* destip){

  int size = BUDDY_HEADER + MAX_USERNAME + STDIN_BUFF;
  char* buff = (char*) malloc(STDIN_BUFF);

  fgets(buff,STDIN_BUFF, stdin);

  char* payload = (char*) malloc(size);

  payload[0] = (char) 0;
  payload[1] = (char) 0;

  net_memcopy(&(payload[2]),username, MAX_USERNAME);

  net_memcopy(&(payload[BUDDY_HEADER + MAX_USERNAME]), buff, STDIN_BUFF);

  free(buff);

  UDP_Sendto(send_socket, dest_port, destip, payload, size);

  free(payload);

  Clear_Stdin();

}

void recvmsg(int recv_socket){

  int size = BUDDY_HEADER + MAX_USERNAME + STDIN_BUFF;
  char* buff = (char*) malloc(size);
  char susername[MAX_USERNAME];

  UDP_Recvfrom(recv_socket, buff, size);

  strcopy(susername, &(buff[2]));

  printf("%s: %s\n", susername, &(buff[BUDDY_HEADER + MAX_USERNAME]));

  free(buff);

}






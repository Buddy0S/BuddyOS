#include "socketAB.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_USERNAME 12
#define TEMP_BUF_SIZE 20
#define MAX_IP 18

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

int main(){

  char username[MAX_USERNAME];
  uint16_t bind_port;
  uint16_t dest_port;
  char ip[MAX_IP];
  char buf[TEMP_BUF_SIZE];

  printf("Enter Username: ");
  fgets(username, MAX_USERNAME, stdin);

  printf("What Port do you want to bind on: ");
  fgets(buf, TEMP_BUF_SIZE, stdin);
  bind_port = (uint16_t) atoi(buf);

  printf("What Port do you want to send to: ");
  fgets(buf, TEMP_BUF_SIZE, stdin);
  dest_port = (uint16_t) atoi(buf);

  printf("What IP do you want to send to: ");
  fgets(ip, MAX_IP, stdin);

  return 0;
}




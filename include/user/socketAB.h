#ifndef SOCKETAB_H
#define SOCKETAB_H

int INIT_Sockets();
int CLOSE_Socket(int socket);
int CLEANUP_Sockets();

int Poll_Socket(int socket_num);

void Clear_Stdin();
int Poll_Stdin();

int UDP_Socket();
int UDP_Sendto(int socket, int destport, char* destip, char* sendbuf, int sendsize);
int UDP_Bind(int socket, int port, char* interface);
int UDP_Recvfrom(int socket, char* buff, int buffsize);

#endif

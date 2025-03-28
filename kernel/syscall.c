#include <stdint.h>
#include "memory.h"
#include "uart.h"
#include "syscall.h"
#include "proc.h"
#include "net.h"

extern PCB pcb;
extern int current_index;

/* test function that calls a syscall that takes 2 arguments */
int __syscalltest(int a, int b) {
    return SYSCALL(SYSCALL_TEST_2_ARGS_NR);
}

int __yield(void) {
    return SYSCALL(SYSCALL_YIELD_NR);
}

int __send_end(void* reply, uint32_t* rlen) {
    return SYSCALL(SYSCALL_SEND_END);
}

int __send_start(int pid, struct Mail* mail_in, void* reply, uint32_t* rlen) {
    return SYSCALL(SYSCALL_SEND_NR);
}

int __send(int pid, void *msg, uint32_t len, void* reply, uint32_t* rlen) {
    int result;
    struct Mail mail_in = {
        .msg = msg,
        .len = len
    };
    result = __send_start(pid, &mail_in, reply, rlen);
    if (result != 0) {
        uart0_printf("send failed: %d\n", result);
        return result;
    }

    return __send_end(reply, rlen);
}

int __receive_end(int* author, void* msg, uint32_t* len) {
    return SYSCALL(SYSCALL_RECEIVE_END);
}

int __receive_start(int* author, void* msg, uint32_t* len) {
    return SYSCALL(SYSCALL_RECEIVE_NR);
}

int __receive(int* author, void* msg, uint32_t* len) {
    int result;
    result = __receive_start(author, msg, len);
    if (result != 0) {
        return result;
    }

    return __receive_end(author, msg, len);
}

int __reply(int pid, void* msg, uint32_t len) {
    return SYSCALL(SYSCALL_REPLY_NR);
}

int __msg_waiting() {
    return SYSCALL(SYSCALL_MSG_WAITING_NR);
}

int __fork() {
	return SYSCALL(SYSCALL_FORK_NR);
}

void __exit() {
	SYSCALL(SYSCALL_EXIT_NR);
}

int __socket(struct socket* soc){
  return SYSCALL(SYSCALL_SOCKET_NR);
}

int __socket_bind(int soc){
  return SYSCALL(SYSCALL_SOCKET_BIND_NR);
}

int __socket_unbind(int soc){
  return SYSCALL(SYSCALL_SOCKET_UNBIND_NR);
}

struct payload* __socket_recv(int soc){
  return (struct payload*) SYSCALL(SYSCALL_SOCKET_RECV_NR);
}

int __socket_request(int soc, uint8_t* frame, int size){
  return SYSCALL(SYSCALL_SOCKET_REQUEST_NR);
}



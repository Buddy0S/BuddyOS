#include "syscall.h"
#include <stdio.h>
#include <stdlib.h>

void process1(void);

int main(void) {
	process1();
}

void process1(void) {
    int sender_pid;
    char msg[20];
    uint32_t len;
    int pid = 3; 

    printf("Process 1 (Consumer) started. PID: %d\n", pid);

    for (int i = 0; i < 10; ++i) {
        printf("\nConsumer PID %d: Waiting for a message...\n", pid);
        len = 20;  // reset length before each receive

        if (__receive(&sender_pid, msg, &len) == 0) {
            printf("Consumer PID %d: Received '%s' from sender PID %d\n", 
                         pid, msg, sender_pid);
            if (__reply(sender_pid, "ACK", 4) == 0) {
                printf("Consumer PID %d: Replied 'ACK' to sender PID %d\n", 
                             pid, sender_pid);
            } else {
                printf("Consumer PID %d: Failed to reply to sender PID %d\n", 
                             pid, sender_pid);
            }
        } else {
            printf("Consumer PID %d: Error receiving message\n", pid);
        }
        /* Im use a longer delay to simulate a slow consumer */
	for (int i = 0; i < 1000000; ++i) {}
    }
}


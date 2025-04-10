/* CMPT432 - ImplementationTeam00 */

#include "syscall.h"
#include <stdio.h>
#include <stdlib.h>

void process2(void);

int main(void) {
	process2();
}



void process2(void) {
    char message1[20] = "Hello buddy #1";
    char message2[20] = "Hello buddy #2";
    char response[20];
    uint32_t rsp_len;
    int consumer_pid = __f_exec("/home/BUD.BIN");  // process 1 pid better be 1 man
    int pid = 2;

    printf("Process 2 (Producer) started. PID: %d\n", pid);

    for (int i = 0; i < 5; ++i) {
        printf("\nProducer PID %d: Preparing to send messages to Consumer PID %d\n",
                     pid, consumer_pid);

        /* Send first message */
        rsp_len = 20;
        printf("Producer PID %d: Sending: '%s'\n", pid, message1);
        if (__send(consumer_pid, message1, 20, response, &rsp_len) == 0) {
            printf("Producer PID %d: Received reply: '%s'\n", pid, response);
        } else {
            printf("Producer PID %d: Failed to send message 1\n", pid);
        }

        /* Send second message immediately after the first */
        rsp_len = 20;
        printf("Producer PID %d: Sending: '%s'\n", pid, message2);
        if (__send(consumer_pid, message2, 20, response, &rsp_len) == 0) {
            printf("Producer PID %d: Received reply: '%s'\n", pid, response);
        } else {
            printf("Producer PID %d: Failed to send message 2\n", pid);
        }

        /* Short delay between the bursts of two messages */
	for (int i = 0; i < 1000000; ++i) {}
    }
}

void process1(void) {
    int sender_pid;
    char msg[20];
    uint32_t len;
    int pid = current_process->pid; 

    uart0_printf("Process 1 (Consumer) started. PID: %d\n", pid);

    while (1) {
        uart0_printf("\nConsumer PID %d: Waiting for a message...\n", pid);
        len = 20;  // reset length before each receive

        if (__receive(&sender_pid, msg, &len) == 0) {
            uart0_printf("Consumer PID %d: Received '%s' from sender PID %d\n", 
                         pid, msg, sender_pid);
            if (__reply(sender_pid, "ACK", 4) == 0) {
                uart0_printf("Consumer PID %d: Replied 'ACK' to sender PID %d\n", 
                             pid, sender_pid);
            } else {
                uart0_printf("Consumer PID %d: Failed to reply to sender PID %d\n", 
                             pid, sender_pid);
            }
        } else {
            uart0_printf("Consumer PID %d: Error receiving message\n", pid);
        }
        /* Im use a longer delay to simulate a slow consumer */
        delay();
        delay();
    }
}


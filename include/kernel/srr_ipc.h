/* CMPT432 - ImplementationTeam00 */

/* 
 * Defines the FIFO mailbox used for the blocking send, receive, and reply
 * IPC.
 */

#ifndef KERNEL_SRR_H
#define KERNEL_SRR_H

#include <stdint.h>
#include <list.h>
#include <stdint.h>

struct Mail {
    void *msg;
    uint32_t len;
};

struct MailEntry {
    struct Mail data;
    int author;
    KNode node;
};

struct SRRMailbox {
    struct KList mail;
    int count;
    int sent_to;
    struct Mail reply;
};

static inline void srr_init_mailbox(struct SRRMailbox *mb) {
    list_init(&mb->mail);
    mb->count = 0;
    mb->sent_to = 0;
    mb->reply.msg = NULL;
    mb->reply.len = 0;
}

int send(int pid, struct Mail* mail_in, void* reply, uint32_t* rlen);

int send_end(void* reply, uint32_t* rlen);

int receive(int* author, void* msg, uint32_t* len);

int receive_end(int* author, void* msg, uint32_t* len);

int reply(int pid, void* msg, uint32_t len);

int msg_waiting();
#endif

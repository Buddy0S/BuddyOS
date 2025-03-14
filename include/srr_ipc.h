/* 
 * Defines the FIFO mailbox used for the blocking send, receive, and reply
 * IPC.
 */

#ifndef KERNEL_SRR_H
#define KERNEL_SRR_H

#include <list.h>
#include <stdint.h>

struct Mail {
    void *msg;
    uint32_t len;
}

struct MailEntry {
    struct Mail data;
    int author;
    KNode node;
}

struct SRRMailbox {
    struct KListp mail;
    int count;
    int sent_to;
    struct Mail reply;
}

static inline void srr_init_mailbox(struct SRRMailbox *mb) {
    list_init(&mb->mail);
    mb->count = 0;
    int sent_to = 0;
    mb->reply.msg = NULL;
    mb->reply.len = 0;
}

#endif

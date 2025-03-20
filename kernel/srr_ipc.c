#include <srr_ipc.h>
#include <stdint.h>
#include <syserr.h>
#include <proc.h>
#include <memory.h>
#include <memcpy.h>

int send(int pid, void *msg, uint32_t len, void* reply, uint32_t* rlen) {
    struct SRRMailbox *r_box, *c_box;
    struct MailEntry* new_mail;
    PCB* receiver;
    void* data;
    uint32_t usable_len;
    
    /* input and state checking */
    if (msg == NULL || reply == NULL ||rlen == NULL) {
        return -EFAULT;
    }

    if (len == 0 || *rlen == 0) {
        return -EINVAL;
    }

    receiver = get_PCB(pid);
    if (receiver == NULL) {
        return -ESRCH;
    } else if (current_process == receiver) {
        return -EINVAL;
    }

    /* This would result in both processes waiting for each other */
    r_box = &receiver->mailbox;
    c_box = &current_process->mailbox;
    if (r_box->sent_to == current_process->pid) {
        return -EDEADLK;
    }

    data = kmalloc(len);
    if (data == NULL) {
        return -ENOMEM;
    }

    new_mail = kmalloc(sizeof(struct MailEntry));
    if (new_mail == NULL) {
        kfree(data);
        return -ENOMEM;
    }

    bmemcpy(data, msg, len);
    new_mail->data.msg = data;
    new_mail->data.len = len;
    new_mail->author = current_process->pid;

    list_add_tail(&r_box->mail, &new_mail->node);
    r_box->count++;

    c_box->sent_to = pid;

    wake_proc(pid);

    block();

    /* By now the reply has been put in */

    usable_len = c_box->reply.len > *rlen ? *rlen : c_box->reply.len;
    bmemcpy(reply, c_box->reply.msg, usable_len);
    *rlen = usable_len;

    kfree(c_box->reply.msg);
    c_box->reply.msg = NULL;
    c_box->sent_to = 0;
    
    return 0;
}

int receive(int* author, void* msg, uint32_t* len) {
    struct SRRMailbox* mb;
    uint32_t usable_len;
    KNode *mail_node;
    struct MailEntry *entry;

    /* input and state checking */
    if (msg == NULL || len == NULL || author == NULL) {
        return -EFAULT;
    }

    if (*len == 0) {
        return -EINVAL;
    }


    mb = &current_process->mailbox;

    if (mb->count == 0) {
        /* will be woken once there is a message waiting*/
        block();
    }

    mail_node = list_pop(&mb->mail);
    entry = knode_data(mail_node, struct MailEntry, node);

    usable_len = entry->data.len > *len ? *len : entry->data.len;
    bmemcpy(msg, entry->data.msg, usable_len);
    *len = usable_len;
    *author = entry->author;

    kfree(entry->data.msg);
    kfree(entry);

    return 0;
}

int reply(int pid, void* msg, uint32_t len) {
    PCB *sender;
    struct SRRMailbox *s_box, *c_box;
    void* reply;

    if (msg == NULL) {
        return -EFAULT;
    }

    sender = get_PCB(pid);
    if (sender == NULL) {
        return -ESRCH;
    }

    if (len == 0) {
        return -EINVAL;
    }

    s_box = &sender->mailbox;
    c_box = &current_process->mailbox;

    if (s_box->sent_to != current_process->pid) {
        return -EINVAL;
    }

    reply = kmalloc(len);

    if (reply == NULL) {
        return -ENOMEM;
    }

    bmemcpy(reply, msg, len);
    s_box->reply.msg = reply;
    s_box->reply.len = len;

    wake_proc(pid);

    return 0;
}

int msg_waiting() {
    return current_process->mailbox.count > 0;
}

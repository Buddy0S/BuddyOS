#include <srr_ipc.h>
#include <stdint.h>
#include <syserr.h>
#include <proc.h>
#include <memory.h>

/*
 * Performs the first half of the send system call.
 *
 * Responsible for:
 *      Checking input validity.
 *      Ensuring the pid entered is a valid receiver.
 *      Putting the message into the receiver's mailbox.
 *      Waking up the receiver.
 *
 * Args:
 *      (int) pid: The process ID of the intended receiver.
 *      (struct Mail*) mail_in: The mail to be sent to the receiver by copying.
 *      (void*) reply: A memory address to store the reply.
 *      (uint*) rlen: A pointer which stores the length of the reply buffer.
 *
 * Returns an error code if an issue occurs, 0 on success.
 */
int send(int pid, struct Mail* mail_in, void* reply, uint32_t* rlen) {
    struct SRRMailbox *r_box, *c_box;
    struct MailEntry* new_mail;
    PCB* receiver;
    void* data;

    void* msg = mail_in->msg;
    uint32_t len = mail_in->len;
    
    /* input and state checking */
    if (pid == 0) {
        return -EINVAL;
    }

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

    r_box = &receiver->mailbox;
    c_box = &current_process->mailbox;
    /* This would result in both processes waiting for each other */
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

    kmemcpy(msg, data, len);

    new_mail->data.msg = data;
    new_mail->data.len = len;
    new_mail->author = current_process->pid;

    list_add_tail(&r_box->mail, &new_mail->node);
    r_box->count++;

    c_box->sent_to = pid;

    wake_proc(pid);

    block();
    return 0;
}


/*
 * Performs the last half of the send system call.
 *
 * Responsible for:
 *      Receiving the reply from the receiver.
 *      Copying the data back to the user.
 *      Freeing the the reply in the kernel.
 *
 * Args:
 *      (void*) reply: A memory address to store the reply.
 *      (uint*) rlen: A pointer which stores the length of the reply buffer.
 *
 * Returns an error code if an issue occurs, 0 on success.
 */
int send_end(void* reply, uint32_t* rlen){
    uint32_t usable_len;
    struct SRRMailbox *c_box;
    c_box = &current_process->mailbox;

    if (c_box->reply.msg == NULL) {
        return -ENOMSG;
    }
    
    /* By now the reply has been put in */
    usable_len = c_box->reply.len > *rlen ? *rlen : c_box->reply.len;
    kmemcpy(c_box->reply.msg, reply, usable_len);
    *rlen = usable_len;

    kfree(c_box->reply.msg);
    c_box->reply.msg = NULL;
    
    return 0;
}


/*
 * Performs the first half of the receive system call.
 *
 * Responsible for:
 *      Checking input validity.
 *      Checkign if there any messages and blocking if needed.
 *
 * Args:
 *      (int*) author: A pointer where the sender's pid will be placed.
 *      (void*) msg: A pointer where the message will be copied into.
 *      (uint*) len: A pointer containing the length of the msg buffer.
 *
 *  Returns an error message on failure and 0 on success.
 */
int receive(int* author, void* msg, uint32_t* len) {
    struct SRRMailbox* mb;

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

    return 0;
}


/*
 * Performs the last half of the receive system call.
 *
 * Responsible for:
 *      Copying the received message to the user inputted buffer and
 *              modifying len accordingly.
 *      Copying the authors pid over.
 *      Freeing the message sent in the kernel.
 *
 * Args:
 *      (int*) author: A pointer where the sender's pid will be placed.
 *      (void*) msg: A pointer where the message will be copied into.
 *      (uint*) len: A pointer containing the length of the msg buffer will be
 *              changed to reflect the length of the message in msg.
 *
 * Returns an error message on failure and 0 on success.
 */
int receive_end(int* author, void* msg, uint32_t* len) {
    struct SRRMailbox* mb;
    uint32_t usable_len;
    KNode *mail_node;
    struct MailEntry *entry;

    mb = &current_process->mailbox;
    if (mb->count == 0) {
        return -ENOMEM;
    }

    mail_node = list_pop(&mb->mail);
    entry = knode_data(mail_node, struct MailEntry, node);
    mb->count--;

    usable_len = entry->data.len > *len ? *len : entry->data.len;
    kmemcpy(entry->data.msg, msg, usable_len);
    *len = usable_len;
    *author = entry->author;

    kfree(entry->data.msg);
    kfree(entry);

    return 0;
}

/*
 * Sends a message back to the provided sender and waking it up.
 *
 * Args:
 *      (int) pid: The ID of the process being replied to.
 *      (void*) msg: the message being sent back in the reply.
 *      (uint) len: The length of the message being sent.
 *
 *  Returns an error message on failure and 0 on success.
 */
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

    kmemcpy(msg, reply, len);
    s_box->reply.msg = reply;
    s_box->reply.len = len;
    s_box->sent_to = 0;

    wake_proc(pid);

    return 0;
}


/*
 * Returns 1 if there is a message waiting to be
 * received by the calling process. Else, returns 0.
 */
int msg_waiting() {
    return current_process->mailbox.count > 0;
}

#include <srr_ipc.h>
#include <syserr.h>
#include <proc.h>

int send(int pid, void *msg, uint32_t len, void* reply, uint32_t* rlen) {
    struct SRRMailbox *r_box, *c_box;
    struct MailEntry* new_mail;
    PCB* receiver;
    void* data;
    uint32_t usable_len;
    
    /* input and state checking */
    if (msg == NULL) {
        return -EFAULT;
    }

    if (len == 0) {
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
    new_mail->mail.msg = data;
    new_mail->mail.len = len;
    new_mail->author = current_process->pid;

    list_add_tail(&r_box->mail, &new_mail->node);
    rbox->count++;

    c_box->sent_to = pid;

    wake_proc(pid);

    block();

    /* By now the reply has been put in */

    usable_len = c_box->reply.len > *rlen ? *rlen : c_box->reply.len;
    bmemcpy(reply, c_box->reply.msg, usable_len);
    *rlen = usable_len;

    kree(c_box->reply.msg);
    c_box->reply.msg = NULL;
    c_box->sent_to = 0;
    
    return 0;
}

int receive(int* author, void* msg, uint32_t* len) {

}

int reply(int pid, void* msg, uint32_t len) {

}

int msg_waiting() {
    return current_process->mailbox.count > 0;
}

/*
 * A file for defining error numbers for use in syscalls
 */

#ifndef SYSERR_H
#define SYSERR_H

#define ESRCH   1       //No such process
#define ENOMEM  2       //Not enough memory
#define EINVAL  3       //Invalid argument
#define ENOSYS  4       //Syscall not implemented
#define EFAULT  5       //Bad memory address
#define EDEADLK 6       //A deadlock would occur

#endif

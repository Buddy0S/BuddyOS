#ifndef ARGS_H
#define ARGS_H

#define BASE10 10
/* represents our lists of arguments */
typedef char* va_list;

/* defines where our variable argument list starts, starts at our first param address + 1 argument over since it would just be our string first then the unamed arguments */
#define va_start(list, param) (list = (va_list)&param + sizeof(param))

/* moves list over by correct number of bytes, but subtracts that type since we needto get the current one, type cast to appropriate and dereference to grab value */
#define va_args(list, type) (*(type*)((list += sizeof(type)) - sizeof(type)))

/* variable argument list has been processed */
#define va_end(list) (list = (void*)0)
#endif


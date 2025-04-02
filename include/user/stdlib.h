#ifndef STDLIB_H
#define STDLIB_H

void* malloc(int size);
void free(void* ptr);

// only works on non negative
int atoi(char *s) {
    int sum = 0;
    while((*s >= '0')&&(*s <= '9')) {
      sum = sum * 10;
      sum = sum + (*s - 48);
      s++;
    }
    return (sum);
}


#endif 

#include <stdint.h>
#include "srr_ipc.h"
#include "syscall.h"
#include "reg.h"
#include "uart.h"
#include "memory.h"
#include "list.h"
#include "proc.h"
#include "fat12.h"
#include "vfs.h"
#include "net.h"
#include "led.h"
#include "vfs.h"

/* Process function declarations */
extern void shell(void);

/* Function for delay */
void delay(void) {
    for (volatile unsigned int i = 0; i < 1000000; i++);
}

extern void null_proc(void);  
extern void dispatcher(void);

void buddy(void) {

    volatile int i;
    volatile int T = 500000;

    for (i = 0; i < T; i++);

    LEDon(LED1);

    for (i = 0; i < T; i++);

    LEDon(LED2);

    for (i = 0; i < T; i++);

    LEDon(LED3);

    for (i = 0; i < T; i++);

    LEDoff(LED0);

    for (i = 0; i < T; i++);

    LEDoff(LED1);

    for (i = 0; i < T; i++);

    LEDoff(LED2);

    for (i = 0; i < T; i++);

    LEDoff(LED3);

    for (i = 0; i < T; i++);

}


int main(){

  uart0_printf("Entering Kernel\n");

  /* Initialize buddyOS memory allocator */
  if (init_alloc() >= 0) {
    //uart0_printf("MEMORY ALLOCATOR INIT\n");
  } else {
    uart0_printf("MEMORY ALLOCATOR FAILED TO INIT\n");
  }

  /* Initialise all PCBs */
  for (int i = 0; i < MAX_PROCS; i++) {
    PROC_TABLE[i].state = DEAD;
  }

  init_network_stack();

  uint32_t* buffer = (uint32_t*)kmalloc(128 * sizeof(uint32_t));
  fat12_init(0, buffer);

  vfs_mount("/", FAT12);
  vfs_mount("/home", FAT12);

  /* Initialize the ready queue */
  init_ready_queue();

  init_process(&PROC_TABLE[0], null_proc, PROC_STACKS[0], LOW);
  init_process(&PROC_TABLE[1], shell, PROC_STACKS[1], HIGH);

  /* Set the current process to the head of the null proc for now */
  current_process = &PROC_TABLE[0];

  /* Call dispatcher */
  dispatcher();

  while (1) {}	

  return 0;

}

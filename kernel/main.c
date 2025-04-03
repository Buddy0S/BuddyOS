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
extern void process1(void);
extern void process2(void);
extern void shell(void);

/* Function for delay */
void delay(void) {
    for (volatile unsigned int i = 0; i < 1000000; i++);
}

extern void null_proc(void);  
extern void supervisor_call(void);
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

  /* ********* Test File system ********* */

  uint32_t* buffer = (uint32_t*)kmalloc(128 * sizeof(uint32_t));
	fat12_init(0, buffer);

	vfs_mount("/", FAT12);
	vfs_mount("/home", FAT12);

  //char buf[64];
  //char __attribute__((aligned(512))) test[512];

	//int fd = vfs_open("/home/test.txt", O_READ | O_WRITE);
	//int bytes = vfs_read(fd, buf, 64);
	//uart0_printf("%s (%d bytes)\n", buf, bytes);
	//bytes = vfs_write(fd, "I ain't reading all that", sizeof("I ain't reading all that"));
	//vfs_close(fd);

	//fd = vfs_open("/home/HELLO.TXT", O_WRITE);
	//bytes = vfs_write(fd, "Hello World!", sizeof("Hello World!"));
	//uart0_printf("Wrote %d bytes\n", bytes);
	//vfs_close(fd);

	//fd = vfs_open("/home/NOT_EX.TXT", O_READ);
	//bytes = vfs_write(fd, "Hello World!", sizeof("Hello World!"));
	//uart0_printf("Wrote %d bytes\n", bytes);
	//vfs_close(fd);

	//fd = vfs_open("/home/DIS.TXT", O_WRITE);
	//vfs_close(fd);

	//fd = vfs_open("/home/TEST.BIN", O_READ);
	//bytes = vfs_read(fd, test, 512);
	//uart0_printf("%s (%d bytes)\n", test, bytes);

  /* Initialize the ready queue */
  init_ready_queue();

  init_process(&PROC_TABLE[0], null_proc, PROC_STACKS[0], LOW);
  // init_process(&PROC_TABLE[1], shell, PROC_STACKS[1], HIGH);
  init_process(&PROC_TABLE[1], process1, PROC_STACKS[1], MEDIUM);
  init_process(&PROC_TABLE[2], process2, PROC_STACKS[2], MEDIUM);
  //init_process(&PROC_TABLE[1], (void (*)(void))(0x85000000), PROC_STACKS[1], HIGH);

  /* Set the current process to the head of the null proc for now */
  current_process = &PROC_TABLE[0];

  /* Call dispatcher */
  dispatcher();

  while (1) {}	

  return 0;

}

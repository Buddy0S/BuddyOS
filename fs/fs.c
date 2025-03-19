#include <stdint.h>
#include "vfs.h"
#include "fat12.h"
#include "memory.h"

int fat12_open(const char* path, int flags);
uint32_t fat12_read(int fd, char* read_buffer, int bytes);
uint32_t fat12_write(int fd, char* write_buffer, int bytes);
int fat12_close(int fd);

fs_ops fat12_ops = {
    .open = fat12_open,
    .read = fat12_read,
    .write = fat12_write,
};

extern file_descriptor vfs_openFiles[MAX_OPENED_FILES]; 

int fat12_open(const char* path, int flags) {
	return 0;
}

int fat12_close(int fd) {
	return 0;
}

uint32_t fat12_read(int fd, char* read_buffer, int bytes) {
	return 0;
}

uint32_t fat12_write(int fd, char* write_buffer, int bytes) {
	return 0;
}


#include "vfs.h"
#include "fat12.h"
#include "memory.h"

fs_ops fat12_fs = {
	.open = fat12_open;
	.close = NULL;
	.read = fat12_read;
	.write = fat12_write;
}

extern file_descriptor vfs_openFiles[MAX_OPENED_FILES]; 

int fat12_open(char* path, int flags) {
	return 0;
}

uint32_t fat12_read(int fd, char* read_buffer, int bytes) {
	return 0;
}

uint32_t fat12_write(int fd, char* write_buffer, int bytes) {
	return 0;
}


#include "vfs.h"
#include "memory.h"

/* Global Variables */
mountpoint vfs_mountpoints[MAX_MOUNTPOINTS];
file_descriptor vfs_openFiles[MAX_OPENED_FILES];
int mountedCount = 0;

mountpoint get_mountpoint(char* path) {
	return null;
}

int vfs_mount(char* target, int fs_type) {
	return 0;
}

int vfs_open(char* path, int flags) {
	return 0;
}

int vfs_close(int fd) {
	return 0;
}

uint32_t vfs_read(int fd, char* read_buffer, int bytes) {
	return 0;
}

uint32_t vfs_write(int fd, char* write_buffer, int bytes) {
	return 0;
}

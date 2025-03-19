#include <stdint.h>
#include "vfs.h"
#include "memory.h"

/* Global Variables */
mountpoint vfs_mountpoints[MAX_MOUNTPOINTS];
file_descriptor vfs_openFiles[MAX_OPENED_FILES];
int mountedCount = 0;

extern fs_ops fat12_ops;

mountpoint get_mountpoint(char* path) {
	return;
}

int vfs_mount(char* target, int type) {

	int i = 0;

	if (mountedCount >= MAX_MOUNTPOINTS) {
		return -1;
	}

	vfs_mountpoints[mountedCount].type = type;
	
	while (target[i] != '\0') {
		vfs_mountpoints[mountedCount].fs_mountpoint[i] = target[i];
		i++;
	}
	vfs_mountpoints[mountedCount].fs_mountpoint[i] = '\0';

	if (type == FAT12) {
		vfs_mountpoints[mountedCount].operations = fat12_ops;
	}

	mountedCount++;

	return 1;
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

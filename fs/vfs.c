#include <stdint.h>
#include "vfs.h"
#include "memory.h"
#include "string.h"
#include "uart.h"

#define NULL (void*)0

/* Global Variables */
mountpoint vfs_mountpoints[MAX_MOUNTPOINTS];
file_descriptor* vfs_openFiles[MAX_OPENED_FILES] = {NULL};
int mountedCount = 0;
int openCount = 0;

extern fs_ops fat12_ops;

mountpoint *get_mountpoint(char* path) {
	
	mountpoint* mntPnt = NULL;
	int longestMatch = -1;
	int mntPntLen = 0;

	for (int i = 0; i < mountedCount; i++) {

		mntPntLen = strlen(vfs_mountpoints[i].fs_mountpoint);

		if (strncmp(path, vfs_mountpoints[i].fs_mountpoint, mntPntLen) == 0) {
			if (mntPntLen > longestMatch) {
				longestMatch = mntPntLen;
				mntPnt = &vfs_mountpoints[i]; 
			}
		}
	}

	return mntPnt;
}

int vfs_mount(char* target, int type) {

	int i = 0;

	if (mountedCount >= MAX_MOUNTPOINTS) {
		return -1;
	}

	vfs_mountpoints[mountedCount].type = type;
	
	if (!strcpy(vfs_mountpoints[mountedCount].fs_mountpoint, target)) {
		return -1;
	}

	if (type == FAT12) {
		vfs_mountpoints[mountedCount].operations = fat12_ops;
	}

	mountedCount++;

	return 1;
}

int vfs_open(char* path, int flags) {

	mountpoint *mnt = get_mountpoint(path);
	char relPath[VFS_PATH_LEN];
	file_descriptor* fdOpen = NULL;

	uart0_printf("Got to vfs_open(), %s, %d\n", path, flags);

	if (openCount == MAX_OPENED_FILES) {
		return -3; /* max files open */
	}

	uart0_printf("Mnt pnt = %s\n", mnt->fs_mountpoint);

	if (mnt != NULL) {
		
		fdOpen = mnt->operations.open(path + strlen(mnt->fs_mountpoint) + 1,
			flags);

		if (fdOpen != NULL) {

			for (int i = 0; i < MAX_OPENED_FILES; i++) {
				if (vfs_openFiles[i] == NULL) {
					vfs_openFiles[i] = fdOpen;
					openCount++;
					return i; /* Return fd id */
				}
			}
		}
		else {
			return -2; /* File not found */
		}
	}
	else {
		return -1; /* Mount point not found */
	}
}

int vfs_close(int fd) {
	
	mountpoint mnt;
	int mountpoint_id; 
	int result = 0;

	if (vfs_openFiles[fd] != NULL) {

		mountpoint_id = vfs_openFiles[fd]->mountpoint_id;
		mnt = vfs_mountpoints[mountpoint_id];

		result = mnt.operations.close(vfs_openFiles[fd]);

		if (result) {
			vfs_openFiles[fd] = NULL;
			openCount--;
			return 0;
		}
		else {
			return -2; /* Could not close */
		}

	}
	else {
		return -1; /* File not open */
	}

}

/* add checks for file open flags */
uint32_t vfs_read(int fd, char* read_buffer, int bytes) {

	int mountpoint_id;
	mountpoint mnt;
	int bytesRead = 0;

	if (vfs_openFiles[fd] != NULL) {
		mountpoint_id = vfs_openFiles[fd]->mountpoint_id;
		mnt = vfs_mountpoints[mountpoint_id];

		bytesRead = mnt.operations.read(fd, read_buffer, bytes);

		/* Maybe move to fs.c */
		if (vfs_openFiles[fd]->read_offset + bytes <
			vfs_openFiles[fd]->file_size) {
				
			vfs_openFiles[fd]->read_offset += bytes;	
		}
		else {
			vfs_openFiles[fd]->read_offset = vfs_openFiles[fd]->file_size;	
		}
		/**********************/

		return bytesRead;

	}
	else {
		return -1; /* File not open */
	}
}

uint32_t vfs_write(int fd, char* write_buffer, int bytes) {
	
	int mountpoint_id;
	mountpoint mnt;
	int bytesRead = 0;

	if (vfs_openFiles[fd] != NULL) {
		mountpoint_id = vfs_openFiles[fd]->mountpoint_id;
		mnt = vfs_mountpoints[mountpoint_id];

		bytesRead = mnt.operations.write(fd, write_buffer, bytes);

		return bytesRead;

	}
	else {
		return -1; /* File not open */
	}

}

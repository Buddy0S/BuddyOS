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
		return MAX_REACHED;
	}

	vfs_mountpoints[mountedCount].type = type;
	
	if (!strcpy(vfs_mountpoints[mountedCount].fs_mountpoint, target)) {
		return MEM_ERROR;
	}

	if (type == FAT12) {
		vfs_mountpoints[mountedCount].operations = fat12_ops;
	}

	mountedCount++;

	return 0;
}

int vfs_open(char* path, int flags) {

	mountpoint *mnt = get_mountpoint(path);
	char relPath[VFS_PATH_LEN];
	file_descriptor* fdOpen = NULL;
	int fdNum = -1;

	uart0_printf("Got to vfs_open(), %s, %d\n", path, flags);

	if (openCount == MAX_OPENED_FILES) {
		return MAX_REACHED; /* max files open */
	}

	uart0_printf("Mnt pnt = %s\n", mnt->fs_mountpoint);

	if (mnt != NULL) {

		strcpy(relPath, path + strlen(mnt->fs_mountpoint) + 1);

		fdOpen = mnt->operations.open(relPath, flags);

		if (fdOpen != NULL) {

			for (int i = 0; i < MAX_OPENED_FILES; i++) {
				if (vfs_openFiles[i] != NULL) {
					if(strcmp(vfs_openFiles[i]->file_name, relPath) == 0) {
						kfree(fdOpen->file_buffer);
						kfree(fdOpen);
						return ALREADY_OPEN;
					}

					uart0_printf("CHECKING DUPE\n");	
				}
				else if (fdNum < 0) {
						fdNum = i;
				}
			}

			uart0_printf("fdNum = %d\n", fdNum);

			vfs_openFiles[fdNum] = fdOpen;
			openCount++;
			return fdNum; /* Return fd id */

		}
		else {
			return NOT_FOUND; /* File not found */
		}
	}
	else {
		return INVALID_MOUNTPOINT; /* Mount point not found */
	}
}

int vfs_close(int fd) {
	
	mountpoint mnt;
	int mountpoint_id; 
	int result = 1;

	if (fd < 0 || fd > MAX_OPENED_FILES - 1) {
		return INVALID_FD;
	}

	if (vfs_openFiles[fd] != NULL) {

		mountpoint_id = vfs_openFiles[fd]->mountpoint_id;
		mnt = vfs_mountpoints[mountpoint_id];

		result = mnt.operations.close(vfs_openFiles[fd]);

		if (result == 0) {
			vfs_openFiles[fd] = NULL;
			openCount--;
			return 0;
		}
		else {
			return CLOSE_ERROR; /* Could not close */
		}

	}
	else {
		return NOT_OPEN; /* File not open */
	}

}

/* add checks for file open flags */
uint32_t vfs_read(int fd, char* read_buffer, int bytes) {

	int mountpoint_id;
	mountpoint mnt;
	int bytesRead = 0;

	if (fd < 0 || fd > MAX_OPENED_FILES - 1) {
		return INVALID_FD;
	}

	if (vfs_openFiles[fd] != NULL) {
		
		if (!(vfs_openFiles[fd]->flags & O_READ)) {
			return INCORRECT_MODE; /* Incorrect Mode */
		}

		mountpoint_id = vfs_openFiles[fd]->mountpoint_id;
		mnt = vfs_mountpoints[mountpoint_id];

		bytesRead = mnt.operations.read(vfs_openFiles[fd], read_buffer, bytes);

		return bytesRead;

	}
	else {
		return NOT_OPEN; /* File not open */
	}
}

uint32_t vfs_write(int fd, char* write_buffer, int bytes) {
	
	int mountpoint_id;
	mountpoint mnt;
	int bytesRead = 0;

	if (fd < 0 || fd > MAX_OPENED_FILES - 1) {
		return INVALID_FD;
	}

	if (vfs_openFiles[fd] != NULL) {
		
		if (!(vfs_openFiles[fd]->flags & O_WRITE)) {
			return INCORRECT_MODE; /* Incorrect Mode */
		}

		mountpoint_id = vfs_openFiles[fd]->mountpoint_id;
		mnt = vfs_mountpoints[mountpoint_id];

		bytesRead = mnt.operations.write(vfs_openFiles[fd], write_buffer, bytes);

		return bytesRead;

	}
	else {
		return NOT_OPEN; /* File not open */
	}

}

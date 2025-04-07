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

/*
 * Gets the most appropriate mountpoint based on passed in path
 *
 * Args:
 *     char* path: Path for which the mountpoint needs to be retrieved
 *
 * Returns a pointer to the mountpoint structure that best matches the path.
 * Returns NULL if a match was not found 
 */
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


/*
 * Mounts a filesystem at the specified target path
 * 
 * Args:
 *     char* target: Path at which the filesystem should be mounted
 *     int type: Type of the filesystem to mount (ex. FAT12)
 *
 * Returns 0 on successful mount
 * Returns MAX_REACHED if the maximum number of mountpoints is reached
 * Returns MEM_ERROR if there was an error copying the mountpoint path
 */
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


/*
 * Opens a file at the specified path with the given flags
 * 
 * Args:
 *     char* path: Path of the file to open
 *     int flags: Flags specifying file access mode (ex. O_READ | O_WRITE)
 *
 * Returns a file descriptor number on success
 * Returns MAX_REACHED if the maximum number of file are already open
 * Returns ALREADY_OPEN if the file is already open
 * Returns NOT_FOUND if the file was not found
 * Returns INVALID_MOUNTPOINT if no valid mountpoint was found for the path
 */
int vfs_open(char* path, int flags) {

	mountpoint *mnt = get_mountpoint(path);
	char relPath[VFS_PATH_LEN];
	file_descriptor* fdOpen = NULL;
	int fdNum = -1;

	if (openCount == MAX_OPENED_FILES) {
		return MAX_REACHED; /* max files open */
	}

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

				}
				else if (fdNum < 0) {
						fdNum = i;
				}
			}

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


/*
 * Closes a file associated with the passed in file descriptor
 * 
 * Args:
 *     int fd: File descriptor of the file to be closed
 *
 * Returns 0 on successful close
 * Returns INVALID_FD if the file descriptor is out of the accepted range
 * Returns NOT_OPEN if the file is not open
 * Returns CLOSE_ERROR if the filesystem function failed to close the file
 */
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


/*
 * Returns data from an open file into the provied buffer
 * 
 * Args:
 *     int fd: File descriptor of the file to read from
 *     char* read_buffer: Buffer into which the file data will be read
 *     int bytes: Number of bytes to read
 *
 * Returns the number of bytes read on success
 * Returns INVALID_FD if the file descriptor is out of the accepted range
 * Returns NOT_OPEN if the file is not open
 * Returns INCORRECT_MODE if the file is not opened in read mode
 */
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


/*
 * Writes data into an open file from the provied buffer
 * 
 * Args:
 *     int fd: File descriptor of the file to write to
 *     char* write_buffer: Buffer containing the data to write
 *     int bytes: Number of bytes to write
 *
 * Returns the number of bytes written on success
 * Returns INVALID_FD if the file descriptor is out of the accepted range
 * Returns NOT_OPEN if the file is not open
 * Returns INCORRECT_MODE if the file is not opened in write mode
 */
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


/*
 * Moves the appropriate file pointer based on the passed in mode
 * 
 * Args:
 *     int fd: File descriptor of the file to seek in
 *     int offset: Offset to seek to from the current location within the file
 *     int mode: Head to seek (O_READ and/or O_WRITE)
 *
 * Returns the new offset within the file on success
 * Returns INVALID_FD if the file descriptor is out of the accepted range
 * Returns NOT_OPEN if the file is not open
 */
uint32_t vfs_seek(int fd, int offset, int mode) {
	
	int mountpoint_id;
	mountpoint mnt;
	int newOffset = 0;

	if (fd < 0 || fd > MAX_OPENED_FILES - 1) {
		return INVALID_FD;
	}

	if (vfs_openFiles[fd] != NULL) {
		
		mountpoint_id = vfs_openFiles[fd]->mountpoint_id;
		mnt = vfs_mountpoints[mountpoint_id];

		newOffset = mnt.operations.seek(vfs_openFiles[fd], offset, mode);

		return newOffset;

	}
	else {
		return NOT_OPEN; /* File not open */
	}

}


/*
 * Retrieves the size of the file based on the passed in file descriptor
 * 
 * Args:
 *     int fd: File descriptor of the file
 *
 * Returns the size of the file on success
 * Returns INVALID_FD if the file descriptor is out of the accepted range
 * Returns NOT_OPEN if the file is not open
 */
uint32_t vfs_getFileSize(int fd) {
	
	if (fd < 0 || fd > MAX_OPENED_FILES - 1) {
		return INVALID_FD;
	}

	if (vfs_openFiles[fd] != NULL) {
		
		return vfs_openFiles[fd]->file_size;

	}
	else {
		return NOT_OPEN; /* File not open */
	}
}


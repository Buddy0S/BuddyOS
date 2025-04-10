/*
	CMPT 432 - Implementation Team 00
*/

#include <stdint.h>
#include "vfs.h"
#include "uart.h"
#include "fat12.h"
#include "memory.h"
#include "string.h"

#define NULL (void*)0

file_descriptor* fat12_open(const char* path, int flags);
uint32_t fat12_read(file_descriptor* fd, char* read_buffer, int bytes);
uint32_t fat12_write(file_descriptor* fd, char* write_buffer, int bytes);
int fat12_close(file_descriptor* fd);
uint32_t fat12_seek(file_descriptor* fd, int offset, int mode);

fs_ops fat12_ops = {
    .open = fat12_open,
    .read = fat12_read,
    .write = fat12_write,
	.close = fat12_close,
	.seek = fat12_seek
};

extern file_descriptor vfs_openFiles[MAX_OPENED_FILES]; 


/*
 * Opens a file on the FAT12 filesystem or creates it if it does not exist
 *
 * Args:
 *      const char* path: Path of the file to open or create
 *      int flags: Flags specifying the file access mode (ex. O_READ|O_WRITE)
 *
 * Returns a pointer to a file_descriptor on success
 * Returns NULL if there was an error creating or opening the file
 */
file_descriptor* fat12_open(const char* path, int flags) {
	uint32_t tempBuffer[128];
	file_descriptor* fdOpen;
	uint32_t entryIndex;
    uint32_t alloc_size;
	DirEntry dir;

	fdOpen = (file_descriptor*) kmalloc(sizeof(file_descriptor));
	if (fdOpen == NULL) {
		return NULL;
	}

	if (fat12_find(path, tempBuffer, &entryIndex) > 0) {
		
		dir = ((DirEntry*)tempBuffer)[entryIndex];

		/* Initialize fd fields */
		strcpy(fdOpen->file_name, path);
		fdOpen->flags = flags;
		fdOpen->read_offset = 0;
		fdOpen->write_offset = 0;
		fdOpen->file_size = dir.fileSize;
        alloc_size = (((fdOpen->file_size / 512) + 1) * 512);
		fdOpen->file_buffer = (char*)kmalloc(sizeof(char) * alloc_size);

		if (fdOpen->file_buffer == NULL) {
			kfree(fdOpen);
			return NULL;
		}

		/* Maybe add some kinda check later */
		fat12_read_file(path, (uint32_t*)fdOpen->file_buffer, tempBuffer);

		return fdOpen;
		
	}
	else if (flags & O_WRITE){
		/* Create file */
		fat12_write_file(path, "", 0, tempBuffer);

		/* Initialize fd fields */
		strcpy(fdOpen->file_name, path);
		fdOpen->flags = flags;
		fdOpen->read_offset = 0;
		fdOpen->write_offset = 0;
		fdOpen->file_size = 0;
		fdOpen->file_buffer = (char*)kmalloc(sizeof(char));

		if (fdOpen->file_buffer == NULL) {
			kfree(fdOpen);
			return NULL;
		}

		return fdOpen;

	}

	return NULL;
}


/*
 * Closes a file on the FAT12 filesystem and writes the data bufferred in memory
 * back to disk
 *
 * Args:
 *      file_descriptor* fd: Pointer to the file descriptor of the file to close
 *
 * Returns 0 on success
 */
int fat12_close(file_descriptor* fd) {
	uint32_t tempBuffer[128];
	int bytes;

	bytes = fat12_write_file(fd->file_name, fd->file_buffer, fd->file_size, tempBuffer);

	if (bytes >= fd->file_size) {
		kfree(fd->file_buffer);
		kfree(fd);
	}

	return 0;

}


/*
 * Reads data from a file in the FAT12 filesystem into the provided buffer
 *
 * Args:
 *      file_descriptor* fd: Pointer to the file descriptor of the file to
 *                           read from
 *      char* read_buffer: Buffer where the data will be read into
 *      int bytes: Number of bytes to read
 *
 * Returns the number of bytes read on success.
 */
uint32_t fat12_read(file_descriptor* fd, char* read_buffer, int bytes) {
	
	int bytesRead = 0;

	if (fd->read_offset >= fd->file_size) {
		return 0;
	}
	else {
		
		kmemcpy(fd->file_buffer + fd->read_offset, read_buffer, bytes);
			if (fd->read_offset + bytes < fd->file_size) {
				bytesRead = bytes;
			}
			else {
				bytesRead = fd->file_size - fd->read_offset;
			}

			fd->read_offset += bytesRead;

		return bytesRead;

	}

}


/*
 * Write data to a file in the FAT12 filesystem from the passed in buffer
 *
 * Args:
 *      file_descriptor* fd: Pointer to the file descriptor of the file to
 *                           write to
 *      char* write_buffer: Buffer containing the data to be written
 *      int bytes: Number of bytes to write
 *
 * Returns the number of bytes written on success.
 */
uint32_t fat12_write(file_descriptor* fd, char* write_buffer, int bytes) {
	
	char *oldBuffer, *newBuffer;

	if (fd->write_offset + bytes > fd->file_size) {
		
		newBuffer = (char*)kmalloc(sizeof(char) * fd->write_offset + bytes);
		oldBuffer = fd->file_buffer;

		fd->file_buffer = newBuffer;

		kmemcpy(oldBuffer, fd->file_buffer, fd->file_size);

		kfree(oldBuffer);
	}

	kmemcpy(write_buffer, fd->file_buffer + fd->write_offset, bytes);
	
	fd->write_offset += bytes;

	if (fd->write_offset > fd->file_size) {
		fd->file_size = fd->write_offset;
	}

	return bytes;

}


/*
 * Moves the read or write offset of a file in the FAT12 filesystem
 *
 * Args:
 *      file_descriptor* fd: Pointer to the file descriptor of the file
 *      int offset: The new offset value
 *      int mode: Indicates whether to adjust the read and/or write head
 *                (O_READ or O_WRITE)
 *
 * Returns the new offset after the seek operation.
 */
uint32_t fat12_seek(file_descriptor* fd, int offset, int mode) {
	
	int newOffset = 0;

	if (mode & O_READ) {
		
		if (offset > fd->file_size) {
			fd->read_offset = fd->file_size;
		}
		else {
			fd->read_offset = offset;
		}

		newOffset = fd->read_offset;

	}

	if (mode & O_WRITE) {
		
		if (offset > fd->file_size) {
			fd->write_offset = fd->file_size;
		}
		else {
			fd->write_offset = offset;
		}

		newOffset = fd->write_offset;

	}

	return newOffset;

}


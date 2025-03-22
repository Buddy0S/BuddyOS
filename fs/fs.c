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

fs_ops fat12_ops = {
    .open = fat12_open,
    .read = fat12_read,
    .write = fat12_write,
	.close = fat12_close
};

extern file_descriptor vfs_openFiles[MAX_OPENED_FILES]; 

file_descriptor* fat12_open(const char* path, int flags) {
	uint32_t tempBuffer[128];
	file_descriptor* fdOpen;
	uint32_t entryIndex;
	DirEntry dir;

	uart0_printf("Got to fat12_open() - %s, %d\n", path, flags);

	fdOpen = (file_descriptor*) kmalloc(sizeof(file_descriptor));
	if (fdOpen == NULL) {
		return NULL;
	}

	if (fat12_find(path, tempBuffer, &entryIndex) > 0) {
		
		dir = ((DirEntry*)tempBuffer)[entryIndex];

		/* Initialize fd fields */
		strcpy(fdOpen->file_name, path);
		fdOpen->fs_file_id = dir.firstClusterLow;
		fdOpen->flags = flags;
		fdOpen->read_offset = 0;
		fdOpen->write_offset = 0;
		fdOpen->file_size = dir.fileSize;
		fdOpen->file_buffer = (char*)kmalloc(sizeof(char) * dir.fileSize);

		if (fdOpen->file_buffer == NULL) {
			kfree(fdOpen);
			return NULL;
		}

		/* Maybe add some kinda check later */
		fat12_read_file(path, (uint32_t*)fdOpen->file_buffer, tempBuffer);

		uart0_printf("File Contents = %s\n", fdOpen->file_buffer);

		return fdOpen;
		
	}
	else if (flags & O_WRITE){
		/* Create file */
		fat12_write_file(path, "", 0, tempBuffer);

		/* Initialize fd fields */
		strcpy(fdOpen->file_name, path);
		fdOpen->fs_file_id = 5;
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

int fat12_close(file_descriptor* fd) {
	uint32_t tempBuffer[128];
	int bytes;

	uart0_printf("Closing %s\n", fd->file_name);
	
	bytes = fat12_write_file(fd->file_name, fd->file_buffer, fd->file_size, tempBuffer);

	if (bytes >= fd->file_size) {
		kfree(fd->file_buffer);
		kfree(fd);
	}

	return 0;

}

uint32_t fat12_read(file_descriptor* fd, char* read_buffer, int bytes) {
	
	int bytesRead = 0;

	uart0_printf("Reading from %s\n", fd->file_name);

	if (fd->read_offset >= fd->file_size) {
		return 0;
	}
	else {
		
		if (strncpy(read_buffer, fd->file_buffer + fd->read_offset, bytes)) {
			if (fd->read_offset + bytes < fd->file_size) {
				bytesRead = bytes;
			}
			else {
				bytesRead = fd->file_size - fd->read_offset;
			}

			fd->read_offset += bytesRead;
		}

		return bytesRead;

	}

}

uint32_t fat12_write(file_descriptor* fd, char* write_buffer, int bytes) {
	
	char *oldBuffer, *newBuffer;

	uart0_printf("Writing to %s\n", fd->file_name);

	if (fd->write_offset + bytes > fd->file_size) {
		
		newBuffer = (char*)kmalloc(sizeof(char) * fd->write_offset + bytes);
		oldBuffer = fd->file_buffer;

		fd->file_buffer = newBuffer;

		strncpy(fd->file_buffer, oldBuffer, fd->file_size);

		kfree(oldBuffer);
	}

	strncpy(fd->file_buffer + fd->write_offset, write_buffer, bytes);
	
	fd->write_offset += bytes;

	if (fd->write_offset > fd->file_size) {
		fd->file_size = fd->write_offset;
		uart0_printf("%s: size = %d\n", fd->file_buffer, fd->file_size);
	}

	return bytes;

}


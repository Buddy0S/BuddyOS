#include <stdint.h>
#include "vfs.h"
#include "fat12.h"
#include "memory.h"

#define NULL (void*)0

file_descriptor* fat12_open(const char* path, int flags);
uint32_t fat12_read(int fd, char* read_buffer, int bytes);
uint32_t fat12_write(int fd, char* write_buffer, int bytes);
int fat12_close(int fd);

fs_ops fat12_ops = {
    .open = fat12_open,
    .read = fat12_read,
    .write = fat12_write,
};

extern file_descriptor vfs_openFiles[MAX_OPENED_FILES]; 

file_descriptor* fat12_open(const char* path, int flags) {
	
	uint32_t tempBuffer[128];
	file_descriptor* fdOpen;
	uint32_t entryIndex;
	DirEntry dir;

	fdOpen = (file_descriptor*) kmalloc(sizeof(file_descriptor));
	if (fdOpen == NULL) {
		return NULL;
	}

	if (fat12_find(path, tempBuffer, &entryIndex) > 0) {
		
		dir = ((DirEntry*)tempBuffer)[entryIndex];

		/* Initialize fd fields */
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
		fat12_read_file(path, fdOpen->file_buffer, tempBuffer);
		
		return fdOpen;
	}
	else {
		return NULL;
	}
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


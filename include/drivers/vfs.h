/* CMPT432 - ImplementationTeam00 */

#ifndef VFS_H
#define VFS_H

#define VFS_PATH_LEN 64
#define MAX_MOUNTPOINTS 8
#define MAX_OPENED_FILES 16

/* Mount point types */
#define FAT12 	12
#define DEVICE 	13

/* File Modes */
#define O_READ 	0x01
#define O_WRITE 0x02

/* Error codes */
#define MAX_REACHED			-1
#define MEM_ERROR			-2
#define NOT_FOUND			-3
#define NOT_OPEN			-4
#define INCORRECT_MODE		-5
#define INVALID_MOUNTPOINT	-6
#define CLOSE_ERROR			-7
#define ALREADY_OPEN		-8
#define INVALID_FD			-9

typedef struct {
  	char file_name[16];
	int mountpoint_id;
	int flags;
	int read_offset;
	int write_offset;
	int file_size;
	char* file_buffer;
} file_descriptor;

typedef struct {
	file_descriptor* (*open)(const char* path, int flags);
	int (*close)(file_descriptor* fd);
	uint32_t (*read)(file_descriptor* fd, char* read_buffer, int bytes);
	uint32_t (*write)(file_descriptor* fd, char* write_buffer, int bytes);
	uint32_t (*seek)(file_descriptor* fd, int offset, int mode);
} fs_ops;

typedef struct {
	int mountpoint_id;
	int type;
	char fs_mountpoint[VFS_PATH_LEN];
	fs_ops operations;
} mountpoint;

extern mountpoint mountpoints[MAX_MOUNTPOINTS]; 

int vfs_mount(char* target, int type);
int vfs_open(char* path, int flags);
int vfs_close(int fd);
uint32_t vfs_read(int fd, char* read_buffer, int bytes);
uint32_t vfs_write(int fd, char* write_buffer, int bytes);
uint32_t vfs_seek(int fd, int offset, int mode);
uint32_t vfs_getFileSize(int fd);
mountpoint *get_mountpoint(char* path);
#endif

#ifndef VFS_H
#define VFS_H

#define VFS_PATH_LEN 64
#define MAX_MOUNTPOINTS 8
#define MAX_OPENED_FILES 16;

extern mountpoint mountpoints[MAX_MOUNTPOINTS]; 

typedef struct {
	int fs_type;
	char fs_mountpoint[VFS_PATH_LEN];
	fs_ops operations;
} mountpoint;

typedef struct {
	int (*open)(const char* path, int flags);
	int (*close)(int fd);
	uint32_t (*read)(int fd, char* read_buffer, int bytes);
	uint32_t (*write)(int fd, char* write_buffer, int bytes);
} fs_ops;

typedef struct {
	uint32_t fs_file_id;
	int mountpoint_id;
	int flags;
	int read_offset;
	int write_offset;
	int file_size;
	char* file_buffer;
} file_descriptor;

int vfs_mount(char* target, int fs_type);
int vfs_open(char* path, flags);
int vfs_close(int fd);
uint32_t vfs_read(int fd, char* read_buffer, int bytes);
uint32_t vfs_write(int fd, char* write_buffer, int bytes);
mountpoint get_mountpoint(char* path);

#endif

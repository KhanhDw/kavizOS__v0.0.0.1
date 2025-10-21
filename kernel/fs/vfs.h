#ifndef VFS_H
#define VFS_H

#include <stdint.h>
#include <stddef.h>

// File system types
#define VFS_TYPE_FAT32    0x01
#define VFS_TYPE_FAT16    0x02
#define VFS_TYPE_EXT2     0x03

// File access modes
#define VFS_MODE_READ     0x01
#define VFS_MODE_WRITE    0x02
#define VFS_MODE_APPEND   0x04
#define VFS_MODE_CREATE   0x08

// File attributes
#define VFS_ATTR_READ_ONLY   0x01
#define VFS_ATTR_HIDDEN      0x02
#define VFS_ATTR_SYSTEM      0x04
#define VFS_ATTR_DIRECTORY   0x10
#define VFS_ATTR_ARCHIVE     0x20

// Maximum path length
#define VFS_MAX_PATH        256
#define VFS_MAX_FILENAME    255

// Forward declarations
struct vfs_filesystem;
struct vfs_file;

// VFS directory entry structure
struct vfs_dirent {
    char name[VFS_MAX_FILENAME + 1];
    uint32_t size;
    uint32_t attributes;
    uint32_t creation_time;
    uint32_t creation_date;
    uint32_t access_time;
    uint32_t access_date;
    uint32_t modification_time;
    uint32_t modification_date;
    struct vfs_filesystem* filesystem;  // Pointer to the filesystem that owns this entry
};

// VFS file structure
struct vfs_file {
    char path[VFS_MAX_PATH];
    uint32_t flags;           // Access mode flags
    uint32_t position;        // Current position in file
    uint32_t size;            // File size
    uint32_t attributes;      // File attributes
    struct vfs_filesystem* filesystem;  // Pointer to the filesystem that owns this file
    void* private_data;       // Filesystem-specific data
};

// VFS filesystem operations structure
struct vfs_filesystem_ops {
    // Mount/unmount operations
    int (*mount)(struct vfs_filesystem* fs, void* device);
    int (*unmount)(struct vfs_filesystem* fs);
    
    // File operations
    int (*open)(struct vfs_file* file, const char* path, uint32_t flags);
    int (*close)(struct vfs_file* file);
    int (*read)(struct vfs_file* file, void* buffer, uint32_t size, uint32_t* bytes_read);
    int (*write)(struct vfs_file* file, const void* buffer, uint32_t size, uint32_t* bytes_written);
    int (*seek)(struct vfs_file* file, int32_t offset, int whence);
    int (*tell)(struct vfs_file* file, uint32_t* position);
    int (*truncate)(struct vfs_file* file, uint32_t size);
    
    // Directory operations
    int (*mkdir)(const char* path);
    int (*rmdir)(const char* path);
    struct vfs_dirent* (*readdir)(struct vfs_file* dir, uint32_t index);
    int (*opendir)(struct vfs_file* dir, const char* path);
    int (*closedir)(struct vfs_file* dir);
    
    // File management operations
    int (*stat)(const char* path, struct vfs_dirent* entry);
    int (*unlink)(const char* path);
    int (*rename)(const char* old_path, const char* new_path);
};

// VFS filesystem structure
struct vfs_filesystem {
    char name[32];
    uint32_t type;
    uint32_t block_size;
    uint32_t total_blocks;
    uint32_t free_blocks;
    struct vfs_filesystem_ops* ops;
    void* device;             // Pointer to the storage device
    void* private_data;       // Filesystem-specific data
};

// VFS mount point structure
struct vfs_mount_point {
    char path[VFS_MAX_PATH];
    struct vfs_filesystem* filesystem;
    struct vfs_mount_point* next;
};

// Global VFS functions
int vfs_init(void);
int vfs_mount(const char* mount_point, struct vfs_filesystem* fs);
int vfs_unmount(const char* mount_point);
int vfs_register_filesystem(const char* name, struct vfs_filesystem_ops* ops);

// File operations
int vfs_open(struct vfs_file* file, const char* path, uint32_t flags);
int vfs_close(struct vfs_file* file);
int vfs_read(struct vfs_file* file, void* buffer, uint32_t size, uint32_t* bytes_read);
int vfs_write(struct vfs_file* file, const void* buffer, uint32_t size, uint32_t* bytes_written);
int vfs_seek(struct vfs_file* file, int32_t offset, int whence);
int vfs_tell(struct vfs_file* file, uint32_t* position);
int vfs_truncate(struct vfs_file* file, uint32_t size);

// Directory operations
int vfs_mkdir(const char* path);
int vfs_rmdir(const char* path);
int vfs_opendir(struct vfs_file* dir, const char* path);
int vfs_closedir(struct vfs_file* dir);
struct vfs_dirent* vfs_readdir(struct vfs_file* dir, uint32_t index);

// File management operations
int vfs_stat(const char* path, struct vfs_dirent* entry);
int vfs_unlink(const char* path);
int vfs_rename(const char* old_path, const char* new_path);

// Path manipulation
int vfs_resolve_path(const char* in_path, char* out_path, size_t out_size);

#endif // VFS_H
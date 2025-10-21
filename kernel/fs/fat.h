#ifndef FAT_H
#define FAT_H

#include <stdint.h>
#include "../fs/vfs.h"

// FAT types
#define FAT_TYPE_FAT12  0
#define FAT_TYPE_FAT16  1
#define FAT_TYPE_FAT32  2

// FAT32 specific constants
#define FAT32_EOC       0x0FFFFFFF  // End of cluster chain marker
#define FAT32_BAD_CLUSTER 0x0FFFFFF7  // Bad cluster marker

// FAT directory entry attributes
#define FAT_ATTR_READ_ONLY  0x01
#define FAT_ATTR_HIDDEN     0x02
#define FAT_ATTR_SYSTEM     0x04
#define FAT_ATTR_VOLUME_ID  0x08
#define FAT_ATTR_DIRECTORY  0x10
#define FAT_ATTR_ARCHIVE    0x20
#define FAT_ATTR_LONG_NAME  (FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN | FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME_ID)

// FAT long directory entry
#define FAT_LFN_LAST_ENTRY  0x40

// FAT boot sector structure (FAT16/FAT32)
struct fat_boot_sector {
    uint8_t jmp_boot[3];
    uint8_t oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t num_fats;
    uint16_t root_entries;
    uint16_t total_sectors_16;
    uint8_t media_type;
    uint16_t fat_size_16;
    uint16_t sectors_per_track;
    uint16_t num_heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;
    
    // Extended BIOS Parameter Block (FAT32)
    union {
        struct {
            uint8_t drive_number;
            uint8_t reserved1;
            uint8_t boot_signature;
            uint32_t volume_id;
            uint8_t volume_label[11];
            uint8_t file_system_type[8];
        } fat16;
        
        struct {
            uint32_t fat_size_32;
            uint16_t ext_flags;
            uint16_t fs_version;
            uint32_t root_cluster;
            uint16_t fs_info;
            uint16_t backup_boot_sector;
            uint8_t reserved[12];
            uint8_t drive_number;
            uint8_t reserved1;
            uint8_t boot_signature;
            uint32_t volume_id;
            uint8_t volume_label[11];
            uint8_t file_system_type[8];
        } fat32;
    };
    
    uint8_t boot_code[420];
    uint16_t boot_sig;
} __attribute__((packed));

// FAT directory entry structure
struct fat_dirent {
    uint8_t name[11];           // 8.3 filename
    uint8_t attr;               // File attributes
    uint8_t nt_res;             // Reserved for NT
    uint8_t crt_time_tenth;     // Creation time (tenths of second)
    uint16_t crt_time;          // Creation time
    uint16_t crt_date;          // Creation date
    uint16_t acc_date;          // Last access date
    uint16_t fst_clus_hi;       // High word of first cluster (FAT32)
    uint16_t wrt_time;          // Last write time
    uint16_t wrt_date;          // Last write date
    uint16_t fst_clus_lo;       // Low word of first cluster
    uint32_t file_size;         // File size in bytes
} __attribute__((packed));

// FAT long directory entry structure
struct fat_lfn_entry {
    uint8_t seq_num;            // Sequence number
    uint16_t name1[5];          // First 5 characters
    uint8_t attr;               // Attribute (always 0x0F)
    uint8_t type;               // Long entry type
    uint8_t checksum;           // Checksum of short name
    uint16_t name2[6];          // Next 6 characters
    uint16_t first_clus;        // First cluster (always 0)
    uint16_t name3[2];          // Last 2 characters
} __attribute__((packed));

// FAT filesystem structure
struct fat_filesystem {
    struct vfs_filesystem vfs_fs;  // VFS filesystem structure
    struct fat_boot_sector boot;   // Boot sector
    uint32_t fat_start;            // FAT start sector
    uint32_t fat_size;             // FAT size in sectors
    uint32_t root_start;           // Root directory start sector (FAT16)
    uint32_t root_cluster;         // Root directory start cluster (FAT32)
    uint32_t data_start;           // Data area start sector
    uint32_t cluster_count;        // Total number of clusters
    uint32_t cluster_size;         // Cluster size in bytes
    uint32_t fat_type;             // FAT type (12, 16, or 32)
    uint8_t* fat;                  // FAT cache in memory
};

// FAT file structure
struct fat_file {
    struct vfs_file vfs_file;   // VFS file structure
    uint32_t first_cluster;     // First cluster of file
    uint32_t current_cluster;   // Current cluster
    uint32_t cluster_offset;    // Offset within current cluster
};

// Function prototypes
int fat_mount(struct vfs_filesystem* vfs_fs, void* device);
int fat_unmount(struct vfs_filesystem* vfs_fs);
int fat_open(struct vfs_file* vfs_file, const char* path, uint32_t flags);
int fat_close(struct vfs_file* vfs_file);
int fat_read(struct vfs_file* vfs_file, void* buffer, uint32_t size, uint32_t* bytes_read);
int fat_write(struct vfs_file* vfs_file, const void* buffer, uint32_t size, uint32_t* bytes_written);
int fat_seek(struct vfs_file* vfs_file, int32_t offset, int whence);
int fat_tell(struct vfs_file* vfs_file, uint32_t* position);
int fat_opendir(struct vfs_file* vfs_dir, const char* path);
struct vfs_dirent* fat_readdir(struct vfs_file* vfs_dir, uint32_t index);
int fat_stat(const char* path, struct vfs_dirent* entry);

#endif // FAT_H
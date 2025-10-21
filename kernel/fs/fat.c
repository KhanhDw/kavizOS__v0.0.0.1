#include "fat.h"
#include "../fs/vfs.h"
#include "../drivers/ata.h"
#include "../drivers/console.h"
#include "../memory.h"
#include <stdint.h>
#include <string.h>

// Global FAT operations structure
static struct vfs_filesystem_ops fat_ops = {
    .mount = fat_mount,
    .unmount = fat_unmount,
    .open = fat_open,
    .close = fat_close,
    .read = fat_read,
    .write = fat_write,
    .seek = fat_seek,
    .tell = fat_tell,
    .opendir = fat_opendir,
    .readdir = fat_readdir,
    .stat = fat_stat
};

// Helper function to convert cluster to sector
static uint32_t cluster_to_sector(struct fat_filesystem* fat_fs, uint32_t cluster) {
    if (cluster < 2) {
        return 0; // Invalid cluster
    }
    return fat_fs->data_start + ((cluster - 2) * fat_fs->boot.sectors_per_cluster);
}

// Helper function to read a cluster
static int read_cluster(struct fat_filesystem* fat_fs, uint32_t cluster, void* buffer) {
    uint32_t sector = cluster_to_sector(fat_fs, cluster);
    for (int i = 0; i < fat_fs->boot.sectors_per_cluster; i++) {
        if (!ata_read_sector(ATA_DEVICE_PRIMARY_MASTER, sector + i, (uint8_t*)buffer + (i * 512))) {
            return 0; // Read failed
        }
    }
    return 1; // Success
}

// Helper function to read FAT entry
static uint32_t read_fat_entry(struct fat_filesystem* fat_fs, uint32_t cluster) {
    if (fat_fs->fat_type == FAT_TYPE_FAT32) {
        uint32_t* fat = (uint32_t*)fat_fs->fat;
        return fat[cluster] & 0x0FFFFFFF; // Mask out upper 4 bits
    } else if (fat_fs->fat_type == FAT_TYPE_FAT16) {
        uint16_t* fat = (uint16_t*)fat_fs->fat;
        return fat[cluster];
    } else { // FAT12
        // For simplicity, we'll handle this case later
        return 0;
    }
}

// Helper function to lookup file in directory
static int lookup_file_in_dir(struct fat_filesystem* fat_fs, uint32_t dir_cluster, 
                              const char* filename, struct fat_dirent* result) {
    char fat_name[12];
    uint32_t current_cluster = dir_cluster;
    uint8_t cluster_buffer[4096]; // Assuming cluster size <= 4KB
    
    // Convert filename to FAT 8.3 format
    memset(fat_name, ' ', 11);
    fat_name[11] = '\0';
    
    const char* dot_pos = strchr(filename, '.');
    int name_len = dot_pos ? (dot_pos - filename) : strlen(filename);
    if (name_len > 8) name_len = 8;
    
    for (int i = 0; i < name_len; i++) {
        fat_name[i] = filename[i];
    }
    
    if (dot_pos) {
        int ext_len = strlen(dot_pos + 1);
        if (ext_len > 3) ext_len = 3;
        for (int i = 0; i < ext_len; i++) {
            fat_name[8 + i] = dot_pos[1 + i];
        }
    }
    
    // Convert to uppercase
    for (int i = 0; i < 11; i++) {
        if (fat_name[i] >= 'a' && fat_name[i] <= 'z') {
            fat_name[i] -= 32;
        }
    }
    
    while (current_cluster < fat_fs->cluster_count + 2) {
        if (!read_cluster(fat_fs, current_cluster, cluster_buffer)) {
            return 0;
        }
        
        struct fat_dirent* dir_entry = (struct fat_dirent*)cluster_buffer;
        for (int i = 0; i < (fat_fs->cluster_size / sizeof(struct fat_dirent)); i++) {
            if (dir_entry[i].name[0] == 0x00) {
                // End of directory
                return 0;
            }
            if (dir_entry[i].name[0] == 0xE5) {
                // Deleted entry, skip
                continue;
            }
            if (memcmp(dir_entry[i].name, fat_name, 11) == 0) {
                // Found the file
                *result = dir_entry[i];
                return 1;
            }
        }
        
        // Get next cluster in chain
        current_cluster = read_fat_entry(fat_fs, current_cluster);
        if (current_cluster >= 0x0FFFFFF8) { // End of chain
            break;
        }
    }
    
    return 0; // File not found
}

// Mount FAT filesystem
int fat_mount(struct vfs_filesystem* vfs_fs, void* device) {
    console_write("Mounting FAT filesystem...\n");
    
    struct fat_filesystem* fat_fs = (struct fat_filesystem*)vfs_fs;
    uint8_t boot_sector[512];
    
    // Read boot sector
    if (!ata_read_sector(ATA_DEVICE_PRIMARY_MASTER, 0, boot_sector)) {
        console_write("Failed to read boot sector\n");
        return 0;
    }
    
    // Copy boot sector to our structure
    memcpy(&fat_fs->boot, boot_sector, sizeof(struct fat_boot_sector));
    
    // Determine FAT type based on sector count and cluster count
    uint32_t root_dir_sectors = ((fat_fs->boot.fat16.root_entries * 32) + 
                                (fat_fs->boot.bytes_per_sector - 1)) / 
                                fat_fs->boot.bytes_per_sector;
    
    uint32_t fat_size = fat_fs->boot.fat_size_16;
    if (fat_fs->boot.fat_size_16 == 0) {
        fat_size = fat_fs->boot.fat32.fat_size_32;
    }
    
    uint32_t total_sectors = fat_fs->boot.total_sectors_16;
    if (total_sectors == 0) {
        total_sectors = fat_fs->boot.total_sectors_32;
    }
    
    fat_fs->fat_start = fat_fs->boot.reserved_sectors;
    fat_fs->root_start = fat_fs->fat_start + (fat_fs->boot.num_fats * fat_size);
    fat_fs->data_start = fat_fs->root_start + root_dir_sectors;
    fat_fs->fat_size = fat_size;
    
    // Determine FAT type
    uint32_t data_sectors = total_sectors - (fat_fs->boot.reserved_sectors + 
                                           (fat_fs->boot.num_fats * fat_size) + 
                                           root_dir_sectors);
    fat_fs->cluster_count = data_sectors / fat_fs->boot.sectors_per_cluster;
    fat_fs->cluster_size = fat_fs->boot.sectors_per_cluster * fat_fs->boot.bytes_per_sector;
    
    if (fat_fs->cluster_count < 4085) {
        fat_fs->fat_type = FAT_TYPE_FAT12;
    } else if (fat_fs->cluster_count < 65525) {
        fat_fs->fat_type = FAT_TYPE_FAT16;
    } else {
        fat_fs->fat_type = FAT_TYPE_FAT32;
        fat_fs->root_cluster = fat_fs->boot.fat32.root_cluster;
    }
    
    console_write("FAT type: ");
    if (fat_fs->fat_type == FAT_TYPE_FAT32) {
        console_write("FAT32\n");
    } else if (fat_fs->fat_type == FAT_TYPE_FAT16) {
        console_write("FAT16\n");
    } else {
        console_write("FAT12\n");
    }
    
    console_write("Cluster count: ");
    // Print cluster count (in hex would require hex printing function)
    console_write("\n");
    
    // Read FAT into memory
    fat_fs->fat = (uint8_t*)kmalloc(fat_size * fat_fs->boot.bytes_per_sector);
    if (!fat_fs->fat) {
        console_write("Failed to allocate memory for FAT\n");
        return 0;
    }
    
    for (uint32_t i = 0; i < fat_size; i++) {
        if (!ata_read_sector(ATA_DEVICE_PRIMARY_MASTER, fat_fs->fat_start + i, 
                            fat_fs->fat + (i * fat_fs->boot.bytes_per_sector))) {
            console_write("Failed to read FAT sector\n");
            kfree(fat_fs->fat);
            return 0;
        }
    }
    
    console_write("FAT filesystem mounted successfully\n");
    return 1;
}

// Unmount FAT filesystem
int fat_unmount(struct vfs_filesystem* vfs_fs) {
    struct fat_filesystem* fat_fs = (struct fat_filesystem*)vfs_fs;
    
    if (fat_fs->fat) {
        kfree(fat_fs->fat);
        fat_fs->fat = NULL;
    }
    
    console_write("FAT filesystem unmounted\n");
    return 1;
}

// Open file in FAT filesystem
int fat_open(struct vfs_file* vfs_file, const char* path, uint32_t flags) {
    struct fat_filesystem* fat_fs = (struct fat_filesystem*)vfs_file->filesystem;
    struct fat_file* fat_file = (struct fat_file*)vfs_file->private_data;
    
    // For now, just find the root directory cluster
    uint32_t root_cluster = fat_fs->root_cluster;
    if (fat_fs->fat_type != FAT_TYPE_FAT32) {
        // For FAT12/FAT16, root directory is in a specific location
        // This is a simplified implementation
        root_cluster = 0; // Placeholder
    }
    
    // Find the file in the directory structure
    struct fat_dirent dirent;
    if (!lookup_file_in_dir(fat_fs, root_cluster, path, &dirent)) {
        if (flags & VFS_MODE_CREATE) {
            // File doesn't exist and creation requested
            // For now, return failure as we don't implement file creation
            return 0;
        } else {
            // File doesn't exist
            return 0;
        }
    }
    
    // Set up the file structure
    strcpy(vfs_file->path, path);
    vfs_file->flags = flags;
    vfs_file->size = dirent.file_size;
    vfs_file->attributes = dirent.attr;
    
    // Set up FAT-specific file structure
    fat_file->first_cluster = (dirent.fst_clus_hi << 16) | dirent.fst_clus_lo;
    fat_file->current_cluster = fat_file->first_cluster;
    fat_file->cluster_offset = 0;
    
    return 1; // Success
}

// Close file in FAT filesystem
int fat_close(struct vfs_file* vfs_file) {
    // Nothing special to do for FAT
    return 1;
}

// Read from file in FAT filesystem
int fat_read(struct vfs_file* vfs_file, void* buffer, uint32_t size, uint32_t* bytes_read) {
    struct fat_filesystem* fat_fs = (struct fat_filesystem*)vfs_file->filesystem;
    struct fat_file* fat_file = (struct fat_file*)vfs_file->private_data;
    
    uint32_t total_bytes_read = 0;
    uint8_t cluster_buffer[4096]; // Assuming cluster size <= 4KB
    
    while (total_bytes_read < size && vfs_file->position < vfs_file->size) {
        // Calculate which cluster and offset we need
        uint32_t bytes_remaining_in_file = vfs_file->size - vfs_file->position;
        uint32_t bytes_to_read = size - total_bytes_read;
        if (bytes_to_read > bytes_remaining_in_file) {
            bytes_to_read = bytes_remaining_in_file;
        }
        
        // Read current cluster if we haven't already
        if (!read_cluster(fat_fs, fat_file->current_cluster, cluster_buffer)) {
            break;
        }
        
        // Calculate offset within the cluster
        uint32_t offset_in_cluster = vfs_file->position % fat_fs->cluster_size;
        uint32_t bytes_from_cluster = fat_fs->cluster_size - offset_in_cluster;
        if (bytes_from_cluster > bytes_to_read) {
            bytes_from_cluster = bytes_to_read;
        }
        
        // Copy data from cluster to user buffer
        memcpy((uint8_t*)buffer + total_bytes_read, 
               cluster_buffer + offset_in_cluster, 
               bytes_from_cluster);
        
        // Update position and bytes read
        vfs_file->position += bytes_from_cluster;
        total_bytes_read += bytes_from_cluster;
        
        // If we've reached the end of the current cluster, move to next
        if (offset_in_cluster + bytes_from_cluster >= fat_fs->cluster_size) {
            // Get next cluster in chain
            uint32_t next_cluster = read_fat_entry(fat_fs, fat_file->current_cluster);
            if (next_cluster >= 0x0FFFFFF8) { // End of chain
                break;
            }
            fat_file->current_cluster = next_cluster;
        }
    }
    
    *bytes_read = total_bytes_read;
    return 1; // Success
}

// Write to file in FAT filesystem (placeholder)
int fat_write(struct vfs_file* vfs_file, const void* buffer, uint32_t size, uint32_t* bytes_written) {
    // For now, just return error as we don't implement writing
    *bytes_written = 0;
    return 0;
}

// Seek in file
int fat_seek(struct vfs_file* vfs_file, int32_t offset, int whence) {
    uint32_t new_pos;
    
    switch (whence) {
        case 0: // SEEK_SET
            new_pos = offset;
            break;
        case 1: // SEEK_CUR
            new_pos = vfs_file->position + offset;
            break;
        case 2: // SEEK_END
            new_pos = vfs_file->size + offset;
            break;
        default:
            return 0; // Invalid whence
    }
    
    // Check bounds
    if (new_pos > vfs_file->size) {
        new_pos = vfs_file->size;
    }
    if (new_pos < 0) {
        new_pos = 0;
    }
    
    vfs_file->position = new_pos;
    return 1; // Success
}

// Tell current position in file
int fat_tell(struct vfs_file* vfs_file, uint32_t* position) {
    *position = vfs_file->position;
    return 1; // Success
}

// Open directory in FAT filesystem
int fat_opendir(struct vfs_file* vfs_dir, const char* path) {
    struct fat_filesystem* fat_fs = (struct fat_filesystem*)vfs_dir->filesystem;
    
    // For now, just set up to read root directory
    // In a complete implementation, we'd find the actual directory
    vfs_dir->private_data = (void*)fat_fs->root_cluster;
    
    strcpy(vfs_dir->path, path);
    vfs_dir->flags = VFS_MODE_READ;
    vfs_dir->position = 0;
    
    return 1; // Success
}

// Read directory entry
struct vfs_dirent* fat_readdir(struct vfs_file* vfs_dir, uint32_t index) {
    // For now, return NULL as we don't implement directory reading
    // This would require implementing directory traversal
    return NULL;
}

// Get file stats
int fat_stat(const char* path, struct vfs_dirent* entry) {
    // For now, return failure as we don't implement this
    return 0;
}
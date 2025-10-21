#include "ata.h"
#include "../drivers/port_io.h"
#include "../drivers/console.h"
#include <stdint.h>

// Global variables to track ATA devices
static int ata_primary_master_exists = 0;
static int ata_primary_slave_exists = 0;

// Wait for ATA device to be busy
void ata_wait_busy(void) {
    while (inb(ATA_REG_STATUS) & ATA_SR_BSY);
}

// Wait for ATA device to be ready
void ata_wait_ready(void) {
    while ((inb(ATA_REG_STATUS) & ATA_SR_BSY) || !(inb(ATA_REG_STATUS) & ATA_SR_DRDY));
}

// Send identify command to device
int ata_identify(uint8_t drive) {
    ata_wait_busy();
    
    // Select drive
    outb(ATA_REG_HDDEVSEL, drive);
    ata_wait_ready();
    
    // Write parameters for IDENTIFY command
    outb(ATA_REG_SECCOUNT0, 0);
    outb(ATA_REG_LBA0, 0);
    outb(ATA_REG_LBA1, 0);
    outb(ATA_REG_LBA2, 0);
    
    // Send IDENTIFY command
    outb(ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
    
    // Check if device exists
    if (inb(ATA_REG_STATUS) == 0) {
        return 0; // No device
    }
    
    // Wait for response
    ata_wait_ready();
    
    // Check for errors
    uint8_t status = inb(ATA_REG_STATUS);
    if (status & ATA_SR_ERR) {
        return 0; // Error
    }
    
    // Wait for data ready
    while (!(inb(ATA_REG_STATUS) & ATA_SR_DRQ));
    
    // Read 256 words of IDENTIFY data
    for (int i = 0; i < 256; i++) {
        inw(ATA_REG_DATA);
    }
    
    return 1; // Success
}

// Read a single sector from ATA device
int ata_read_sector(uint8_t drive, uint32_t lba, uint8_t* buffer) {
    ata_wait_busy();
    
    // Select drive
    outb(ATA_REG_HDDEVSEL, drive | ((lba >> 24) & 0x0F));
    ata_wait_ready();
    
    // Write parameters
    outb(ATA_REG_SECCOUNT0, 1); // Read 1 sector
    outb(ATA_REG_LBA0, (uint8_t)lba);
    outb(ATA_REG_LBA1, (uint8_t)(lba >> 8));
    outb(ATA_REG_LBA2, (uint8_t)(lba >> 16));
    
    // Send READ command
    outb(ATA_REG_COMMAND, ATA_CMD_READ_PIO);
    
    // Wait for response
    ata_wait_ready();
    
    // Check for errors
    uint8_t status = inb(ATA_REG_STATUS);
    if (status & ATA_SR_ERR) {
        return 0; // Error
    }
    
    // Wait for data ready
    while (!(inb(ATA_REG_STATUS) & ATA_SR_DRQ));
    
    // Read 256 words (512 bytes) of data
    for (int i = 0; i < 256; i++) {
        uint16_t data = inw(ATA_REG_DATA);
        buffer[i*2] = (uint8_t)data;
        buffer[i*2+1] = (uint8_t)(data >> 8);
    }
    
    return 1; // Success
}

// Write a single sector to ATA device
int ata_write_sector(uint8_t drive, uint32_t lba, const uint8_t* buffer) {
    ata_wait_busy();
    
    // Select drive
    outb(ATA_REG_HDDEVSEL, drive | ((lba >> 24) & 0x0F));
    ata_wait_ready();
    
    // Write parameters
    outb(ATA_REG_SECCOUNT0, 1); // Write 1 sector
    outb(ATA_REG_LBA0, (uint8_t)lba);
    outb(ATA_REG_LBA1, (uint8_t)(lba >> 8));
    outb(ATA_REG_LBA2, (uint8_t)(lba >> 16));
    
    // Send WRITE command
    outb(ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);
    
    // Wait for device ready
    ata_wait_ready();
    
    // Check for errors
    uint8_t status = inb(ATA_REG_STATUS);
    if (status & ATA_SR_ERR) {
        return 0; // Error
    }
    
    // Wait for data request
    while (!(inb(ATA_REG_STATUS) & ATA_SR_DRQ));
    
    // Write 256 words (512 bytes) of data
    for (int i = 0; i < 256; i++) {
        uint16_t data = (uint16_t)buffer[i*2] | ((uint16_t)buffer[i*2+1] << 8);
        outw(ATA_REG_DATA, data);
    }
    
    // Wait for completion
    ata_wait_ready();
    
    return 1; // Success
}

// Initialize ATA subsystem
void ata_init(void) {
    console_write("Initializing ATA subsystem...\n");
    
    // Detect primary master
    if (ata_identify(ATA_DEVICE_PRIMARY_MASTER)) {
        ata_primary_master_exists = 1;
        console_write("ATA Primary Master detected\n");
    } else {
        console_write("ATA Primary Master not detected\n");
    }
    
    // Detect primary slave
    if (ata_identify(ATA_DEVICE_PRIMARY_SLAVE)) {
        ata_primary_slave_exists = 1;
        console_write("ATA Primary Slave detected\n");
    } else {
        console_write("ATA Primary Slave not detected\n");
    }
    
    console_write("ATA initialization complete.\n");
}
#ifndef ATA_H
#define ATA_H

#include <stdint.h>

// ATA I/O Ports
#define ATA_PRIMARY_IO_BASE     0x1F0
#define ATA_PRIMARY_CTRL_BASE   0x3F6
#define ATA_SECONDARY_IO_BASE   0x170
#define ATA_SECONDARY_CTRL_BASE 0x376

// ATA Registers (Primary)
#define ATA_REG_DATA       0x1F0
#define ATA_REG_ERROR      0x1F1
#define ATA_REG_SECCOUNT0  0x1F2
#define ATA_REG_LBA0       0x1F3
#define ATA_REG_LBA1       0x1F4
#define ATA_REG_LBA2       0x1F5
#define ATA_REG_HDDEVSEL   0x1F6
#define ATA_REG_COMMAND    0x1F7
#define ATA_REG_STATUS     0x1F7
#define ATA_REG_CTRL       0x3F6
#define ATA_REG_ALTSTATUS  0x3F6

// ATA Commands
#define ATA_CMD_READ_PIO      0x20
#define ATA_CMD_WRITE_PIO     0x30
#define ATA_CMD_IDENTIFY      0xEC

// ATA Status Bits
#define ATA_SR_BSY     0x80
#define ATA_SR_DRDY    0x40
#define ATA_SR_DF      0x20
#define ATA_SR_DSC     0x10
#define ATA_SR_DRQ     0x08
#define ATA_SR_CORR    0x04
#define ATA_SR_IDX     0x02
#define ATA_SR_ERR     0x01

// ATA Error Bits
#define ATA_ER_BBK      0x80
#define ATA_ER_UNC      0x40
#define ATA_ER_MC       0x20
#define ATA_ER_IDNF     0x10
#define ATA_ER_MCR      0x08
#define ATA_ER_ABRT     0x04
#define ATA_ER_TK0NF    0x02
#define ATA_ER_AMNF     0x01

// ATA Device Types
#define ATA_DEVICE_PRIMARY_MASTER    0xE0
#define ATA_DEVICE_PRIMARY_SLAVE     0xF0
#define ATA_DEVICE_SECONDARY_MASTER  0xE0
#define ATA_DEVICE_SECONDARY_SLAVE   0xF0

// Function prototypes
void ata_init(void);
void ata_wait_busy(void);
void ata_wait_ready(void);
int ata_identify(uint8_t drive);
int ata_read_sector(uint8_t drive, uint32_t lba, uint8_t* buffer);
int ata_write_sector(uint8_t drive, uint32_t lba, const uint8_t* buffer);

#endif // ATA_H
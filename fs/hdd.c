/*
    hdd.c
    Copyright Menotdan 2018-2019

    HDD Setup

    MIT License
*/

#include <fs/hdd.h>

int ata_pio = 0;
uint16_t ata_drive = MASTER_DRIVE;
uint32_t ata_controler = PRIMARY_IDE;
int nodrives = 0;
uint16_t ata_buffer[256];
int mp = 1;
int sp = 1;
int ss = 1;
int ms = 1;
int mp28 = 1;
int sp28 = 1;
int ss28 = 1;
int ms28 = 1;
int mp48 = 1;
int sp48 = 1;
int ss48 = 1;
int ms48 = 1;
uint32_t sectors_read = 0;

int primary = 0;
int secondary = 0;
uint16_t tmp;
uint16_t readB;

void clear_ata_buffer() {
    for(int i=0; i<256; i++) {
        ata_buffer[i]=0;
    }
}

// Primary IRQ handler
static void primary_handler(registers_t *r) {
    sectors_read++;
    UNUSED(r);
}

// Secondary IRQ handler
static void secondary_handler(registers_t *r) {
    sectors_read++;
    UNUSED(r);
}

void init_hdd() {
    register_interrupt_handler(IRQ14, primary_handler);
    register_interrupt_handler(IRQ15, secondary_handler);
}

int ata_pio28(uint16_t base, uint8_t type, uint16_t drive, uint32_t addr) {
    if (nodrives == 0) {
        int cycle=0;
        port_byte_out(base+ATA_PORT_DRV, drive);
        //PIO28
        port_byte_out(base+ATA_PORT_FEATURES, 0x00);
        port_byte_out(base+ATA_PORT_SCT_COUNT, 0x01);
        port_byte_out(base+ATA_PORT_SCT_NUMBER, (unsigned char)addr);
        port_byte_out(base+ATA_PORT_CYL_LOW, (unsigned char)(addr >> 8));
        port_byte_out(base+ATA_PORT_CYL_HIGH, (unsigned char)(addr >> 16));
        //type
        if(type==ATA_READ) {
            port_byte_out(base+ATA_PORT_COMMAND, 0x20);  // Send command
        }
        else {
            port_byte_out(base+ATA_PORT_COMMAND, 0x30);
        }

        //wait for BSY clear and DRQ set
        cycle=0;
        for(int i=0; i<1000; i++) {
            port_byte_in(base+ATA_PORT_ALT_STATUS);  //Delay
            port_byte_in(base+ATA_PORT_ALT_STATUS);
            port_byte_in(base+ATA_PORT_ALT_STATUS);
            port_byte_in(base+ATA_PORT_ALT_STATUS);
            if( (port_byte_in(base+ATA_PORT_ALT_STATUS) & 0x88)==0x08 ) {  //drq is set
                cycle=1;
                break;    
            }    
        }
        if(cycle==0) {
            port_byte_in(base+ATA_PORT_ALT_STATUS); //Delay so the drive can set its port values
            port_byte_in(base+ATA_PORT_ALT_STATUS);
            port_byte_in(base+ATA_PORT_ALT_STATUS);
            port_byte_in(base+ATA_PORT_ALT_STATUS);
            if( (port_byte_in(base+ATA_PORT_ALT_STATUS) & 0x01)==0x01 ) {
                kprint("");
            } 
            return 1;
        }

        if( (port_byte_in(base+ATA_PORT_ALT_STATUS) & 0x01)==0x01 ) {
            kprint("");
            return 2;
        }

        for (int idx = 0; idx < 256; idx++)
        {
            if(type==ATA_READ) {
                ata_buffer[idx] = port_word_in(base + ATA_PORT_DATA);
            }
            else {
                port_word_out(base + ATA_PORT_DATA, ata_buffer[idx]);
            }
        }

        return 0;
    } else {
        kprint("No drives found on this system!");
        return 1;
    }
}

int ata_pio48(uint16_t base, uint8_t type, uint16_t drive, uint32_t addr) {
    if (nodrives == 0) {
        int cycle=0;
        port_byte_out(base+ATA_PORT_DRV, drive);
        //PIO48
        port_byte_out(base+ATA_PORT_FEATURES, 0x00); // NULL 1
        port_byte_out(base+ATA_PORT_FEATURES, 0x00); // NULL 2
        port_byte_out(base+ATA_PORT_SCT_COUNT, 0x0); // High sector count
        port_byte_out(base+ATA_PORT_SCT_NUMBER, 0); // High sector num
        port_byte_out(base+ATA_PORT_CYL_LOW, 0); // High low Cyl
        port_byte_out(base+ATA_PORT_CYL_HIGH, 0); // High high Cyl
        port_byte_out(base+ATA_PORT_SCT_COUNT, 0x01); // Low sector count
        port_byte_out(base+ATA_PORT_SCT_NUMBER, (unsigned char)addr); // Low sector num
        port_byte_out(base+ATA_PORT_CYL_LOW, (unsigned char)(addr >> 8)); // Low low Cyl
        port_byte_out(base+ATA_PORT_CYL_HIGH, (unsigned char)(addr >> 16)); // Low high Cyl
        //type
        if(type==ATA_READ) {
            port_byte_out(base+ATA_PORT_COMMAND, 0x24);  // Send command
        }
        else {
            port_byte_out(base+ATA_PORT_COMMAND, 0x34);
        }

        //wait for BSY clear and DRQ set
        cycle=0;
        for(int i=0; i<1000; i++) {
            port_byte_in(base+ATA_PORT_ALT_STATUS);  //Delay
            port_byte_in(base+ATA_PORT_ALT_STATUS);
            port_byte_in(base+ATA_PORT_ALT_STATUS);
            port_byte_in(base+ATA_PORT_ALT_STATUS);
            if( (port_byte_in(base+ATA_PORT_ALT_STATUS) & 0x88)==0x08 ) {  //drq is set
                cycle=1;
                break;    
            }    
        }
        if(cycle==0) {
            port_byte_in(base+ATA_PORT_ALT_STATUS); //Delay so the drive can set its port values
            port_byte_in(base+ATA_PORT_ALT_STATUS);
            port_byte_in(base+ATA_PORT_ALT_STATUS);
            port_byte_in(base+ATA_PORT_ALT_STATUS);
            if( (port_byte_in(base+ATA_PORT_ALT_STATUS) & 0x01)==0x01 ) {
                kprint("");
            } 
            return 1;
        }

        if( (port_byte_in(base+ATA_PORT_ALT_STATUS) & 0x01)==0x01 ) {
            kprint("");
            return 2;
        }

        if(type==ATA_READ) {
            for (int idx = 0; idx < 256; idx++)
            {
                ata_buffer[idx] = port_word_in(base + ATA_PORT_DATA);
            }
        }
        else {
            for (int idx = 0; idx < 256; idx++)
            {
                port_word_out(base + ATA_PORT_DATA, ata_buffer[idx]);
            }
        }
        return 0;
    } else {
        kprint("No drives found on this system!");
        return 1;
    }
}

void new_scan() {
    mp = ata_pio28(0x1F0, ATA_READ, 0xE0, 0x0);
    ms = ata_pio28(0x1F0, ATA_READ, 0xF0, 0x0);
    sp = ata_pio28(0x170, ATA_READ, 0xE0, 0x0);
    ss = ata_pio28(0x170, ATA_READ, 0xF0, 0x0);
    mp48 = ata_pio48(0x1F0, ATA_READ, 0x40, 0x0);
    ms48 = ata_pio48(0x1F0, ATA_READ, 0x50, 0x0);
    sp48 = ata_pio48(0x170, ATA_READ, 0x40, 0x0);
    ss48 = ata_pio48(0x170, ATA_READ, 0x50, 0x0);
    // PIO28 Drives
    if (mp == 0) {
        ata_drive = MASTER_DRIVE;
        ata_controler = PRIMARY_IDE;
        ata_pio = 0;
    }
    else if (ms == 0) {
        ata_drive = SLAVE_DRIVE;
        ata_controler = PRIMARY_IDE;
        ata_pio = 0;
    }
    else if (sp == 0) {
        ata_drive = MASTER_DRIVE;
        ata_controler = SECONDARY_IDE;
        ata_pio = 0;
    }
    else if (ss == 0) {
        ata_drive = SLAVE_DRIVE;
        ata_controler = SECONDARY_IDE;
        ata_pio = 0;
    }
    // PIO48 Drives
    else if (mp48 == 0) {
        ata_drive = MASTER_DRIVE_PIO48;
        ata_controler = PRIMARY_IDE;
        ata_pio = 1;
    }
    else if (ms48 == 0) {
        ata_drive = SLAVE_DRIVE_PIO48;
        ata_controler = PRIMARY_IDE;
        ata_pio = 1;
    }
    else if (sp48 == 0) {
        ata_drive = MASTER_DRIVE_PIO48;
        ata_controler = SECONDARY_IDE;
        ata_pio = 1;
    }
    else if (ss48 == 0) {
        ata_drive = SLAVE_DRIVE_PIO48;
        ata_controler = SECONDARY_IDE;
        ata_pio = 1;
    } else {
        clear_ata_buffer();
        nodrives = 1;
        return;
    }
    clear_ata_buffer();
    return;
}

void drive_scan() {
    // Detect primary controller
    port_byte_out(ATA_PORT_PRIMARY_DETECT, MAGIC_DETECT);
    readB = port_byte_in(ATA_PORT_PRIMARY_DETECT);
    if (readB == 0x88) {
        // Primary controller exists
        primary = 1;
        // Detect Master Drive
        port_byte_out(ATA_PORT_PRIMARY_DRIVE_DETECT, MASTER_DRIVE_DETECT);
        wait(1);
        tmp = port_byte_in(ATA_PORT_PRIMARY_STATUS);
        if (tmp & 0x40) {
            // Master drive exists
            mp = 0;
            mp28 = ata_pio28(0x1F0, ATA_READ, 0xE0, 0x0);
            mp48 = ata_pio48(0x1F0, ATA_READ, 0x40, 0x0);
            if (mp28 == 0) {
                ata_drive = MASTER_DRIVE;
                ata_controler = PRIMARY_IDE;
                ata_pio = 0;
                current_drive = 1;
            } else if (mp48 == 0) {
                ata_drive = MASTER_DRIVE_PIO48;
                ata_controler = PRIMARY_IDE;
                ata_pio = 1;
                current_drive = 1;
            } else {
                // Default to PIO28
                ata_drive = MASTER_DRIVE;
                ata_controler = PRIMARY_IDE;
                ata_pio = 0;
                current_drive = 1;
            }
        }
        // Detect Slave Drive
        port_byte_out(ATA_PORT_PRIMARY_DRIVE_DETECT, SLAVE_DRIVE_DETECT);
        wait(1);
        //uint16_t tmp;
        tmp = port_byte_in(ATA_PORT_PRIMARY_STATUS);
        if (tmp & 0x40) {
            // Slave drive exists
            ms = 0;
            ms28 = ata_pio28(0x1F0, ATA_READ, 0xF0, 0x0);
            ms48 = ata_pio48(0x1F0, ATA_READ, 0x50, 0x0);
            if (ms28 == 0) {
                ata_drive = SLAVE_DRIVE;
                ata_controler = PRIMARY_IDE;
                ata_pio = 0;
                current_drive = 2;
            } else if (ms48 == 0) {
                ata_drive = SLAVE_DRIVE_PIO48;
                ata_controler = PRIMARY_IDE;
                ata_pio = 1;
                current_drive = 2;
            } else {
                // Default to PIO28
                ata_drive = SLAVE_DRIVE;
                ata_controler = PRIMARY_IDE;
                ata_pio = 0;
                current_drive = 2;
            }
        }
    }
    // Detect secondary controller
    port_byte_out(ATA_PORT_SECONDARY_DETECT, MAGIC_DETECT);
    readB = port_byte_in(ATA_PORT_SECONDARY_DETECT);
    if (readB == 0x88) {
        // Secondary controller exists
        secondary = 1;
        // Detect Master Drive
        port_byte_out(ATA_PORT_SECONDARY_DRIVE_DETECT, MASTER_DRIVE_DETECT);
        wait(1);
        tmp = port_byte_in(ATA_PORT_SECONDARY_STATUS);
        if (tmp & 0x40) {
            // Master drive exists
            sp = 0;
            sp28 = ata_pio28(0x170, ATA_READ, 0xE0, 0x0);
            sp48 = ata_pio48(0x170, ATA_READ, 0x40, 0x0);
            if (sp28 == 0) {
                ata_drive = MASTER_DRIVE;
                ata_controler = SECONDARY_IDE;
                ata_pio = 0;
                current_drive = 3;
            } else if (sp48 == 0) {
                ata_drive = MASTER_DRIVE_PIO48;
                ata_controler = SECONDARY_IDE;
                ata_pio = 1;
                current_drive = 3;
            } else {
                // Default to PIO28
                ata_drive = MASTER_DRIVE;
                ata_controler = SECONDARY_IDE;
                ata_pio = 0;
                current_drive = 3;
            }
        }
        // Detect Slave Drive
        port_byte_out(ATA_PORT_SECONDARY_DRIVE_DETECT, SLAVE_DRIVE_DETECT);
        wait(1);
        tmp = port_byte_in(ATA_PORT_SECONDARY_STATUS);
        if (tmp & 0x40) {
            // Slave drive exists
            ss = 0;
            ss28 = ata_pio28(0x170, ATA_READ, 0xF0, 0x0);
            ss48 = ata_pio48(0x170, ATA_READ, 0x50, 0x0);
            if (ss28 == 0) {
                ata_drive = SLAVE_DRIVE;
                ata_controler = SECONDARY_IDE;
                ata_pio = 0;
                current_drive = 4;
            } else if (ss48 == 0) {
                ata_drive = SLAVE_DRIVE_PIO48;
                ata_controler = SECONDARY_IDE;
                ata_pio = 1;
                current_drive = 4;
            } else {
                // Default to PIO28
                ata_drive = SLAVE_DRIVE;
                ata_controler = SECONDARY_IDE;
                ata_pio = 0;
                current_drive = 4;
            }
        }
    }
    clear_ata_buffer();
    if (mp == 0) {
        if (mp28 == 0) {
            ata_drive = MASTER_DRIVE;
            ata_controler = PRIMARY_IDE;
            ata_pio = 0;
        } else if (mp48 == 0) {
            ata_drive = MASTER_DRIVE_PIO48;
            ata_controler = PRIMARY_IDE;
            ata_pio = 1;
        } else {
            // Default to PIO28
            ata_drive = MASTER_DRIVE;
            ata_controler = PRIMARY_IDE;
            ata_pio = 0;
        }
        return;
    } else if (ms == 0) {
        return;
    } else if (sp == 0) {
        return;
    } else if (ss == 0) {
        return;
    } else {
        nodrives = 1;
        return;
    }

}

hdd_size_t drive_sectors(uint8_t devP, uint8_t controllerP) {
    hdd_size_t size;
    uint16_t controller = 0x170 + controllerP*0x80;
    uint16_t deviceBit = (devP << 4) + (1 << 6);
    if (deviceBit == 0) {

    }
    read(0, 0); // Start drive
    while ((port_byte_in(controller+7) & 0x40) == 0); // Wait for the drive to be ready
    if (ata_pio == 0) {
        port_byte_out(controller + 7, 0xF8); // Send the command
        while ((port_byte_in(controller + 7) & 0x80) != 0); // Wait for BSY to clear
        size.MAX_LBA = (uint32_t)port_byte_in(controller+3);
        size.MAX_LBA += (uint32_t)port_byte_in(controller+4) <<8;
        size.MAX_LBA += (uint32_t)port_byte_in(controller+5) <<16;
        size.MAX_LBA += ((uint32_t)port_byte_in(controller+6) & 0xF) <<24;
    } else {
        port_byte_out(controller + 7, 0x27); // Send the command
        while ((port_byte_in(controller + 7) & 0x80) != 0); // Wait for BSY to clear
        size.MAX_LBA =  (uint32_t)port_byte_in(controller+3);
        size.MAX_LBA += (uint32_t)port_byte_in(controller+4) <<8;
        size.MAX_LBA += (uint32_t)port_byte_in(controller+5) <<16;

        port_byte_out(controller+2, 0x80); // Set HOB to 1

        size.MAX_LBA += (uint32_t)port_byte_in(controller+3)<<24;
        size.MAX_LBA_HIGH = (uint32_t)port_byte_in(controller+4);
        size.MAX_LBA_HIGH += (uint32_t)port_byte_in(controller+5) << 8;
        size.HIGH_USED = 1;
    }
    return size;
}
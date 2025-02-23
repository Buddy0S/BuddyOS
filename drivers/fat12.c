#include "mmc.h"
#include "reg.h"
#include <stdint.h>
#include "uart.h"
#include "fat12.h"

fat_bs_t bootSector;
fat12_ebs_t extendedBootRecord;

int compareFileNames(DirEntry *entry, volatile char* fileName) {
    
    int i, j;

    for (i = 0; i < 8; i++) {
        
        if (fileName[i] != '.' && entry->name[i] != fileName[i]) {
            return 0;
        }
        else if (fileName[i] == '.') {
            break;
        }
    }

    for (j = 0; j < 3; j++) {
        if (entry->ext[j] != fileName[i+1 + j]) {
            return 0;
        }
    }

    return 1;

}

void fat12_init(unsigned int startSector, volatile uint32_t* buffer) {
	MMCreadblock(startSector, buffer);

	unsigned char* buf = (unsigned char*) buffer;

	for (int i = 0; i < 3; i++) {
		bootSector.bootJmp[i] = buf[i];
	}
	
	for (int i = 0; i < 8; i++) {
		bootSector.oemName[i] = buf[3 + i];
	}

	bootSector.bytesPerSector = buf[11] | (buf[12] << 8);
	bootSector.sectorsPerCluster = buf[13];
	bootSector.reservedSectorCount = buf[14] | (buf[15] << 8);
	bootSector.FATTableCount = buf[16];
	bootSector.rootEntryCount = buf[17] | (buf[18] << 8);
	bootSector.totalSectorCount = buf[19] | (buf[20] << 8);
	bootSector.mediaType = buf[21];
	bootSector.sectorsPerFATTable = buf[22] | (buf[23] << 8);
	bootSector.sectorsPerTrack = buf[24] | (buf[25] << 8);
	bootSector.headCountOnMedia = buf[26] | (buf[27] << 8);
	bootSector.hiddenSectorCount = buf[28] | (buf[29] << 8) | (buf[30] << 16) | (buf[31] << 24);
	bootSector.largeSectorCount = buf[32] | (buf[33] << 8) | (buf[34] << 16) | (buf[35] << 24);

	for (int i = 0; i < 54; i++) {
		bootSector.extendedBootRecord[i] = buf[36 + i];
	}

	extendedBootRecord.driverNumber = bootSector.extendedBootRecord[0];
	extendedBootRecord.reserved1 = bootSector.extendedBootRecord[1];
	extendedBootRecord.bootSignature = bootSector.extendedBootRecord[2];
	extendedBootRecord.volumeID = bootSector.extendedBootRecord[3] | 
		(bootSector.extendedBootRecord[4] << 8) | 
		(bootSector.extendedBootRecord[5] << 16) | 										(bootSector.extendedBootRecord[6] << 24);

	for (int i = 0; i < 11; i++) {
		extendedBootRecord.volumeLabel[i] = bootSector.extendedBootRecord[7 + i];
	}
		
	for (int i = 0; i < 8; i++) {
		extendedBootRecord.FATTypeLabel[i] = bootSector.extendedBootRecord[18 + i];
	}
/*
    uart0_printf("OEM = %s\n", bootSector.oemName);
    uart0_printf("Bytes per sector = %d\n", bootSector.bytesPerSector);
    uart0_printf("Sectors per cluster = %d\n", bootSector.sectorsPerCluster);
    uart0_printf("REC = %d\n", bootSector.rootEntryCount);
*/
}

int fat12_find(volatile char* filename, volatile uint32_t* buffer,
    volatile uint16_t *startCluster, volatile uint32_t *fileSize) {

    uint32_t rootSectorStart = bootSector.reservedSectorCount +
                (bootSector.FATTableCount * bootSector.sectorsPerFATTable);

    uint32_t numRootSectors = (bootSector.rootEntryCount * 32) / 512;

    DirEntry dirEntry;

    uart0_printf("Args = %s %d %d\n", filename, *startCluster, *fileSize);

    uart0_printf("start = %d, numSectors = %d\n", rootSectorStart,
    numRootSectors);

    /* Iterate through each sector */
    for (uint32_t i = rootSectorStart; i < rootSectorStart + numRootSectors;
        i++) {

        MMCreadblock(i, buffer);
        uart0_printf("Reading Sector %d ...\n", i);

        char *buf = (char*)buffer;

        /* In FAT12, each sector has 16 directory entries */
        for (int j = 0; j < 16; j++) {

            /* Each directory entry in FAT 12 is 32 bytes long */
            /* See DirEntry struct for fields in directory entry */
            dirEntry = *((DirEntry*) &buf[j * 32]);

            /* Check for directory end */
            if (dirEntry.name[0] == 0x00) {
                break; 
            }

            /* Check for valid entry */
            if (dirEntry.name[0] == 0xE5) {
                continue;
            }
            
            /* File found - set passed pointers to appropriate values*/
            if (compareFileNames(&dirEntry, filename)) {
                *startCluster = dirEntry.firstClusterLow;// do we needd  this 2/20 9:21 pm Noah | (dirEntry.firstClusterHigh << 8);
                *fileSize = dirEntry.fileSize;
                uart0_printf("fat12_find - RETURNING 1\n");
                return 1;
            }

        }

    }
    
    uart0_printf("fat12_find - RETURNING 0\n");
    return 0;
}

/* //wiki.osdev.org/FAT#FAT_12 */
uint16_t fat12_get_next_cluster(uint16_t cluster) {
	uart0_printf("Entered getNextCluster\n");
	uint16_t nextCluster;
	
	uint32_t FATTable[bootSector.bytesPerSector * 2]; /*may need to straddle a sector due to 12 bits */
	uint32_t fatStart = bootSector.reservedSectorCount;
	uint32_t fatOffset = cluster + (cluster / 2); /*1.5 bytes per entry */
	uint32_t fatSector = fatStart + (fatOffset / bootSector.bytesPerSector);
	uint32_t fatByteOffset = fatOffset % bootSector.bytesPerSector;

	MMCreadblock(fatSector, FATTable);

	if (cluster % 2 == 0) {
		nextCluster = (FATTable[fatByteOffset] | ((FATTable[fatByteOffset + 1] & 0x0F) << 8)) & 0xFFF;
	} else {
		nextCluster = ((FATTable[fatByteOffset] >> 4) | (FATTable[fatByteOffset + 1] << 4)) & 0xFFF;
	}

    	return nextCluster;
}

/* //wiki.osdev.org/FAT#FAT_12 */
uint32_t fat12_read_file(volatile char* filename, volatile uint32_t* buffer) {
	uart0_printf("Entered read file\n");
	uint32_t sectorRead; /* sector to start reading from */
	uint32_t rootSectorStart = bootSector.reservedSectorCount +
                (bootSector.FATTableCount * bootSector.sectorsPerFATTable);

	uint16_t startCluster;
	uint32_t fileSize;

	if (fat12_find(filename, buffer, &startCluster, &fileSize) == 0) {
		return -1;
	}

   	uint32_t numRootSectors = (bootSector.rootEntryCount * 32) / 512;
	uint32_t bytesRead = 0;
	uint16_t loopCluster = startCluster;

	/* read until EOF marker */
	while (loopCluster < FAT12_EOF_MIN) {
		/* reads from first data sector available */
		//uart0_printf("Entered loopCluster loop\n");
		sectorRead = rootSectorStart + numRootSectors + ((loopCluster-2) * bootSector.sectorsPerCluster);
		//uart0_printf("rootSectorStart = %d\n", rootSectorStart);
		//uart0_printf("numRootSectors = %d\n", numRootSectors);
		//uart0_printf("loopCluster = %d\n", loopCluster);
		//uart0_printf("sectorsPerCluster = %d\n", bootSector.sectorsPerCluster);
		//uart0_printf("Calculated sectorRead = %d\n", sectorRead);

		/* need to divide bytesRead/4 to convert to pointer index */
		MMCreadblock(sectorRead, buffer + bytesRead / 4);
        uart0_printf("Addr: %x\n", buffer + bytesRead / 4);

		/* updates bytes read */
		bytesRead += bootSector.bytesPerSector * bootSector.sectorsPerCluster;
		uart0_printf("bytesRead = %d\n", bytesRead);

		if (bytesRead > fileSize) {
			uart0_printf("breaking\n");
			break;
		}	
				
		loopCluster = fat12_get_next_cluster(loopCluster);
	}
	uart0_printf("File read complete\n");
    return bytesRead;

}

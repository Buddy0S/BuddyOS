#include "mmc.h"
#include "reg.h"
#include <stdint.h>
#include "uart.h"
#include "fat12.h"
#include "memory.h"

fat_bs_t bootSector;
fat12_ebs_t extendedBootRecord;

int compareFileNames(DirEntry *entry, const char* fileName) {
    
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

void splitFilename(const char* input, char* filename, char* extension) {
	
	int i = 0;
	int dotLoc = -1;
	int nameLen = 0;
	int extLen = 0;

	/* Search for dot */
	while (input[i] != '\0') {
		if (input[i] == '.') {
			dotLoc = i;
			break;
		}
		i++;
	}

	/* Extract file name */
	if (dotLoc == -1) {
		while (input[nameLen] != '\0' && nameLen < 8) {
			filename[nameLen] = input[nameLen];
			nameLen++;
		}
	}
	else {
		while (nameLen < dotLoc && nameLen < 8) {
			filename[nameLen] = input[nameLen];
			nameLen++;
		}
	}

	/* Pad filename with spaces if necessary */
	while (nameLen < 8) {
		filename[nameLen] = ' ';
		nameLen++;
	}

	/* Extract extension */
	if (dotLoc != -1) {
		i = dotLoc + 1;
		while (input[i] != '\0' && extLen < 3) {
			extension[extLen] = input[i];
			extLen++;
			i++;
		}
	}

	/* Pad extension with spaces if necessary */
	while (extLen < 3) {
		extension[extLen] = ' ';
		extLen++;
	}

}

void fat12_init(unsigned int startSector, uint32_t* buffer) {
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

/* Return Dir Sector that file is in */
int fat12_find(const char* filename, uint32_t* buffer, uint32_t* entryIndex) {

    uint32_t rootSectorStart = bootSector.reservedSectorCount +
                (bootSector.FATTableCount * bootSector.sectorsPerFATTable);

    uint32_t numRootSectors = (bootSector.rootEntryCount * 32) / 512;

    DirEntry tempEntry;

    //uart0_printf("Args = %s %d %d\n", filename, *startCluster, *fileSize);

    //uart0_printf("start = %d, numSectors = %d\n", rootSectorStart,
    //numRootSectors);

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
            tempEntry = *((DirEntry*) &buf[j * 32]);

            /* Check for directory end */
            if (tempEntry.name[0] == 0x00) {
                break; 
            }

            /* Check for valid entry */
            if (tempEntry.name[0] == 0xE5) {
                continue;
            }
            
            /* File found - set passed pointers to appropriate values*/
            if (compareFileNames(&tempEntry, filename)) {
                *entryIndex = j;
                uart0_printf("fat12_find - RETURNING 1\n");
                return i;
            }

        }

    }
    
    uart0_printf("fat12_find - RETURNING 0\n");
    return 0;
}

/* //wiki.osdev.org/FAT#FAT_12 */

uint16_t fat12_get_next_cluster(uint16_t cluster) {
	//uart0_printf("Entered getNextCluster\n");
	uint16_t nextCluster;
	
	uint8_t FATTable[bootSector.bytesPerSector * 2]; /*may need to straddle a sector due to 12 bits */
	uint32_t fatStart = bootSector.reservedSectorCount;
	uint32_t fatOffset = cluster + (cluster / 2); /*1.5 bytes per entry */
	uint32_t fatSector = fatStart + (fatOffset / bootSector.bytesPerSector);
	uint32_t fatByteOffset = fatOffset % bootSector.bytesPerSector;

	MMCreadblock(fatSector, (uint32_t*)FATTable);

	if (cluster % 2 == 0) {
		nextCluster = (FATTable[fatByteOffset] | ((FATTable[fatByteOffset + 1]) << 8)) & 0xFFF;
	} else {
		nextCluster = ((FATTable[fatByteOffset] >> 4) | (FATTable[fatByteOffset + 1] << 4)) & 0xFFF;
	}

	//uart0_printf("Next Cluster = %d\n",nextCluster);

	return nextCluster;
}

void fat12_set_next_cluster(uint16_t cluster, uint16_t nextCluster) {
	
	uint8_t FATTable[bootSector.bytesPerSector * 2]; /* May need to straddle a sector due to 12 bits */
    uint32_t fatStart = bootSector.reservedSectorCount;

    // Calculate the correct FAT offset for the cluster
    uint32_t fatOffset = cluster + (cluster / 2); // Equivalent to (cluster * 3) / 2

    // Calculate the sector and byte offset within the sector
    uint32_t fatSector = fatStart + (fatOffset / bootSector.bytesPerSector);
    uint32_t fatByteOffset = fatOffset % bootSector.bytesPerSector;

    // Read the FAT sector(s) into memory
    MMCreadblock(fatSector, (uint32_t*)FATTable);

    // Modify the FAT entry for the cluster
    if (cluster % 2 == 0) {
        // Even cluster: lower 12 bits of the 16-bit word
        FATTable[fatByteOffset] = nextCluster & 0xFF;
        FATTable[fatByteOffset + 1] = (FATTable[fatByteOffset + 1] & 0xF0) | ((nextCluster >> 8) & 0x0F);
    } else {
        // Odd cluster: upper 12 bits of the 16-bit word
        FATTable[fatByteOffset] = (FATTable[fatByteOffset] & 0x0F) | ((nextCluster << 4) & 0xF0);
        FATTable[fatByteOffset + 1] = (nextCluster >> 4) & 0xFF;
    }

    // Write the modified FAT sector(s) back to the storage device
    MMCwriteblock(fatSector, (uint32_t*)FATTable);
}

void fat12_print_cluster_chain(uint16_t firstCluster) {
    uint16_t cluster = firstCluster;
    uint16_t nextCluster;

    uart0_printf("Cluster chain for file (starting at cluster %d):\n", cluster);

    while (cluster < FAT12_EOF_MIN || cluster > FAT12_EOF_MAX) {
        uart0_printf("%d -> ", cluster);

        // Get the next cluster in the chain
        nextCluster = fat12_get_next_cluster(cluster);

        // Check for invalid cluster or end of chain
        if (nextCluster == 0xFFFF || nextCluster >= FAT12_EOF_MIN) {
            uart0_printf("%d\n", nextCluster);
            break;
        }

        cluster = nextCluster;
    }

    if (cluster >= FAT12_EOF_MIN && cluster <= FAT12_EOF_MAX) {
        uart0_printf("EOF\n");
    }
}

uint16_t fat12_find_free_cluster() {
	
	uint16_t cluster = 2;	/* clusters 0 and 1 are reserved **/
	uint8_t FATTable[bootSector.bytesPerSector * 2]; /*may need to straddle a sector due to 12 bits */
	uint32_t fatSector = bootSector.reservedSectorCount;
	uint32_t fatByteOffset = 0;

	for (uint32_t sector = 0; sector < bootSector.sectorsPerFATTable; sector++) {
		
		MMCreadblock(fatSector + sector, (uint32_t*)FATTable);

		/* Iterate through the current FAT sector */
        for (fatByteOffset = 0; fatByteOffset < bootSector.bytesPerSector; fatByteOffset += 3) {
            
			/* Read two 12 bit FAT entries */
            uint16_t entry1 = (FATTable[fatByteOffset] | (FATTable[fatByteOffset + 1] << 8)) & 0xFFF;
            uint16_t entry2 = ((FATTable[fatByteOffset + 1] >> 4) | (FATTable[fatByteOffset + 2] << 4)) & 0xFFF;

            /* Check if the first entry is free */
            if (entry1 == FAT12_UNUSED) {
				FATTable[fatByteOffset] = (FAT12_EOF_MAX & 0xFF);
                FATTable[fatByteOffset + 1] = (FATTable[fatByteOffset + 1] & 0xF0) | ((FAT12_EOF_MAX >> 8) & 0x0F);
                
				MMCwriteblock(fatSector + sector, (uint32_t*)FATTable);
				return cluster;
            }
            cluster++;

            /* Check if the second entry is free */
            if (entry2 == FAT12_UNUSED) {
				FATTable[fatByteOffset + 1] = (FATTable[fatByteOffset + 1] & 0x0F) | ((FAT12_EOF_MAX << 4) & 0xF0);
                FATTable[fatByteOffset + 2] = (FAT12_EOF_MAX >> 4) & 0xFF;

				MMCwriteblock(fatSector + sector, (uint32_t*)FATTable);
                return cluster;
            }
            cluster++;
        }
	}
	return 0xFFFF; /* No clusters found */

}

/* //wiki.osdev.org/FAT#FAT_12 */
uint32_t fat12_read_file(const char* filename, uint32_t* buffer, uint32_t* tempBuffer) {
	uart0_printf("Entered read file\n");
	uint32_t sectorRead; /* sector to start reading from */
	uint32_t rootSectorStart = bootSector.reservedSectorCount +
                (bootSector.FATTableCount * bootSector.sectorsPerFATTable);
	uint32_t entryIndex;
	DirEntry dirEntry;

	if (fat12_find(filename, tempBuffer, &entryIndex) == 0) {
		return -1;
	}

	dirEntry = ((DirEntry*)tempBuffer)[entryIndex];

   	uint32_t numRootSectors = (bootSector.rootEntryCount * 32) / 512;
	uint32_t bytesRead = 0;
	uint16_t loopCluster = dirEntry.firstClusterLow;
	uint32_t fileSize = dirEntry.fileSize;

	fat12_print_cluster_chain(loopCluster);

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
		//uart0_printf("bytesRead = %d\n", bytesRead);

		if (bytesRead > fileSize) {
			//uart0_printf("breaking\n");
			break;
		}	
				
		loopCluster = fat12_get_next_cluster(loopCluster);
	}
	uart0_printf("File read complete\n");
    return bytesRead;

}

uint32_t fat12_create_dir_entry(const char* filename,
	uint16_t parent_dir_sector, uint8_t attributes, uint32_t* buffer) {

	char* buf = (char*)buffer; /* CHANGE TO MALLOC'D ARRAY WHEN POSSIBLE */

    DirEntry *dirEntry;

	MMCreadblock(parent_dir_sector, (uint32_t*)buf);

	/* In FAT12, each sector has 16 directory entries */
	for (int j = 0; j < 16; j++) {

		/* Each directory entry in FAT 12 is 32 bytes long */
		/* See DirEntry struct for fields in directory entry */
		dirEntry = (DirEntry*) &buf[j * 32];

		/* Check for directory end */
		if (dirEntry->name[0] == 0x00 || dirEntry->name[0] == 0xE5) {
			
			splitFilename(filename, dirEntry->name, dirEntry->ext);			
			dirEntry->firstClusterLow = fat12_find_free_cluster();
			fat12_set_next_cluster(dirEntry->firstClusterLow, FAT12_EOF_MAX);
			dirEntry->attrib = attributes;
			dirEntry->fileSize = 0;

			uart0_printf("%s.%s - %x - %d (%d)\n", dirEntry->name, dirEntry->ext,
			dirEntry->attrib, dirEntry->firstClusterLow,
			fat12_get_next_cluster(dirEntry->firstClusterLow));

			MMCwriteblock(parent_dir_sector, (uint32_t*)buf);
			return dirEntry->firstClusterLow;
		}

	}

	return -1;

}

uint32_t fat12_write_file(const char* filename, char* data, uint32_t size, 
	uint32_t* tempBuffer) {

	uart0_printf("GOT TO WRITE FILE\n");

	DirEntry *fileEntry;
	uint16_t cluster, prevCluster, k; 
	uint32_t bytesWritten = 0;
	uint32_t sector, entryIndex;
	uint32_t dirSector = fat12_find(filename, tempBuffer, &entryIndex);
	uint32_t firstDataSector = bootSector.reservedSectorCount +
		(bootSector.FATTableCount * bootSector.sectorsPerFATTable) +
		((bootSector.rootEntryCount * 32) / bootSector.bytesPerSector);

	char temp[32];

	/* File not found */
	if (dirSector == 0) {
		return -1;
	}

	MMCreadblock(dirSector, tempBuffer);

	fileEntry = &((DirEntry*)tempBuffer)[entryIndex];

	uart0_printf("writing to %s\n", fileEntry->name);

	cluster = fileEntry->firstClusterLow;

	k = cluster;

	while (bytesWritten < size) {

		uart0_printf("doing cluster %d, bytes written %d\n", cluster,
			bytesWritten);

		if (cluster >= FAT12_EOF_MIN && cluster <= FAT12_EOF_MAX) {
			cluster = fat12_find_free_cluster();

			uart0_printf("prevEnd = %d, cluster = %d\n", prevCluster, cluster);

			fat12_set_next_cluster(prevCluster, cluster);
			fat12_set_next_cluster(cluster, FAT12_EOF_MAX);
		}

		sector = firstDataSector + ((cluster - 2) * bootSector.sectorsPerCluster);

		MMCwriteblock(sector, (uint32_t*)(data + bytesWritten));
	
		bytesWritten += bootSector.bytesPerSector * bootSector.sectorsPerCluster;
		prevCluster = cluster;
		cluster = fat12_get_next_cluster(cluster);

	}

	fat12_print_cluster_chain(k);

	fileEntry->fileSize = size;

	MMCwriteblock(dirSector, tempBuffer);

	return bytesWritten;
}

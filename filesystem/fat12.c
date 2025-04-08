#include "mmc.h"
#include "reg.h"
#include <stdint.h>
#include "uart.h"
#include "fat12.h"
#include "memory.h"

fat_bs_t bootSector;
fat12_ebs_t extendedBootRecord;


/*
 * Compares a FAT12 directory entry's name and extension with a given filename
 *
 * Args:
 *      DirEntry* entry: Pointer to the directory entry to compare
 *      const char* fileName: filename string in the format "NAME.EXT"
 *
 * Returns 1 if the directory entry matches the filename
 * Returns 0 otherwise
 */
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


/*
 * Splits a filename string in the format "NAME.EXT" into two separate parts:
 * an 8-character filename and a 3-character extension, both padded with spaces
 *
 * Args:
 *      const char* input: String representing the full filename
 *      char* filename: Output buffer for the 8-character filename
 *      char* extension: Output buffer for the 3-character extension
 *
 * The extracted filename and extention are stored in the passed in buffers
 */
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


/*
 * Converts an uppercase ASCII character to lowercase
 *
 * Args:
 *     char c: The character to convert
 *
 * Returns:
 *     char: The lowercase equivalent of the passed in character
 */
char toLower(char c) {
	if (c >= 'A' && c <= 'Z') {
		return c + ('a' - 'A');
	}
	return c;
}


/*
 * Initializes the FAT12 filesystem by reading and parsing the boot sector
 *
 * Args:
 *      unsigned int startSector: The starting sector of the FAT12 filesystem
 *      uint32_t* buffer: A buffer for temporary storage of the boot sector data
 *
 * Assumptions:
 *      - Passed in buffer is at least 512 bytes in size
 *      - "bootSector" and "extendedBootRecord" structures are gloabally defined
 */
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
	bootSector.hiddenSectorCount = buf[28] | (buf[29] << 8) | (buf[30] << 16) |
                                   (buf[31] << 24);
	bootSector.largeSectorCount = buf[32] | (buf[33] << 8) | (buf[34] << 16) |
                                  (buf[35] << 24);

	for (int i = 0; i < 54; i++) {
		bootSector.extendedBootRecord[i] = buf[36 + i];
	}

	extendedBootRecord.driverNumber = bootSector.extendedBootRecord[0];
	extendedBootRecord.reserved1 = bootSector.extendedBootRecord[1];
	extendedBootRecord.bootSignature = bootSector.extendedBootRecord[2];
	extendedBootRecord.volumeID = bootSector.extendedBootRecord[3] | 
		(bootSector.extendedBootRecord[4] << 8) |
		(bootSector.extendedBootRecord[5] << 16) |
		(bootSector.extendedBootRecord[6] << 24);

	for (int i = 0; i < 11; i++) {
		extendedBootRecord.volumeLabel[i] = bootSector.extendedBootRecord[7+i];
	}
		
	for (int i = 0; i < 8; i++) {
		extendedBootRecord.FATTypeLabel[i] = bootSector.extendedBootRecord[18+i];
	}
}


/*
 * Searches the root directory of the FAT12 filesystem for a file by name
 *
 * Args:
 *     const char* filename: The name of the file to search for (8.3 format)
 *     uint32_t* buffer: A buffer for reading sectors
 *     uint32_t* entryIndex: Pointer to an integer where the index of the
 *                           matching directory entry will be stored
 *
 * Assumptions:
 *     - Passed in buffer is large enough to hold one sector (512 bytes)
 *
 * Returns:
 *     - The sector number containing the matching directory entry
 *       if the file is found
 *     - 0 if the file is not found
 *
 */
int fat12_find(const char* filename, uint32_t* buffer, uint32_t* entryIndex) {

    uint32_t rootSectorStart = bootSector.reservedSectorCount +
                (bootSector.FATTableCount * bootSector.sectorsPerFATTable);

    uint32_t numRootSectors = (bootSector.rootEntryCount * 32) / 512;

    DirEntry tempEntry;

    /* Iterate through each sector */
    for (uint32_t i = rootSectorStart; i < rootSectorStart + numRootSectors;
        i++) {

        MMCreadblock(i, buffer);

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
                return i;
            }

        }

    }
    
    return 0;
}


/*
 * Retrieves the next cluster number in the FAT12 cluster chain
 *
 * Args:
 *     uint16_t cluster: The current cluster number whose next cluster
 *                       needs to be retrieved
 *
 * Returns:
 *     The next cluster number in the chain
 */
uint16_t fat12_get_next_cluster(uint16_t cluster) {
	uint16_t nextCluster;
	
	uint8_t FATTable[bootSector.bytesPerSector * 2];
	uint32_t fatStart = bootSector.reservedSectorCount;
	uint32_t fatOffset = cluster + (cluster / 2); /*1.5 bytes per entry */
	uint32_t fatSector = fatStart + (fatOffset / bootSector.bytesPerSector);
	uint32_t fatByteOffset = fatOffset % bootSector.bytesPerSector;

	MMCreadblock(fatSector, (uint32_t*)FATTable);

	if (cluster % 2 == 0) {
		nextCluster = (FATTable[fatByteOffset] |
           ((FATTable[fatByteOffset + 1]) << 8)) & 0xFFF;
	} else {
		nextCluster = ((FATTable[fatByteOffset] >> 4) |
           (FATTable[fatByteOffset + 1] << 4)) & 0xFFF;
	}


	return nextCluster;
}


/*
 * Sets the next cluster in the FAT12 table for a passed in cluster
 *
 * Args:
 *     uint16_t cluster: The cluster number whose FAT entry should be updated
 *     uint16_t nextCluster: The cluster number to set as the next in the chain
 */
void fat12_set_next_cluster(uint16_t cluster, uint16_t nextCluster) {
	
	uint8_t FATTable[bootSector.bytesPerSector * 2];
    uint32_t fatStart = bootSector.reservedSectorCount;
    uint32_t fatOffset = cluster + (cluster / 2);
    uint32_t fatSector = fatStart + (fatOffset / bootSector.bytesPerSector);
    uint32_t fatByteOffset = fatOffset % bootSector.bytesPerSector;

    MMCreadblock(fatSector, (uint32_t*)FATTable);

    if (cluster % 2 == 0) {
        /* Even cluster = lower 12 bits of the 16-bit entry */
        FATTable[fatByteOffset] = nextCluster & 0xFF;
        FATTable[fatByteOffset + 1] = (FATTable[fatByteOffset + 1] & 0xF0) |
		                              ((nextCluster >> 8) & 0x0F);
    } else {
        /* Odd cluster = upper 12 bits of the 16-bit entry */
        FATTable[fatByteOffset] = (FATTable[fatByteOffset] & 0x0F) |
		                          ((nextCluster << 4) & 0xF0);
        FATTable[fatByteOffset + 1] = (nextCluster >> 4) & 0xFF;
    }

    MMCwriteblock(fatSector, (uint32_t*)FATTable);
}


/*
 * Prints the cluster chain for a file starting from a passed in cluster
 *
 * Args:
 *     uint16_t firstCluster: The first cluster of the file whose
 *                            chain is to be printed.
 */
void fat12_print_cluster_chain(uint16_t firstCluster) {
    uint16_t cluster = firstCluster;
    uint16_t nextCluster;

    uart0_printf("Cluster chain for file (starting at cluster %d):\n", cluster);

    while (cluster < FAT12_EOF_MIN || cluster > FAT12_EOF_MAX) {
        uart0_printf("%d -> ", cluster);

        /* Get the next cluster in the chain */
        nextCluster = fat12_get_next_cluster(cluster);

        /* Check for invalid cluster or end of chain */
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


/*
 * Finds the first free cluster in the FAT12 filesystem
 *
 * Args:
 *     None
 *
 * Returns:
 *     uint16_t: The cluster number of the first free cluster
 *               or 0xFFFF if no free clusters are found
 */
uint16_t fat12_find_free_cluster() {
	
	uint16_t cluster = 2;	/* clusters 0 and 1 are reserved */
	uint8_t FATTable[bootSector.bytesPerSector * 2];
	uint32_t fatSector = bootSector.reservedSectorCount;
	uint32_t fatByteOffset = 0;

	/* Iterate through all sectors */
	for (uint32_t sector=0; sector < bootSector.sectorsPerFATTable; sector++) {
		
		MMCreadblock(fatSector + sector, (uint32_t*)FATTable);

		/* Iterate through the current FAT sector */
        for (fatByteOffset = 0; fatByteOffset < bootSector.bytesPerSector; fatByteOffset += 3) {
            
			/* Read two 12 bit FAT entries */
            uint16_t entry1 = (FATTable[fatByteOffset] |
			                  (FATTable[fatByteOffset + 1] << 8)) & 0xFFF;
            uint16_t entry2 = ((FATTable[fatByteOffset + 1] >> 4) |
			                  (FATTable[fatByteOffset + 2] << 4)) & 0xFFF;

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


/*
 * Reads the contents of a file from the FAT12 filesystem into a buffer
 *
 * Args:
 *     const char* filename: The name of the file to be read
 *     uint32_t* buffer: A buffer where the file data will be stored
 *     uint32_t* tempBuffer: A temporary buffer used for reading sectors
 *
 * Assumptions:
 *      - Passed in tempBuffer is at least 512 bytes in size
 *
 * Returns:
 *     uint32_t: The number of bytes read from the file
 *               or -1 if the file is not found
 */
uint32_t fat12_read_file(const char* filename, uint32_t* buffer, uint32_t* tempBuffer) {
	
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

	/* read until EOF marker */
	while (loopCluster < FAT12_EOF_MIN) {
		/* reads from first data sector available */
		sectorRead = rootSectorStart + numRootSectors +
		             ((loopCluster-2) * bootSector.sectorsPerCluster);
		
		/* need to divide bytesRead/4 to convert to pointer index */
		MMCreadblock(sectorRead, buffer + bytesRead / 4);

		/* updates bytes read */
		bytesRead += bootSector.bytesPerSector * bootSector.sectorsPerCluster;

		if (bytesRead > fileSize) {
			break;
		}	
				
		loopCluster = fat12_get_next_cluster(loopCluster);
	}
    
	return bytesRead;
}


/*
 * Creates a new directory entry for a file in the FAT12 root directory
 *
 * Args:
 *     const char* filename: The name of the file to create
 *     uint8_t attributes: File attributes to assign
 *     uint32_t* buffer: A buffer for reading and writing sectors
 *
 * Assumptions:
 *      - Passed in buffer is at least 512 bytes in size
 *
 * Returns:
 *     uint32_t: The first cluster allocated to the new file
 *               or -1 if no free entry is found.
 */
uint32_t fat12_create_dir_entry(const char* filename, uint8_t attributes, uint32_t* buffer) {
  
	uint32_t rootSectorStart = bootSector.reservedSectorCount +
                (bootSector.FATTableCount * bootSector.sectorsPerFATTable);

  	uint32_t numRootSectors = (bootSector.rootEntryCount * 32) / 512;

  	char* buf = (char*)buffer;

  	DirEntry *dirEntry;
  
  	for (uint32_t i = rootSectorStart; i < rootSectorStart + numRootSectors;
        i++) {
    	
		MMCreadblock(i, (uint32_t*)buf);
    
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
    
    			fat12_get_next_cluster(dirEntry->firstClusterLow);
    
    			MMCwriteblock(i, (uint32_t*)buf);
    			return dirEntry->firstClusterLow;
    		}
    	}
  	}

	return -1;

}


/*
 * Writes data to a file in the FAT12 file system. If the file does not exists,
 * a new file is created
 *
 * Args:
 *     const char* filename: The name of the file to write to
 *     char* data: Data to be written
 *     uint32_t size: Number of bytes to write
 *     uint32_t* tempBuffer: Buffer used for sector reads/writes
 *
 * Assumptions:
 *      - Passed in tempBuffer is at least 512 bytes in size
 *
 * Returns:
 *     uint32_t: The number of bytes written
 *               or -1 if the file was not found and a new entry was created
 */
uint32_t fat12_write_file(const char* filename, char* data, uint32_t size, uint32_t* tempBuffer) {

	DirEntry *fileEntry;
	uint16_t cluster, prevCluster, k; 
	uint32_t bytesWritten = 0;
	uint32_t sector, entryIndex;
	
	/* Look for the file */
	uint32_t dirSector = fat12_find(filename, tempBuffer, &entryIndex);

	uint32_t firstDataSector = bootSector.reservedSectorCount +
		(bootSector.FATTableCount * bootSector.sectorsPerFATTable) +
		((bootSector.rootEntryCount * 32) / bootSector.bytesPerSector);
	uint32_t rootSectorStart = bootSector.reservedSectorCount +
                (bootSector.FATTableCount * bootSector.sectorsPerFATTable);

	char temp[32];

	/* File not found, create a new entry */
	if (dirSector == 0) {
    	uart0_printf("Not found - Creating new \n");
    	fat12_create_dir_entry(filename, 0x20, tempBuffer);
    	return -1;
	}

	/* Read directory sector and get file entry */
	MMCreadblock(dirSector, tempBuffer);
	fileEntry = &((DirEntry*)tempBuffer)[entryIndex];

	cluster = fileEntry->firstClusterLow;
	k = cluster;

	/* Write to file */
	while (bytesWritten < size) {

		if (cluster >= FAT12_EOF_MIN && cluster <= FAT12_EOF_MAX) {
			cluster = fat12_find_free_cluster();

			fat12_set_next_cluster(prevCluster, cluster);
			fat12_set_next_cluster(cluster, FAT12_EOF_MAX);
		}

		sector = firstDataSector + ((cluster - 2) * bootSector.sectorsPerCluster);

		MMCwriteblock(sector, (uint32_t*)(data + bytesWritten));
	
		bytesWritten += bootSector.bytesPerSector * bootSector.sectorsPerCluster;
		prevCluster = cluster;
		cluster = fat12_get_next_cluster(cluster);

	}

	/* Update file size in dir entry */
	fileEntry->fileSize = size;
	MMCwriteblock(dirSector, tempBuffer);

	return bytesWritten;
}


/*
 * Lists the contents of the root directory in a FAT12 filesystem
 *
 * Args:
 *     uint32_t* buffer: A temporary buffer used to read sectors
 *     uint32_t allFlag: If non-zero, includes all entries
 *                       otherwise, filters out hidden/system files
 * Assumptions:
 *      - Passed in buffer is at least 512 bytes in size
 */
void list_dir(uint32_t* buffer, uint32_t allFlag) {
	uint32_t rootSectorStart = bootSector.reservedSectorCount +
                (bootSector.FATTableCount * bootSector.sectorsPerFATTable);
	uint32_t numRootSectors = (bootSector.rootEntryCount * 32) / 512;
	DirEntry dirEntry;
	char extTruncated[4];
	int extLength = 0;

	for (uint32_t i = rootSectorStart; i < rootSectorStart + numRootSectors; i++) {
		MMCreadblock(i, buffer);

		char *buf = (char*)buffer;
		for (int j = 0; j < 16; j++) {
			dirEntry = *((DirEntry*) &buf[j*32]);
			extLength = 0;
			
			for (int i = 0; i < sizeof(extTruncated); i++) {
			    extTruncated[i] = '\0';
			}

			for (int k = 0; k < 8; k++) {
				if (dirEntry.name[k] == ' ' || dirEntry.name[k] == '\0') {
					dirEntry.name[k] = '\0';
					break;
				}
				dirEntry.name[k] = toLower(dirEntry.name[k]);
			}
			
			/* Check for directory end */
			if (dirEntry.name[0] == 0x00) {
				break; 
			}

			if (!allFlag && ((dirEntry.attrib & 0x04) || (dirEntry.attrib & 0x02))) {
				continue;
			}

			/* Check for valid entry */
			if (dirEntry.name[0] == 0xE5) {
				continue;
			}
		
			for (int l = 0; l < 3; l++) {
				if (dirEntry.ext[l] != ' ' && dirEntry.ext[l] != '\0') {
					extTruncated[extLength++] = toLower(dirEntry.ext[l]);
				}	
			}

			if (dirEntry.attrib == 0x10) {
                uart0_printf("[DIR] %s\n", dirEntry.name);
            } else {
                if (extLength > 0) {
                    uart0_printf("%s.%s\n", dirEntry.name, extTruncated);
                } else {
                    uart0_printf("%s\n", dirEntry.name);
                }
            }
		}
	}
}

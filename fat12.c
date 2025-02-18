#include "mmc.h"
#include "reg.h"
#include <stdint.h>
#include "uart.h"
#include "fat12.h"

fat_bs_t bootSector;
fat12_ebs_t extendedBootRecord;

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
}

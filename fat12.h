#ifndef FAT12_H
#define FAT12_H

typedef struct FAT_BS
{
	unsigned char bootJmp[3]; /* jump to executable code */
	unsigned char oemName[8]; /* Original Equipment Manufacturer, states formatting standards */
	unsigned int bytesPerSector;
	unsigned char sectorsPerCluster;
	unsigned int reservedSectorCount;
	unsigned char FATTableCount;
	unsigned int rootEntryCount;
	unsigned int totalSectorCount;
	unsigned char mediaType;
	unsigned int sectorsPerFATTable;
	unsigned int sectorsPerTrack;
	unsigned int headCountOnMedia;
	unsigned int hiddenSectorCount;
	unsigned int largeSectorCount;
}

#endif

#ifndef FAT12_H
#define FAT12_H

typedef struct fat_bs {
	unsigned char bootJmp[3]; /* jump over disk format info to actual executable code */
	unsigned char oemName[8]; /* original equipment manufacturer, states formatting standards */
	unsigned int bytesPerSector; /* all numbers are little-endian format */
	unsigned char sectorsPerCluster;  
	unsigned int reservedSectorCount; /*boot record sectors included in this count */
	unsigned char FATTableCount; /*Number of FAT's on storage media, generally 2 */
	unsigned int rootEntryCount; /*root directory entry count, set so root directory occupies entire sectors */
	unsigned int totalSectorCount; /* total sectors in logical volume, if 0 that means > 65535 sectors in volume and count stored in 0x20 (largeSectorCount) */
	unsigned char mediaType; 
	unsigned int sectorsPerFATTable; 
	unsigned int sectorsPerTrack;
	unsigned int headCountOnMedia; /* number of heads or sides on storage media */
	unsigned int hiddenSectorCount; /* the LBA of the beginning of the partition */
	unsigned int largeSectorCount; /* used for totalSectorCount overflow */
	unsigned char extendedBootRecord[54]; /* flexible for FAT12/FAT16/FAT32 respectively */
} fat_bs_t;

typedef struct fat12_ebs {
	unsigned char driverNumber; 
	unsigned char reserved1;
	unsigned char bootSignature;
	unsigned int volumeID; /* used to track volumes between computeres */
	unsigned char volumeLabel[11];
	unsigned char FATTypeLabel[8]; /* string representation of FAT file system type */
} fat12_ebs_t;

typedef struct {
    char name[8];   /* file name */
    char ext[3];    /* file extension */
    uint8_t attrib; /* file attributes */
    uint16_t reserved; /* reserved (unused) */
    uint16_t createTime;
    uint16_t createDate;
    uint16_t lastAccessDate;
    uint16_t firstClusterHigh; /* High 16-bits of the first cluster number */
    uint16_t modifyTime;
    uint16_t modifyDate;
    uint16_t firstClusterLow;  /* Low 16-bits of the first cluster number */
    uint32_t fileSize;
} __attribute__((packed)) DirEntry;


void fat12_init(unsigned int startSector, volatile uint32_t* buffer); 
void fat12_find(const char* filename, volatile uint32_t* buffer);//,
    //uint16_t *start_cluster, uint32_t *file_size); 
#endif

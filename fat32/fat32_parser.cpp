#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// FAT32 constants
#define FAT_ENTRY_SIZE 4
#define EOC_MARKER 0x0FFFFFF8  // End of Chain marker for FAT32

// Function to read FAT and print cluster chain
int read_fat(const char* filename, int starting_cluster) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Cannot open file %s\n", filename);
        return 1;
    }

    unsigned char buf[512];  // Buffer size for the BPB (Boot Sector) read

    // Read the BPB (BIOS Parameter Block) to get important offsets
    if (fread(buf, 1, 512, fp) != 512) {
        printf("Failed to read BPB\n");
        fclose(fp);
        return 1;
    }

    // BPB values
    uint16_t bytes_per_sector = *((uint16_t*)&buf[11]);  // Bytes per Sector
    uint16_t reserved_sectors = *((uint16_t*)&buf[14]);  // Reserved Sector Count
    uint8_t num_fats = buf[16];  // Number of FATs
    uint32_t sectors_per_fat = *((uint32_t*)&buf[36]);  // Sectors per FAT

    // Calculate FAT starting position
    uint32_t fat_offset = reserved_sectors * bytes_per_sector;

    // Move to the start of the FAT table
    fseek(fp, fat_offset, SEEK_SET);

    // Calculate the position of the cluster in the FAT
    uint32_t fat_entry_offset = starting_cluster * FAT_ENTRY_SIZE;
    uint32_t cluster_number = starting_cluster;

    // printf("Cluster chain: ");
    
    while (cluster_number < 0x0FFFFFF8) {
        // Seek to the position of the FAT entry
        fseek(fp, fat_offset + fat_entry_offset, SEEK_SET);

        // Read the FAT entry
        uint32_t next_cluster;
        fread(&next_cluster, FAT_ENTRY_SIZE, 1, fp);

        // Mask the higher 4 bits as they are reserved
        next_cluster &= 0x0FFFFFFF;

        printf("%d ", cluster_number);

        if (next_cluster >= EOC_MARKER) {
            break;
        }

        // Move to the next cluster
        cluster_number = next_cluster;
        fat_entry_offset = cluster_number * FAT_ENTRY_SIZE;
    }

    printf("\n");

    fclose(fp);
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Wrong number of arguments\n");
        printf("Usage: %s <disk image file> <starting cluster number>\n", argv[0]);
        return 1;
    }

    char *endptr;
    int starting_cluster = strtol(argv[2], &endptr, 10);
    if (*endptr != '\0') {
        printf("Invalid starting cluster number\n");
        return 1;
    }
    // printf("Starting cluster: %d\n", starting_cluster);

    return read_fat(argv[1], starting_cluster);
}

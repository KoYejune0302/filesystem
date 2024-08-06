#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Define GUID Partition Entry
typedef struct {
    unsigned char type_guid[16];
    unsigned char unique_guid[16];
    unsigned long long first_lba;
    unsigned long long last_lba;
    unsigned long long attributes;
    unsigned char name[72];
} __attribute__((packed)) gpt_entry_t;

// Function to convert GUID into the required format
void convert_type_guid(unsigned char* guid, unsigned char* converted_guid) {
    // Rearrange GUID bytes
    converted_guid[0] = guid[3];
    converted_guid[1] = guid[2];
    converted_guid[2] = guid[1];
    converted_guid[3] = guid[0];
    converted_guid[4] = guid[5];
    converted_guid[5] = guid[4];
    converted_guid[6] = guid[7];
    converted_guid[7] = guid[6];
    for (int i = 8; i < 16; ++i) {
        converted_guid[i] = guid[i];
    }
}

// Function to determine the filesystem type by reading the actual partition
const char* get_filesystem_type(FILE* fp, unsigned long long first_lba) {
    unsigned char buf[512];
    // Seek to the first sector of the partition
    fseek(fp, first_lba * 512, SEEK_SET);
    fread(buf, 1, 512, fp);

    // Check for FAT32 signature (0x55, 0xAA at the end of sector and "FAT" at byte 54)
    if (buf[510] == 0x55 && buf[511] == 0xAA && buf[54] == 'F' && buf[55] == 'A' && buf[56] == 'T') {
        return "FAT32";
    }

    // Check for NTFS signature ("NTFS" at byte 3)
    if (buf[3] == 'N' && buf[4] == 'T' && buf[5] == 'F' && buf[6] == 'S') {
        return "NTFS";
    }

    return "UNKNOWN";
}

int read_gpt(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Cannot open file %s\n", filename);
        return 1;
    }

    unsigned char buf[512];

    // Read the primary GPT header (LBA 1)
    fseek(fp, 512, SEEK_SET); // Move to the second sector (LBA 1)
    fread(buf, 1, 512, fp);

    // Check the GPT signature
    if (memcmp(buf, "EFI PART", 8) != 0) {
        printf("Not a valid GPT partition table.\n");
        fclose(fp);
        return 1;
    }

    // Get the number of partition entries and the size of each entry
    unsigned int num_entries = *((unsigned int*)(buf + 80));
    unsigned int entry_size = *((unsigned int*)(buf + 84));
    unsigned long long entries_lba = *((unsigned long long*)(buf + 72));
    // printf("Number of partition entries: %u\n", num_entries);
    // printf("Size of each entry: %u bytes\n", entry_size);

    // Allocate buffer for partition entries
    gpt_entry_t* entries = (gpt_entry_t*) malloc(num_entries * entry_size);
    if (entries == NULL) {
        printf("Memory allocation error.\n");
        fclose(fp);
        return 1;
    }

    // Read the partition entries
    fseek(fp, entries_lba * 512, SEEK_SET); // Move to the first partition entry
    fread(entries, entry_size, num_entries, fp);

    // Print partition entries
    for (unsigned int i = 0; i < num_entries; ++i) {
        gpt_entry_t* entry = &entries[i];

        // Check if the partition entry is unused
        int is_empty = 1;
        for (int j = 0; j < 16; ++j) {
            if (entry->type_guid[j] != 0) {
                is_empty = 0;
                break;
            }
        }
        if (is_empty) {
            continue; // Skip unused entry
        }

        // Convert type GUID
        unsigned char converted_guid[16];
        convert_type_guid(entry->type_guid, converted_guid);

        // Print converted GUID
        for (int j = 0; j < 16; ++j) {
            printf("%02X", converted_guid[j]);
        }
        printf(" ");

        // Determine and print filesystem type
        const char* fs_type = get_filesystem_type(fp, entry->first_lba);
        // printf("%s ", fs_type);

        // Print Start Sector and Size in sectors
        printf("%llu %llu\n", entry->first_lba, entry->last_lba - entry->first_lba + 1);
    }

    free(entries);
    fclose(fp);
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Wrong number of arguments\n");
        printf("Usage: %s <disk image file>\n", argv[0]);
        return 1;
    }

    return read_gpt(argv[1]);
}

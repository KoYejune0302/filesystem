#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

struct ntfs_boot_sector {
    uint8_t jump[3];
    uint8_t oem_id[8];
    uint8_t bytes_per_sector[2];
    uint8_t sectors_per_cluster;
    uint8_t reserved_sectors[2];
    uint8_t unused1[5];
    uint8_t media_descriptor;
    uint8_t unused2[18];
    uint8_t total_sectors[8];
    uint8_t mft_cluster_number[8];
    uint8_t mft_mirror_cluster_number[8];
    uint8_t clusters_per_mft_record;
    uint8_t unused3[3];
    uint8_t clusters_per_index_buffer;
    uint8_t volume_serial_number[8];
    uint8_t bootstrap_code[430];
    uint8_t end_of_sector_marker[2];
} __attribute__((packed));

struct mft_entry_header {
    uint8_t signature[4];
    uint16_t fixup_offset;
    uint16_t fixup_entries;
    uint8_t lsn[8];
    uint16_t sequence_number;
    uint16_t hard_link_count;
    uint16_t attribute_offset;
    uint16_t flags;
    uint32_t used_size;
    uint32_t allocated_size;
    uint8_t base_mft_record[8];
    uint16_t next_attribute_id;
} __attribute__((packed));

struct attr_header {
    uint32_t type;
    uint32_t length;
    uint8_t non_resident_flag;
    uint8_t name_length;
    uint16_t name_offset;
    uint16_t flags;
    uint16_t instance;
} __attribute__((packed));

struct resident_addr {
    uint32_t length;
    uint16_t attr_offset;
    uint8_t indexed_flag;
    uint8_t padding;
} __attribute__((packed));

struct non_resident_addr {
    uint8_t start_vcn[8];
    uint8_t end_vcn[8];
    uint16_t runlist_offset;
    uint8_t compression_unit_size[2];
    uint8_t padding[4];
    uint8_t allocated_size[8];
    uint8_t data_size[8];
    uint8_t initialized_size[8];
} __attribute__((packed));


// Function to read ntfs MFT0 and print the runlist
// print the run length(in decimal) and run offset(in size of LCN in decimal) of each run in runlist 
int read_ntfs_runlist(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Cannot open file %s\n", filename);
        return 1;
    }

    struct ntfs_boot_sector boot_sector;
    fread(&boot_sector, sizeof(boot_sector), 1, fp);

    // printf("Bytes per sector: %d\n", *((uint16_t*)boot_sector.bytes_per_sector));
    // printf("Sectors per cluster: %d\n", boot_sector.sectors_per_cluster);

    // printf("\nMFT cluster number: %lx\n", *((uint64_t*)boot_sector.mft_cluster_number));
    // printf("MFT actual address: %lx\n", *((uint64_t*)boot_sector.mft_cluster_number) * boot_sector.sectors_per_cluster * *((uint16_t*)boot_sector.bytes_per_sector));
    uint64_t mft0_offset = *((uint64_t*)boot_sector.mft_cluster_number) * boot_sector.sectors_per_cluster * *((uint16_t*)boot_sector.bytes_per_sector);
    // printf("MFT0 address: %lx\n", mft0_offset);

    struct mft_entry_header mft_entry;
    fseek(fp, mft0_offset, SEEK_SET);
    fread(&mft_entry, sizeof(mft_entry), 1, fp);

    // printf("\nMFT entry signature: %02x %02x %02x %02x\n", mft_entry.signature[0], mft_entry.signature[1], mft_entry.signature[2], mft_entry.signature[3]);
    // printf("first attribute offset: %x\n", mft_entry.attribute_offset);

    while ( true ) {
        struct attr_header attr_header;
        fseek(fp, mft0_offset + mft_entry.attribute_offset, SEEK_SET);
        fread(&attr_header, sizeof(attr_header), 1, fp);

        if (attr_header.type == 0xffffffff) {
            break;
        }

        // printf("\nAttribute type: %x\n", attr_header.type);
        // printf("Attribute length: %x\n", attr_header.length);

        if (attr_header.non_resident_flag == 0) {
            // struct resident_addr resident_addr;
            // fread(&resident_addr, sizeof(resident_addr), 1, fp);
            // printf("Resident attribute length: %x\n", resident_addr.length);
            // printf("Resident attribute offset: %x\n", resident_addr.attr_offset);
        } else {
            struct non_resident_addr non_resident_addr;
            fread(&non_resident_addr, sizeof(non_resident_addr), 1, fp);
            // printf("Non-resident start VCN: %lx\n", *((uint64_t*)non_resident_addr.start_vcn));
            // printf("Non-resident end VCN: %lx\n", *((uint64_t*)non_resident_addr.end_vcn));
            // printf("Non-resident runlist offset: %x\n", non_resident_addr.runlist_offset);

            fseek(fp, mft0_offset + mft_entry.attribute_offset + non_resident_addr.runlist_offset, SEEK_SET);
            uint8_t run_info;
            uint8_t run_length;
            uint8_t run_offset;
            uint8_t run_length_value[4];
            uint8_t run_offset_value[4];

            int offset = 0;
            int lcn_offset = 0;
            while (true) {
                fread(&run_info, sizeof(run_info), 1, fp);
                // printf("Run info: %x\n", run_info);
                if (run_info == 0) {
                    break;
                }

                run_length = run_info & 0x0f;
                run_offset = run_info >> 4;
                // printf("Run length: %x Run offset: %x\n", run_length, run_offset);

                if (run_length == 0) {
                    break;
                }

                fseek(fp, mft0_offset + mft_entry.attribute_offset + non_resident_addr.runlist_offset + offset + 1, SEEK_SET);
                fread(run_length_value, run_length, 1, fp);

                fseek(fp, mft0_offset + mft_entry.attribute_offset + non_resident_addr.runlist_offset + offset + 1 + run_length, SEEK_SET);
                fread(run_offset_value, run_offset, 1, fp);

                // printf("Run length value: ");
                // for (int i = 0; i < run_length; i++) {
                //     printf("%x ", run_length_value[i]);
                // }
                // printf("\n");

                // printf("Run offset value: ");
                // for (int i = 0; i < run_offset; i++) {
                //     printf("%02x ", run_offset_value[i]);
                // }
                // printf("\n");

                uint32_t run_length_dec = 0;
                for (int i = 0; i < run_length; i++) {
                    run_length_dec |= run_length_value[i] << (i * 8);
                }

                int32_t run_offset_dec = 0;
                for (int i = 0; i < run_offset; i++) {
                    run_offset_dec |= run_offset_value[i] << (i * 8);
                }
                // printf("offset real value hex: %02x\n", run_offset_dec);
                // check if the run_offset_desc msb is 1
                if (run_offset_dec & (1 << (run_offset * 8 - 1))) {
                    run_offset_dec = run_offset_dec - (1 << (run_offset * 8));
                }
                // printf("final offset real value: %d\n", run_offset_dec);

                // printf("Run length: %d Run offset: %d\n", run_length_dec, lcn_offset + run_offset_dec);
                printf("%d %d\n", lcn_offset + run_offset_dec, run_length_dec);

                offset += 1 + run_length + run_offset;
                lcn_offset += run_offset_dec;
                fseek(fp, mft0_offset + mft_entry.attribute_offset + non_resident_addr.runlist_offset + offset, SEEK_SET);
                // break;
            }
        }

        mft_entry.attribute_offset += attr_header.length;
    }

    fclose(fp);
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Wrong number of arguments\n");
        printf("Usage: %s <disk image file>\n", argv[0]);
        return 1;
    }

    return read_ntfs_runlist(argv[1]);
}
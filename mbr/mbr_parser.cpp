// input file: disk image file
// ex: mbr_parser ./mbr_128.dd

// print all the partition information in the MBR section of the disk image file
// output format: {filesystem type(ex: FAT32, NTFS, ... )} {start sector} {size}

#include <stdio.h>

int read_mbr(const char* filename){
    FILE* fp = fopen(filename, "rb");
    if(fp == NULL){
        printf("Cannot open file %s\n", filename);
        return 1;
    }

    unsigned char buf[512];
    fread(buf, 1, 512, fp);

    // print mbr in hex
    // for(int i = 0; i < 512; i++){
    //     printf("%02X ", buf[i]);
    //     if(i % 16 == 15){
    //         printf("\n");
    //     }
    // }

    // print pte in hex
    // for(int i = 0; i < 4; i++){
    //     for(int j = 0; j < 16; j++){
    //         printf("%02X ", buf[0x1BE + i * 16 + j]);
    //         if(j % 16 == 15){
    //             printf("\n");
    //         }
    //     }
    // }

    for(int i = 0x1BE; i < 0x1FE; i += 16){
        if(buf[i + 4] == 0x05 || buf[i + 4] == 0x0F){
            // printf("EBR found\n");
            return 0;
        }

        // check which filesystem type
        char fs_type[10];
        if(buf[i + 4] == 0x01){
            sprintf(fs_type, "FAT12");
        }else if(buf[i + 4] == 0x04){
            sprintf(fs_type, "FAT16");
        }else if(buf[i + 4] == 0x06){
            sprintf(fs_type, "FAT16B");
        }else if(buf[i + 4] == 0x07){
            sprintf(fs_type, "NTFS");
        }else if(buf[i + 4] == 0x0B){
            sprintf(fs_type, "FAT32");
        }else if(buf[i + 4] == 0x0C){
            sprintf(fs_type, "FAT32X");
        }else if(buf[i + 4] == 0x0E){
            sprintf(fs_type, "FAT16X");
        }else{
            sprintf(fs_type, "Unknown");
        }

        // print the information
        unsigned int start_sector = buf[i + 8] + (buf[i + 9] << 8) + (buf[i + 10] << 16) + (buf[i + 11] << 24);
        unsigned int size = buf[i + 12] + (buf[i + 13] << 8) + (buf[i + 14] << 16) + (buf[i + 15] << 24);
        printf("%s %u %u\n", fs_type, start_sector, size);
    }

    fclose(fp);
    return 0;

}

int main(int argc, char** argv){
    if(argc != 2){
        printf("Wrong number of arguments\n");
        printf("Usage: %s <disk image file>\n", argv[0]);
        return 1;
    }

    read_mbr(argv[1]);
    return 0;
}


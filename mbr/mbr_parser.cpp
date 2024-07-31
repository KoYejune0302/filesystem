// input file: disk image file
// ex: mbr_parser ./mbr_128.dd
#include <stdio.h>


int ebr_traversal(FILE* fp, unsigned int base_location, unsigned int offset){

    unsigned char buf[512];
    fseek(fp, (base_location + offset) * 512, SEEK_SET);
    fread(buf, 1, 512, fp);

    int i = 0x1be;

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
    printf("%s %u %u\n", fs_type, base_location + offset + start_sector, size);

    i += 16;
    if(buf[i + 4] == 0x05 || buf[i + 4] == 0x0F){
        // NEXT EBR
        ebr_traversal(fp, base_location, buf[i + 8] + (buf[i + 9] << 8) + (buf[i + 10] << 16) + (buf[i + 11] << 24));
    }


    return 0;
}

int read_mbr(const char* filename){
    FILE* fp = fopen(filename, "rb");
    if(fp == NULL){
        printf("Cannot open file %s\n", filename);
        return 1;
    }

    unsigned char buf[512];
    fread(buf, 1, 512, fp);

    for(int i = 0x1BE; i < 0x1FE; i += 16){
        // if there is no partition, return
        if(buf[i] == 0x00 && buf[i + 1] == 0x00 && buf[i + 2] == 0x00 && buf[i + 3] == 0x00 && buf[i + 3] == 0x00){
            return 0;
        }

        if(buf[i + 4] == 0x05 || buf[i + 4] == 0x0F){
            // EBR
            ebr_traversal(fp, buf[i + 8] + (buf[i + 9] << 8) + (buf[i + 10] << 16) + (buf[i + 11] << 24), 0);
            break;
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
            return 0;
        }

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


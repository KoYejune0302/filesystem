#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


// Function to read ntfs MFT0 and print the runlist
// print the run length(in decimal) and run offset(in size of LCN in decimal) of each run in runlist 
int read_ntfs_runlist(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Cannot open file %s\n", filename);
        return 1;
    }

    // TODO


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
#include "fs.h"
#include "lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void usage()
{
    printf("usage: mkfs <input_image>\n");
    exit(-1);
}

int main(int argc, char **argv)
{
    if (argc < 2)
        usage();

    FILE *file = fopen(argv[1], "r+");
    if (!file)
    {
        printf("mkfs: %s: %s\n", argv[1], strerror(errno));
        return -1;
    }
    
    fseek(file, 0, SEEK_END);
    size_t sect = ftell(file) / 512;
    fseek(file, 0, SEEK_SET);

    uint32_t bm = sect % 8 ? sect / 8 + 1 : sect / 8;
    if (bm % 512) bm += 512 - (bm % 512);

    struct super super = {
        .bootsig = 0xaa55,
        .root = 1 + bm / 512
    };

    fwrite(&super, sizeof(struct super), 1, file);

    size_t pos = 0;

    fputbit(1, file, &pos); // Superblock
    
    // Bitmap sectors
    for (size_t i = 0; i < bm / 512; i++)
        fputbit(1, file, &pos);

    fputbit(1, file, &pos); // Root directory
    
    struct node root = {
        .size = 0,
    };

    root.sects[0] = getsect(file);
   
    fseek(file, super.root * 512, SEEK_SET);
    fwrite(&root, sizeof(struct node), 1, file);

    fclose(file);
    return 0;
}

#include "fs.h"
#include "lib.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void usage()
{
    printf("usage: copy <fsimage> <file> <path>\n");
    exit(-1);
}

FILE *xfopen(const char *path, const char *acc)
{
    FILE *f = fopen(path, acc);
    if (!f)
    {
        printf("copy: %s: %s\n", path, strerror(errno));
        exit(-1);
    }
    return f;
}

uint32_t getsect(FILE *file)
{
    fseek(file, 1024, SEEK_SET);

    size_t i = 0, pos = 0;
    while (getbit(file, &pos)) i++;

    fbackbit(file, &pos);
    fputbit(1, file, &pos);

    return i;
}

int main(int argc, char **argv)
{
    if (argc < 4)
        usage();

    FILE *img = xfopen(argv[1], "r+");
    FILE *file = xfopen(argv[2], "r");

    fseek(file, 0, SEEK_END);
    size_t len = ftell(file);
    size_t lensect = len % 512 ? len - (len % 512) + 512 : len;
    fseek(file, 0, SEEK_SET);

    struct dirent ent = {
        .size = lensect
    };

    strncpy(ent.name, argv[3], 15);
    ent.name[15] = 0;

    struct super super;
    fseek(img, 512, SEEK_SET);
    fread(&super, sizeof(struct super), 1, img);

    uint32_t off = super.rootsize;
    uint32_t root = super.root;

    super.rootsize += sizeof(struct dirent);

    fseek(img, 512, SEEK_SET);
    fwrite(&super, sizeof(struct super), 1, img);

    uint8_t buf[512];
    for (size_t i = 0; i < lensect; i += 512)
    {
        uint32_t sect = getsect(img);
        fseek(img, sect * 512, SEEK_SET);

        size_t s = 512;
        if (i == lensect - 512)
            s = len % 512;

        fread(buf, s, 1, file);
        fwrite(buf, s, 1, img);

        ent.sects[i / 512] = sect;
    }

    fseek(img, root * 512 + off, SEEK_SET);
    fwrite(&ent, sizeof(struct dirent), 1, img);

    fclose(img);
    fclose(file);
    return 0;
}

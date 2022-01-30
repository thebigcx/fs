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

    size_t esize = strlen(argv[3]) + 1 + sizeof(struct dirent);
    struct dirent *ent = malloc(esize);
    strcpy(ent->name, argv[3]);
    
    ent->node = getsect(img);

    struct super super;
    fseek(img, 0, SEEK_SET);
    fread(&super, sizeof(struct super), 1, img);

    struct node pnode;
    fseek(img, super.root * 512, SEEK_SET);
    fread(&pnode, sizeof(struct node), 1, img);

    struct node node = {
        .size = len,
    };

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

        node.sects[i / 512] = sect;
    }

    fseek(img, ent->node * 512, SEEK_SET);
    fwrite(&node, sizeof(struct node), 1, img);

    fseek(img, pnode.sects[0] * 512, SEEK_SET);
    fseek(img, pnode.size, SEEK_CUR);

    fwrite(ent, esize, 1, img);

    pnode.size += esize;

    fseek(img, super.root * 512, SEEK_SET);
    fwrite(&pnode, sizeof(struct node), 1, img);

    fclose(img);
    fclose(file);
    return 0;
}

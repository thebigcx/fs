#pragma once

#include <stdint.h>

#define DT_DIR (1 << 0)

struct dirent
{   
    uint32_t node;
    int8_t name[];
};

struct __attribute__((packed)) super
{
    uint8_t boot[506];

    uint32_t root;
    uint16_t bootsig;
};

struct node
{
    uint64_t size;
    uint8_t flags;
    uint32_t sects[8];
    uint32_t sisect; // Single indirect sectors
    uint32_t disect; // Double indirect sectors
    uint32_t tisect; // Triple indirect sectors

    uint8_t pad[7];
};

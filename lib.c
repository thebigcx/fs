#include "lib.h"

void fputbit(uint8_t bit, FILE *file, size_t *pos)
{
    uint8_t byte;
    fread(&byte, 1, 1, file);

    if (bit)
        byte |= (1 << (7 - (*pos % 8)));
    else
        byte &= ~(1 << (7 - (*pos % 8)));

    fseek(file, -1, SEEK_CUR);
    fwrite(&byte, 1, 1, file);
    
    if (++(*pos) % 8)
        fseek(file, -1, SEEK_CUR);
}

uint8_t getbit(FILE *file, size_t *pos)
{
    uint8_t byte;
    fread(&byte, 1, 1, file);

    uint8_t bit = byte & (1 << (7 - (*pos % 8)));

    if (++(*pos) % 8)
        fseek(file, -1, SEEK_CUR);

    return bit;
}

void fbackbit(FILE *file, size_t *pos)
{
    if (!((*pos)-- % 8))
        fseek(file, -1, SEEK_CUR);
}

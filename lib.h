#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

void fputbit(uint8_t bit, FILE *file, size_t *pos);
void fbackbit(FILE *file, size_t *pos);
uint8_t getbit(FILE *file, size_t *pos);

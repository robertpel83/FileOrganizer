#ifndef IMGHASH_C_H
#define IMGHASH_C_H

#include "simplehasher.h"

unsigned long long int imghash_buffer(char *buffer, unsigned int bufferSize);
unsigned long long int imghash_file(char *fileName);
const char * imghash_file_hex(char* fileName);

#endif

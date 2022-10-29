#include "imghash_c.h"

unsigned long long imghash_buffer(char *buffer, unsigned int size)
{
    try {
        imghash::Source source(buffer, size);
        imghash::Hasher *hasher = new imghash::SimpleHasher();
        imghash::Hash hash = hasher->hash(source);
        delete hasher;
        return hash.toLongLong();
    } catch (Magick::Error e) {
        printf("oops, cannot calculate hash: %s\n", e.what());
        return 0;
    }
}


unsigned long long imghash_file(char *fileName)
{
    try {
        imghash::Source source;
        source.loadFile(fileName);
        imghash::Hasher *hasher = new imghash::SimpleHasher();
        imghash::Hash hash = hasher->hash(source);
        delete hasher;
        return hash.toLongLong();
    } catch (Magick::Error e) {
        printf("oops, cannot calculate hash: %s\n", e.what());
        return 0;
    }
}


const char *imghash_file_hex(char *fileName)
{
    try {
        imghash::Source source;
        source.loadFile(fileName);
        imghash::Hasher *hasher = new imghash::SimpleHasher();
        imghash::Hash hash = hasher->hash(source);
        delete hasher;
        return hash.toHex().c_str();
    } catch (Magick::Error e) {
        printf("oops, cannot calculate hash: %s\n", e.what());
        return 0;
    }
}

#ifndef HASHER_H
#define HASHER_H

#include "source.h"
#include "hash.h"

namespace imghash {

class Hasher {
public:
    Hash hash(char* buffer, unsigned int length);
    Hash hash(std::string& data);
    Hash hash(Source image);
protected:
    virtual Hash hashSource(Source image) = 0;
};

}

#endif

#ifndef SIMPLEHASHER_H
#define SIMPLEHASHER_H

#include "hasher.h"

namespace imghash {

class SimpleHasher: public Hasher {
public:
    SimpleHasher(unsigned int size = 8);
protected:
    virtual Hash hashSource(Source image);
private:
    unsigned int size;
    float getAverage(const Magick::PixelPacket *pixels) const;
    int getMedian(const Magick::PixelPacket *pixels) const;
};

}

#endif

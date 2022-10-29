/*
 * Copyright 2012 Sociohub LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: Roman Grebennikov
 */

#include "hash.h"

imghash::Hash::Hash()
{

}

std::string imghash::Hash::toString()
{
    return "";
}

std::string imghash::Hash::toHex()
{
    std::stringstream stream;
    for (unsigned int i=0; i<data.size(); ++i) {
        stream << std::hex << std::setfill('0') << std::setw(8) << data[i];
    }
    return stream.str();
}

unsigned long long imghash::Hash::toLongLong()
{
    if (data.empty())
        return 0;
    else if (data.size() == 1)
        return data[0];
    else if (data.size() == 2){
        unsigned long long result = (unsigned long long)data[0] << 8*sizeof(unsigned int) | data[1];
        return result;
    } else {
        assert(data.size() <= 2);
    }
}

void imghash::Hash::setBit(unsigned int index, bool value)
{
    unsigned int blockNumber = (index - (index % (sizeof(unsigned int)*8))) / (sizeof(unsigned int)*8);
    unsigned int blockOffset = index - (blockNumber * sizeof(unsigned int)*8);
    if (blockNumber >= data.size())
        data.resize(blockNumber+1,0);
    if (value)
        data[blockNumber] |= (1 << blockOffset);
}

bool imghash::Hash::getBit(unsigned int index) const
{
    unsigned int blockNumber = (index - (index % (sizeof(unsigned int)*8))) / (sizeof(unsigned int)*8);
    unsigned int blockOffset = index - (blockNumber * sizeof(unsigned int)*8);
    return data[blockNumber] & (1 << blockOffset);
}

float imghash::Hash::getRMSE(const imghash::Hash &other)
{
    return sqrt(getMSE(other));
}

float imghash::Hash::getMSE(const imghash::Hash &other)
{
    int sum = 0;
    int count = 0;
    for (unsigned int i=0; i<data.size()*8*sizeof(unsigned int); ++i) {
        count++;
        bool sourceBit = getBit(i);
        bool otherBit = other.getBit(i);
        if (sourceBit != otherBit)
            sum += 1;
    }

    return (float)sum/(float)count;
}

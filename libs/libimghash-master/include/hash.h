#ifndef HASH_H
#define HASH_H

#include <string>
#include <vector>
#include <iomanip>
#include <assert.h>
#include <sstream>
#include <math.h>

namespace imghash {

class Hash {
public:
    Hash();
    std::string toString();
    std::string toHex();
    long long unsigned int toLongLong();
    void setBit(unsigned int index, bool value);
    bool getBit(unsigned int index) const;
    float getRMSE(const Hash& other);
    float getMSE(const Hash& other);
private:
    std::vector<unsigned int> data;
};

}

#endif

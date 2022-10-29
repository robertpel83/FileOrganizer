#ifndef TEST_H
#define TEST_H

#include "simplehasher.h"

class Test {
public:
    imghash::Source source;
    virtual void run();

protected:
    std::vector<imghash::Source> images;
};

#endif

#ifndef SOURCE_H
#define SOURCE_H

#include <string>
#include <vector>
#include <Magick++.h>
#include <iostream>
#include <fstream>
#include <ios>
#include <sys/stat.h>


namespace imghash {

class Source {
private:
    Magick::Image image;
public:
    Source();
    Source(Magick::Image image);
    Source(std::string data);
    Source(char* buffer, int size);
    Magick::PixelPacket* getPixels();
    Magick::Image getImage();
    bool loadFile(std::string fileName);
};

}

#endif

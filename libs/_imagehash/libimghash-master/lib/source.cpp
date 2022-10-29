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

#include "source.h"

using namespace imghash;

Source::Source()
{

}

Source::Source(Magick::Image image)
    :image(image)
{

}

Source::Source(std::string data)
{
    const char* raw = data.data();
    int len = data.length();
    Magick::Blob blob(data.data(), data.length());
    image.read(blob);
}

Source::Source(char *buffer, int size)
{
    Magick::Blob blob(buffer, size);
    image.magick("JPEG");
    image.read(blob);
}

Magick::PixelPacket* Source::getPixels()
{
    return image.getPixels(0,0,image.geometry().width(), image.geometry().height());
}

Magick::Image Source::getImage() {
    return image;
}

bool Source::loadFile(std::string fileName)
{
    std::ifstream input;
    input.open(fileName.c_str(), std::ios::in | std::ios::binary);
    if (input.is_open()) {
        // getting file size
        struct stat stat_buf;
        int rc = stat(fileName.c_str(), &stat_buf);
        int size = stat_buf.st_size;
        // reading data
        char* buffer = new char[size];
        input.read(buffer,size);
        Magick::Blob blob(buffer, size);
        image.read(blob);
        delete buffer;
        return true;
    }
    return false;
}

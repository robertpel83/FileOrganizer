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

#include "simplehasher.h"
#include "test.h"

class CompressionTest: public Test {
public:
    CompressionTest(std::string fileName) {
        source.loadFile(fileName);
        for (int i=1; i<99; ++i) {
            Magick::Image image = source.getImage();
            image.magick("JPEG");
            image.quality(i);
            Magick::Blob blob;
            image.write(&blob);
            imghash::Source target((char*)blob.data(), blob.length());
            images.push_back(target);
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc == 2) {
        CompressionTest test(argv[1]);
        test.run();
    }
}


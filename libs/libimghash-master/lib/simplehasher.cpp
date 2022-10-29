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

imghash::SimpleHasher::SimpleHasher(unsigned int size)
    :size(size)
{

}

imghash::Hash imghash::SimpleHasher::hashSource(Source image)
{
    Magick::Image target(image.getImage());
    target.type(Magick::TrueColorType);
    target.modifyImage();
    target.colorSpace(Magick::GRAYColorspace);
    target.filterType(Magick::PointFilter);
    Magick::Geometry resizeGeom;
    resizeGeom.aspect(true);
    resizeGeom.width(size);
    resizeGeom.height(size);
    target.scale(resizeGeom);
    Magick::Geometry geom = target.size();
    Magick::PixelPacket* pixels = target.getPixels(0,0,size,size);
    float averageColor = getAverage(pixels);
    Hash result;
    int count=0;
    for (unsigned int i=0; i<size; i++) {
        for (unsigned int j=0; j<size; j++) {
            int color = (*pixels++).green;
            bool bit = (color > averageColor);
            //printf("%3d ", color);
            result.setBit(count, bit );
            count++;
        }
        //printf("\n");
    }
    //printf("\n");
    return result;
}

float imghash::SimpleHasher::getAverage(const Magick::PixelPacket* pixels) const
{
    int sum = 0;
    int count = 0;
    for (unsigned int i=0; i<size; i++) {
        for (unsigned int j=0; j<size; j++) {
            sum += (*pixels++).green;
            count++;
        }
    }
    return (float)sum/(float)count;
}


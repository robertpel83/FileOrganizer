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

#include "test.h"

void Test::run() {
    imghash::Hasher *hasher = new imghash::SimpleHasher(8);
    imghash::Hash sourceHash = hasher->hash(source);
    int fails = 0;
    for (unsigned int i=0; i<images.size(); ++i) {
        imghash::Hash targetHash = hasher->hash(images[i]);
        float rmse = sourceHash.getRMSE(targetHash);
        printf("test %2d: RMSE=%f\n", i, rmse);
        if (rmse >= 0.5)
            fails++;
    }
    printf("Test result: %d fails\n", fails);
}

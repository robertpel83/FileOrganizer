/*
  fstlib - A C++ library for ultra fast storage and retrieval of datasets

  Copyright (C) 2017-present, Mark AJ Klik

  This file is part of fstlib.

  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this file,
  You can obtain one at https://mozilla.org/MPL/2.0/.

  https://www.mozilla.org/en-US/MPL/2.0/FAQ/

  You can contact the author at:
  - fstlib source repository : https://github.com/fstpackage/fstlib
*/


// Framework libraries
#include <blockstreamer/blockstreamer_v2.h>
#include <compression/compressor.h>
#include <interface/fstdefines.h>

using namespace std;


void fdsWriteInt64Vec_v11(ofstream &myfile, long long* int64Vector, unsigned long long nrOfRows, unsigned int compression,
  std::string annotation, bool hasAnnotation)
{
  int blockSize = 8 * BLOCKSIZE_INT64;  // block size in bytes

  if (compression == 0)
  {
    return fdsStreamUncompressed_v2(myfile, reinterpret_cast<char*>(int64Vector), nrOfRows, 8, BLOCKSIZE_INT64, nullptr, annotation, hasAnnotation);
  }

  if (compression <= 50)  // low compression: linear mix of uncompressed and LZ4_SHUF8
  {
    Compressor* compress1 = new SingleCompressor(CompAlgo::LZ4_SHUF8, 2 * compression);
    StreamCompressor* streamCompressor = new StreamLinearCompressor(compress1, (float)(2 * compression));
    streamCompressor->CompressBufferSize(blockSize);
    fdsStreamcompressed_v2(myfile, reinterpret_cast<char*>(int64Vector), nrOfRows, 8, streamCompressor, BLOCKSIZE_INT64, annotation, hasAnnotation);

    delete compress1;
    delete streamCompressor;
    return;
  }

  // higher compression: linear mix of LZ4_SHUF8 and ZSTD_SHUF8

  Compressor* compress1 = new SingleCompressor(CompAlgo::LZ4_SHUF8, 100);
  Compressor* compress2 = new SingleCompressor(CompAlgo::ZSTD_SHUF8, compression - 50);
  StreamCompressor* streamCompressor = new StreamCompositeCompressor(compress1, compress2, (float)(2 * (compression - 50)));
  streamCompressor->CompressBufferSize(blockSize);
  fdsStreamcompressed_v2(myfile, reinterpret_cast<char*>(int64Vector), nrOfRows, 8, streamCompressor, BLOCKSIZE_INT64, annotation, hasAnnotation);

  delete compress1;
  delete compress2;
  delete streamCompressor;

  return;
}


void fdsReadInt64Vec_v11(istream &myfile, long long* int64Vector, unsigned long long blockPos, unsigned long long startRow,
  unsigned long long length, unsigned long long size)
{
  std::string annotation;
  bool hasAnnotation;

  return fdsReadColumn_v2(myfile, reinterpret_cast<char*>(int64Vector), blockPos, startRow, length, size, 8, annotation,
    BATCH_SIZE_READ_INT64, hasAnnotation);
}

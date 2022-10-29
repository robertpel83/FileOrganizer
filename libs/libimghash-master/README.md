libimghash
==========

Image hashing library for C++. Main features:
* Fast. Can hash 60 images/sec on [lena.bmp](http://www.ece.rice.edu/~wakin/images/lena512.bmp) on single core.
* Supports a lot of image formats.
* Simple. You need only 3 lines of code to hash an image.

**Installation**

Tle library is built with CMake in a common way.

    cmake CMakeLists.txt
    make
    make install
    
There's also stuff for building debian/ubuntu packages and a PPA with them.

**How to use**

A small example showing library usage:

    imghash::Source data;
    data.loadFile("foo.jpg");
    imghash::Hasher *hasher = new imghash::SimpleHasher();
    imghash::Hash hash = hasher->hash(data);
    std::cout << "hash: " << hash.toHex() << "\n";
    delete hasher;

In a `examples/` subdirectory there is an examples showing more advanced aspects of the library.

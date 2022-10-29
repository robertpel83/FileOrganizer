project(libimghash)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
FIND_PACKAGE(ImageMagick COMPONENTS Magick++ REQUIRED)
include_directories("${ImageMagick_INCLUDE_DIRS}")
include_directories("include")
add_definitions("-DMAGICKCORE_QUANTUM_DEPTH=8" "-DMAGICKCORE_HDRI_ENABLE=0")
add_library(imghash SHARED lib/source.cpp include/source.h lib/hash.cpp include/hash.h lib/hasher.cpp include/hasher.h lib/simplehasher.cpp include/simplehasher.h include/imghash_c.h lib/imghash_c.cpp)
target_link_libraries(imghash ${ImageMagick_LIBRARIES})

add_library(imghash-c SHARED lib/imghash_c.cpp include/imghash_c.h)
target_link_libraries(imghash-c imghash)

add_executable(singlefile examples/singlefile.cpp)
target_link_libraries(singlefile imghash ${ImageMagick_LIBRARIES})

add_executable(delta examples/delta.cpp)
target_link_libraries(delta imghash ${ImageMagick_LIBRARIES})

add_executable(benchmark examples/benchmark.cpp)
target_link_libraries(benchmark imghash ${ImageMagick_LIBRARIES})

add_executable(compression_test tests/compression_test.cpp tests/test.cpp tests/test.h)
target_link_libraries(compression_test imghash ${ImageMagick_LIBRARIES})

add_executable(resize_test tests/resize_test.cpp tests/test.cpp tests/test.h)
target_link_libraries(resize_test imghash ${ImageMagick_LIBRARIES})

install(TARGETS imghash
    LIBRARY DESTINATION lib
)

install(FILES include/source.h include/hash.h include/hasher.h include/simplehasher.h include/imghash_c.h
    DESTINATION include/imghash
)

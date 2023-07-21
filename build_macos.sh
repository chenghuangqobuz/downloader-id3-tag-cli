#!/bin/bash

rm -rf build_dist
mkdir build_dist
pushd build_dist

cmake -G Xcode \
    -DTaglib_INCLUDE_DIRS=../tmp/local/taglib/include \
    -DTaglib_LIBRARIES=../tmp/local/taglib/lib/libtag.a \
    ..
    
cmake --build . --config Release

popd build_dist

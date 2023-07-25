#!/bin/bash

rm -rf build_dist
mkdir build_dist
pushd build_dist

cmake \
    -DTaglib_INCLUDE_DIRS=../tmp/local/taglib/include \
    -DTaglib_LIBRARIES=../tmp/local/taglib/lib/tag.lib \
    ..

cmake --build . --config Release

popd

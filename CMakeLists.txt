cmake_minimum_required(VERSION 3.22)
project(id3-tag-cli LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (APPLE)
  set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")
  set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package(Taglib REQUIRED)

include(FetchContent)
macro(fetch_dependency name repo tag)
  FetchContent_Declare(${name}
    GIT_REPOSITORY ${repo}
    GIT_TAG ${tag}
  )
  FetchContent_GetProperties(${name})
  if (NOT ${name}_POPULATED)
    message(STATUS "Populating ${name} dependencies")
    FetchContent_Populate(${name})
  endif()
endmacro()

fetch_dependency(cli11 https://github.com/CLIUtils/CLI11.git v2.3.2)
add_subdirectory(${cli11_SOURCE_DIR})

add_executable(id3-tag-cli
  src/arguments.hpp
  src/help.hpp
  src/main.cpp
  src/platform.hpp)
target_link_libraries(id3-tag-cli Taglib::Taglib CLI11)

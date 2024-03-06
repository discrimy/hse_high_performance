# Set the minimum version of CMake that can be used
# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 3.5)

# Set the project name
project (maldebrot)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNO_FREETYPE -O3" CACHE STRING "Set C++ Compiler Flags" FORCE)

add_executable(maldebrot_singlethread src/maldebrot_singlethread.cpp src/pngwriter.cc)
target_include_directories(maldebrot_singlethread PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_executable(maldebrot_multithread src/maldebrot_multithread.cpp src/pngwriter.cc)
target_include_directories(maldebrot_multithread PRIVATE ${PROJECT_SOURCE_DIR}/include)


find_package(PNGwriter 0.7.0)
if(PNGwriter_FOUND)
  target_link_libraries(maldebrot_singlethread PRIVATE PNGwriter::PNGwriter)
  target_link_libraries(maldebrot_multithread PRIVATE PNGwriter::PNGwriter)
endif(PNGwriter_FOUND)
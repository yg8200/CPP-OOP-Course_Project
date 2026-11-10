cmake_minimum_required(VERSION 3.16)
project(Adventure_Project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    include/core
    include/entities
    include/recording
    include/utils
)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.h")

add_executable(Adventure_Project ${SOURCES} ${HEADERS})

if(NOT MSVC)
    target_compile_options(Adventure_Project PRIVATE -Wall -Wextra)
endif()

# The game finds its 'assets' folder by itself: it looks in the current directory and next to the
# executable. Copy the assets next to the build output so `./Adventure_Project` works from build/.
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

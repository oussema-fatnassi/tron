cmake_minimum_required(VERSION 3.10)
project(Tron)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SOURCE_FILES
    "src/*.cpp"
)

file(GLOB HEADER_FILES
    "include/*.h"
)

add_executable(tron main.cpp ${SOURCE_FILES})


if(MSVC)
    source_group("Sources" FILES ${SOURCE_FILES})

    source_group("Headers" FILES ${HEADER_FILES})
endif()

if(MSVC)
    target_compile_options(tron PRIVATE /W4)
else()
    target_compile_options(tron PRIVATE -Wall -Wextra -Wpedantic)
endif()
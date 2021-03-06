cmake_minimum_required(VERSION 3.4)
project(criti C CXX)

# Setup output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

# Enable C++ 11 standard
if (CMAKE_VERSION VERSION_LESS "3.1.3")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "-std=gnu++11 ${CMAKE_CXX_FLAGS}")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    endif ()
else ()
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif ()

# Enable C99 standard
if (CMAKE_VERSION VERSION_LESS "3.1.3")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")
    elseif (CMAKE_C_COMPILER_ID STREQUAL "Intel")
        set(CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")
    elseif (CMAKE_C_COMPILER_ID MATCHES "Clang")
        set(CMAKE_C_FLAGS "-std=c99 ${CMAKE_C_FLAGS}")
    endif ()
else ()
    set(CMAKE_C_STANDARD 99)
    set(CMAKE_C_STANDARD_REQUIRED ON)
endif ()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(COMPILE_FLAGS "-m64 -Wall -Wextra -pedantic")
set(IGNORED_WARNINGS "-Wno-pointer-arith")
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(IGNORED_WARNINGS "${IGNORED_WARNINGS} -Wno-newline-eof")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILE_FLAGS} ${IGNORED_WARNINGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS} ${IGNORED_WARNINGS}")
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")
endif()

# Output compiling flags
if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "")
    message(STATUS "CMAKE build type: ${CMAKE_BUILD_TYPE}")
endif ()

message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C compiler flags: ${CMAKE_C_FLAGS}")
message(STATUS "CXX compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "CXX compiler flags: ${CMAKE_CXX_FLAGS}")

#===============================================================================
# Architecture specific definitions
#===============================================================================

if (${UNIX})
    add_definitions(-DUNIX)
endif ()

#===============================================================================
# git SHA1 hash
#===============================================================================

execute_process(COMMAND git rev-parse HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE GIT_SHA1_SUCCESS
        OUTPUT_VARIABLE GIT_SHA1
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
if (GIT_SHA1_SUCCESS EQUAL 0)
    add_definitions(-DGIT_SHA1="${GIT_SHA1}")
endif ()

file(GLOB SRC_H ${CMAKE_SOURCE_DIR}/src/*.h)
file(GLOB SRC_C ${CMAKE_SOURCE_DIR}/src/*.c)
file(GLOB SRC_CPP ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(criti ${SRC_C} ${SRC_CPP} ${SRC_H})

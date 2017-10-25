cmake_minimum_required(VERSION 3.4)
project(ttyw C CXX)

# Enable C++ 11 standard
if (CMAKE_VERSION VERSION_LESS "3.1.3")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "--std=gnu++11 ${CMAKE_CXX_FLAGS}")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
        set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    endif ()
else ()
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif ()

# Output compiling flags
if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "")
    message(STATUS "CMAKE build type: ${CMAKE_BUILD_TYPE}")
endif()

message(STATUS "CXX compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "CXX compiler flags: ${CMAKE_CXX_FLAGS}")

#===============================================================================
# Architecture specific definitions
#===============================================================================

if (${UNIX})
    add_definitions(-DUNIX)
endif()

#===============================================================================
# git SHA1 hash
#===============================================================================

execute_process(COMMAND git rev-parse HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE GIT_SHA1_SUCCESS
        OUTPUT_VARIABLE GIT_SHA1
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
if(GIT_SHA1_SUCCESS EQUAL 0)
    add_definitions(-DGIT_SHA1="${GIT_SHA1}")
endif()

file(GLOB SRC_H ${CMAKE_SOURCE_DIR}/*.h)
file(GLOB SRC_C ${CMAKE_SOURCE_DIR}/*.c)
file(GLOB SRC_CPP ${CMAKE_SOURCE_DIR}/*.cpp)

add_executable(ttyw ${SRC_C} ${SRC_CPP} ${SRC_H})
cmake_minimum_required(VERSION 3.4)
project(covscript-jvm)
include_directories(include)
include_directories(../covscript/include)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS " -fPIC -O3")

if (NOT DEFINED ENV{JAVA_HOME})
    message(FATAL_ERROR "Environment variable required: JAVA_HOME")
endif ()

set(JAVA_HOME $ENV{JAVA_HOME})
LINK_DIRECTORIES(${JAVA_HOME}/jre/lib)
LINK_DIRECTORIES(${JAVA_HOME}/jre/lib/server)
INCLUDE_DIRECTORIES(${JAVA_HOME}/include)
set(CMAKE_BUILD_RPATH ${JAVA_HOME}/jre/lib ${JAVA_HOME}/jre/lib/server)

IF (WIN32)
    message(FATAL_ERROR "Windows is not supported")
ELSEIF (APPLE)
    INCLUDE_DIRECTORIES(${JAVA_HOME}/include/darwin)
ELSEIF (UNIX)
    message(FATAL_ERROR "Unix is not supported.")
ENDIF ()

add_library(csjvm SHARED jvm.cpp)
target_link_libraries(csjvm jvm)
set_target_properties(csjvm PROPERTIES PREFIX "")
set_target_properties(csjvm PROPERTIES SUFFIX ".cse")


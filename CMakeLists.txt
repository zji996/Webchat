cmake_minimum_required(VERSION 2.8)
project(platinum)

set(CMAKE_VERBOSE_MAKEFILE ON) 
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

include_directories(.)
include_directories(./src)
include_directories(./platinum)

set(LIB_SRC
	platinum/log.cc
	platinum/util.cc
	platinum/thread.cc
    platinum/mutex.cc
    platinum/threadpool.cc
    platinum/webserver.cc
	)
set(LIBS
        platinum
        pthread)

add_library(platinum SHARED ${LIB_SRC})
add_executable(test tests/test.cc)
add_dependencies(test platinum)
target_link_libraries(test ${LIBS})

add_executable(webchat src/main.cc src/webchat.cc)
add_dependencies(webchat platinum)
target_link_libraries(webchat ${LIBS})


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

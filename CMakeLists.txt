# CMake entry point
cmake_minimum_required (VERSION 3.0)
set (CMAKE_CXX_STANDARD 11)

project (Lab01)

#在当前目录下搜索头文件及源文件
file(GLOB_RECURSE CURRENT_HEADERS  *.h *.hpp *inc)
file(GLOB CURRENT_SOURCES  *.cpp)
#为头文件和源文件进行分组
source_group("Include" FILES ${CURRENT_HEADERS})
source_group("Source" FILES ${CURRENT_SOURCES})

message(STATUS "CURRENT_HEADERS in ${CMAKE_SOURCE_DIR}")

add_executable(Rendering ${CURRENT_HEADERS} ${CURRENT_SOURCES})

#增加并行支持，需要引入并行库
find_package(OpenMP REQUIRED)
if(OPENMP_CXX_FOUND)
  target_link_libraries(Rendering PUBLIC OpenMP::OpenMP_CXX)
endif()
# FIND_PACKAGE( OpenMP REQUIRED)
# if(OPENMP_FOUND)
# message("OPENMP FOUND")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
# endif()

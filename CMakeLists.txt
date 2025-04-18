#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project(vdecode VERSION 1.0.0 LANGUAGES C CXX )
add_compile_options(-w)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
# add_definitions(-std=c++17)

#----------------------------------------------------------------------------
# Find ROOT (required package)
#
find_package(ROOT REQUIRED COMPONENTS TMVA ROOTVecOps ROOTDataFrame Geom Eve)
find_package(Python COMPONENTS Interpreter Development REQUIRED)
# find_package(yaml-cpp REQUIRED)

# include_directories(${YAML_CPP_INCLUDE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the ROOT libraries
#
add_executable(vdecode main.cxx ${sources} ${headers})
target_link_libraries(vdecode ${ROOT_LIBRARIES})


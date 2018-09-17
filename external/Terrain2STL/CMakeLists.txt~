#----------------------------------------------------------------------------
# Setup the project

message(STATUS "TERRAIN2STL CMAKE Build Generator ")
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(TERRAIN)

#----------------------------------------------------------------------------
# Set the build flags
SET( BUILD_SHARED_LIBS "ON" )

#----------------------------------------------------------------------------
# Main TERRAIN2STL Setup

message(STATUS "Loading TerrainToSTL Files")
file(GLOB headers ${PROJECT_SOURCE_DIR}/src/*.hh)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB cresta_include ${PROJECT_SOURCE_DIR}/src/ )

include_directories(${PROJECT_SOURCE_DIR}/src/ )

set(TERRAIN2STL_DIRS "-L${PROJECT_BINARY_DIR}/lib/")
set(TERRAIN2STL_INCS "-I${PROJECT_BINARY_DIR}/ -I${PROJECT_SOURCE_DIR}/src/")
set(TERRAIN2STL_LIBS "${PROJECT_NAME}")

#----------------------------------------------------------------------------
message(STATUS "Consolidating variables.")

SET(DEPEND_DIRS_STR "${TERRAIN2STL_DIRS}")
string(STRIP "${DEPEND_DIRS_STR}" DEPEND_DIRS_STR)

SET(DEPEND_LIBS "")
string(REPLACE ";"  " "    DEPEND_LIBS  "${DEPEND_LIBS}")
string(REPLACE "  "  " "   DEPEND_LIBS  "${DEPEND_LIBS}")
string(STRIP "${DEPEND_LIBS}" DEPEND_LIBS)
string(REPLACE " "  ";"    DEPEND_LIBS  "${DEPEND_LIBS}")
string(REPLACE ";"   " -l" DEPEND_LIBS_STR ";${DEPEND_LIBS}")
string(STRIP "${DEPEND_LIBS}" DEPEND_LIBS)
string(STRIP "${DEPEND_LIBS_STR}" DEPEND_LIBS_STR)

SET(DEPEND_INCS_STR "${TERRAIN2STL_INCS}")

message(STATUS "--> DEPEND_DIRS = ${DEPEND_DIRS_STR}")
message(STATUS "--> DEPEND_LIBS = ${DEPEND_LIBS}")
message(STATUS "--> DEPEND_LIBS_STR = ${DEPEND_LIBS_STR}")
message(STATUS "--> DEPEND_INCS = ${DEPEND_INCS_STR}")

#----------------------------------------------------------------------------
message(STATUS "Setting Build Flags")
SET(CMAKE_CXX_FLAGS_RELEASE "-std=c++11 -Wno-shadow -Wno-reorder -O3 -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter -Wno-sign-compare -Wno-unused-private-field -rdynamic")
SET(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -Wno-shadow -Wno-reorder -Wno-unused-parameter -Wno-unused-variable -Wno-sign-compare -Wno-unused-private-field -rdynamic")

if (CMAKE_BUILD_TYPE MATCHES RELEASE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
endif()
message(STATUS "CMAKE_CXX_FLAGS : ${CMAKE_CXX_FLAGS}")


#----------------------------------------------------------------------------
# Configure the standard setup script
message(STATUS "Generating Setup Script")
configure_file(cmake/setup.sh.in
  "${PROJECT_BINARY_DIR}/setup.sh" @ONLY)

#----------------------------------------------------------------------------
# Build the TERRAIN2STL Library
message(STATUS "Building TERRAIN2STL Library")
file(MAKE_DIRECTORY  ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

add_library(${PROJECT_NAME} SHARED ${sources} ${headers})
target_link_libraries(${PROJECT_NAME} ${DEPEND_DIRS_STR} ${DEPEND_LIBS})

#----------------------------------------------------------------------------
# Add the executables, and link them to the libraries
message(STATUS "Building TERRAIN2STL Applications")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(terrain2stl ${PROJECT_SOURCE_DIR}/app/terrain2stl.cc)
target_link_libraries(terrain2stl ${DEPEND_DIRS_STR} ${DEPEND_LIBS} ${TERRAIN2STL_LIBS})

#if(USE_PUMAS)
#add_executable(pumas_example ${PROJECT_SOURCE_DIR}/app/pumas/pumas_example.cc)
#target_DEPEND_LIBS(pumas_example  ${DEPEND_DIRS} ${DEPEND_LIBS} ${PROJECT_NAME})
#endif()

#----------------------------------------------------------------------------
# Install headers and libraries to sub folder
if (CMAKE_INSTALL_PREFIX STREQUAL "" OR CMAKE_INSTALL_PREFIX STREQUAL "/usr/local")
  set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/")
elseif(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/")
endif()

INSTALL(DIRECTORY DESTINATION include)
INSTALL(FILES ${headers} DESTINATION include)

#----------------------------------------------------------------------------
# Finito
message(STATUS "Configuration Complete!")






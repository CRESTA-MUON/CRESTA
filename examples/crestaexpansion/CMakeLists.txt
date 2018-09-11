#----------------------------------------------------------------------------
# Setup the project

message(STATUS "CRESTA EXPANSION CMAKE Build Generator ")
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(EXPANSION)

#----------------------------------------------------------------------------
# Set the build flags

message(STATUS "Setting Build Flags")
SET(CMAKE_CXX_FLAGS_RELEASE "-std=c++11 -Wno-shadow -Wno-reorder -O3 -Wno-unused-function -rdynamic")
SET(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -Wno-shadow -Wno-reorder -rdynamic")

if (CMAKE_BUILD_TYPE MATCHES RELEASE)
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_RELEASE})
else()
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_DEBUG})
endif()
SET( BUILD_SHARED_LIBS "ON" )


#----------------------------------------------------------------------------
# Main EXPANSION Setup

message(STATUS "Loading Main EXPANSION Files")
file(GLOB headers ${PROJECT_SOURCE_DIR}/src/*.hh ${PROJECT_SOURCE_DIR}/src/*/*.hh)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc ${PROJECT_SOURCE_DIR}/src/*/*.cc)
file(GLOB expansion_include ${PROJECT_SOURCE_DIR}/src/* ${PROJECT_SOURCE_DIR}/src/*/* ${PROJECT_SOURCE_DIR}/src/ )

include_directories(${PROJECT_SOURCE_DIR}/src/ ${PROJECT_SOURCE_DIR}/)

set(CUSTOM_PROJECT_DIRS "-L${PROJECT_BINARY_DIR}/")
set(CUSTOM_PROJECT_INCS "-I${PROJECT_BINARY_DIR}/ -I${PROJECT_SOURCE_DIR}/src/")
set(CUSTOM_PROJECT_LIBS "${PROJECT_NAME}")


#----------------------------------------------------------------------------
# Main CRESTA Setup

message(STATUS "Loading CRESTA")
SET(CRESTA $ENV{CRESTA})
SET(CRESTA_LIB_DIR $ENV{CRESTA_LIB_DIR})
SET(CRESTA_INC_DIR $ENV{CRESTA_INC_DIR})
SET(CRESTA_INC_FLAGS $ENV{CRESTA_INC_FLAGS})
SET(CRESTA_DIRS "-L${CRESTA_LIB_DIR}")
SET(CRESTA_INCS "${CRESTA_INC_FLAGS}")
SET(CRESTA_LIBS ";CRESTA")

string(REPLACE " -I"  ";" CRESTA_INC_STR "${CRESTA_INC_FLAGS}")
string(REPLACE "-I"  ";" CRESTA_INC_STR "${CRESTA_INC_STR}")
include_directories(${CRESTA_INC_STR})

message(STATUS "INCLUDE DIRS : ${INCLUDE_DIRECTORIES} ${CRESTA_INC_STR}")

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default

message(STATUS " Loading Geant4")
add_definitions(-DG4ANALYSIS_USE_ROOT)
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

SET(G4LIB "$ENV{G4LIB}")
SET(GEANT4_DIRS "-L${G4LIB}")
SET(GEANT4_INCS "-I${Geant4_INCLUDE_DIR}")
SET(GEANT4_LIBS ";${Geant4_LIBRARIES}")
include_directories(${Geant4_INCLUDE_DIR})



#----------------------------------------------------------------------------
# Find ROOT variables if the variable G4ANALYSIS_USE_ROOT is set

message(STATUS "Loading ROOT")
add_definitions(-DG4ANALYSIS_USE_ROOT)
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS Core Cint RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript Matrix Physics MathCore Thread Minuit Minuit2)
if(EXISTS ${ROOT_USE_FILE})
  include(${ROOT_USE_FILE})
endif()

string(REPLACE ";" " -L" ROOT_LIBRARIES_STR ";${ROOT_LIBRARIES}")

SET(ROOTSYS "$ENV{ROOTSYS}")
SET(ROOTSYS_DIRS "-L${ROOTSYS}/lib/")
SET(ROOTSYS_INCS "-I${ROOTSYS}/include/")
SET(ROOTSYS_LINKS ";Core;Cint;RIO;Net;Hist;Graf;Graf3d;Gpad;Tree;Rint;Postscript;Matrix;Physics;MathCore;Thread;Minuit;Minuit2")

# Remove ROOT issues
SET(ROOTSYS_LIBS "${ROOT_LIBRARIES}")
string(REPLACE "-L${ROOTSYS}/lib"  ""    ROOTSYS_LIBS_STR  "${ROOTSYS_LIBS}")
string(REPLACE "-pthread -lm -ldl -rdynamic" ""  ROOTSYS_LIBS_STR  "${ROOTSYS_LIBS_STR}")
string(REPLACE " -l"  " "    ROOTSYS_LIBS_STR  "${ROOTSYS_LIBS_STR}")
string(REPLACE "  "  " "    ROOTSYS_LIBS_STR  "${ROOTSYS_LIBS_STR}")
string(STRIP "${ROOTSYS_LIBS_STR}" ROOTSYS_LIBS_STR)
string(REPLACE " "  ";"    ROOTSYS_LIBS_STR  "${ROOTSYS_LIBS_STR}")
SET(ROOTSYS_LIBS "${ROOTSYS_LIBS_STR};Minuit2")
include_directories(${ROOT_INCLUDE_DIR})



#----------------------------------------------------------------------------
# GPerf Tools Support

if(USE_GPERF)
  message(STATUS "GPerf Build Option ")
  
  if(GPERF_DIR)
    message(STATUS "Reading GPERF_DIR=${GPERF_DIR}")
  else()
    message(FATAL_ERROR "NO GPERF_DIR GIVEN")
  endif()
  
  if (EXISTS ${GPERF_DIR})
  else()
    message(FATAL_ERROR "Invalid GPERF_DIR GIVEN")
  endif()

  set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH} {GPERF_DIR}/lib/")
  set(GPERF_DIRS "-L{GPERF_DIR}/lib/")
  set(GPERF_INCS "-I${GPERF_DIR}/include/")
  set(GPERF_LIBS "profiler")
  include_directories(${GPERF_DIR}/include/)

else()
 message(STATUS "No GPerf Support : [-DUSE_GPERF=0]")
endif()

#----------------------------------------------------------------------------
message(STATUS "Consolidating variables.")

SET(DEPEND_DIRS_STR "${CUSTOM_PROJECT_DIRS} ${CRESTA_DIRS} ${GEANT4_DIRS} ${ROOTSYS_DIRS} ${CADMESH_DIRS} ${GPERF_DIRS}")

SET(DEPEND_LIBS "${CRESTA_LIBS} ${GEANT4_LIBS} ${ROOTSYS_LIBS} ${CADMESH_LIBS} ${GPERF_LIBS}")
string(REPLACE ";"  " "    DEPEND_LIBS  "${DEPEND_LIBS}")
string(REPLACE "  "  " "   DEPEND_LIBS  "${DEPEND_LIBS}")
string(STRIP "${DEPEND_LIBS}" DEPEND_LIBS)
string(REPLACE " "  ";"    DEPEND_LIBS  "${DEPEND_LIBS}")
string(REPLACE ";"   " -l" DEPEND_LIBS_STR ";${DEPEND_LIBS}")

SET(DEPEND_INCS_STR "${CUSTOM_PROJECT_INCS} ${CRESTA_INCS} ${GEANT4_INCS} ${ROOTSYS_INCS} ${CADMESH_INCS} ${GPERF_INCS}")

message(STATUS "--> DEPEND_DIRS = ${DEPEND_DIRS_STR}")
message(STATUS "--> DEPEND_LIBS = ${DEPEND_LIBS}")
message(STATUS "--> DEPEND_LIBS_STR = ${DEPEND_LIBS_STR}")
message(STATUS "--> DEPEND_INCS = ${DEPEND_INCS_STR}")

SET(BUILDER_DIRS_STR "${CUSTOM_PROJECT_DIRS} ${CRESTA_DIRS} ${GEANT4_DIRS} ${ROOTSYS_DIRS} ${CADMESH_DIRS} ${GPERF_DIRS}")

SET(BUILDER_LIBS "${CUSTOM_PROJECT_LIBS} ${CRESTA_LIBS} ${GEANT4_LIBS} ${ROOTSYS_LINKS} ${CADMESH_LIBS} ${GPERF_LIBS}")
string(REPLACE ";"  " "    BUILDER_LIBS  "${BUILDER_LIBS}")
string(REPLACE "  "  " "   BUILDER_LIBS  "${BUILDER_LIBS}")
string(STRIP "${BUILDER_LIBS}" BUILDER_LIBS)
string(REPLACE " "  ";"    BUILDER_LIBS  "${BUILDER_LIBS}")
string(REPLACE ";"   " -l" BUILDER_LIBS_STR ";${BUILDER_LIBS}")


string(REPLACE ";"  " -I"    BUILDER_SOURCES  ";${sources}")
string(REPLACE ";"  " -I"    BUILDER_HEADERS  ";${headers}")
SET(BUILDER_INCS_STR "${BUILDER_HEADERS} ${CUSTOM_PROJECT_INCS} ${CRESTA_INCS} ${GEANT4_INCS} ${ROOTSYS_INCS} ${CADMESH_INCS} ${GPERF_INCS}")


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
# Build the EXPANSION Library
message(STATUS "Building EXPANSION Library")

file(MAKE_DIRECTORY  ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

add_library(${PROJECT_NAME} SHARED ${sources} ${headers})
target_link_libraries(${PROJECT_NAME} ${DEPEND_DIRS_STR} ${DEPEND_LIBS})

#----------------------------------------------------------------------------
# Add the executables, and link them to the libraries
message(STATUS "Building EXPANSION Applications")

file(MAKE_DIRECTORY  ${PROJECT_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(expansion_g4sim ${PROJECT_SOURCE_DIR}/app/expansion_g4sim.cc)
target_link_libraries(expansion_g4sim ${DEPEND_DIRS_STR} ${DEPEND_LIBS} ${CUSTOM_PROJECT_LIBS})

#----------------------------------------------------------------------------
# Finito
message(STATUS "Configuration Complete!")






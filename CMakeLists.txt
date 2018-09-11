#----------------------------------------------------------------------------
# Setup the project

message(STATUS "CRESTA CMAKE Build Generator ")
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(CRESTA)

#----------------------------------------------------------------------------
# Set the build flags
SET( BUILD_SHARED_LIBS "ON" )

#----------------------------------------------------------------------------
# Main CRESTA Setup

message(STATUS "Loading Main Cresta Files")
file(GLOB headers ${PROJECT_SOURCE_DIR}/src/*/*.hh ${PROJECT_SOURCE_DIR}/src/geo/*/*.hh)
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*/*.cc ${PROJECT_SOURCE_DIR}/src/geo/*/*.cc)
file(GLOB cresta_include ${PROJECT_SOURCE_DIR}/src/*/ ${PROJECT_SOURCE_DIR}/src/*/*/ ${PROJECT_SOURCE_DIR}/src/ )

include_directories(${PROJECT_SOURCE_DIR}/src/ ${PROJECT_SOURCE_DIR}/geo/)

set(CRESTA_DIRS "-L${PROJECT_BINARY_DIR}/lib/")
set(CRESTA_INCS "-I${PROJECT_BINARY_DIR}/ -I${PROJECT_SOURCE_DIR}/src/")
set(CRESTA_LIBS "${PROJECT_NAME}")



#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default

message(STATUS "Loading Geant4")
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
SET(ROOTSYS_LIBS "${ROOTSYS_LIBS_STR}")
include_directories(${ROOT_INCLUDE_DIR})


#----------------------------------------------------------------------------
# USE CADMESH
SET(CADMESH_DIRS "")
SET(CADMESH_INCS "")
SET(CADMESH_LIBS "")

if(USE_CADMESH)
  message(STATUS "Loading CADMESH : [-DUSE_CADMESH=1]")
  find_package(cadmesh)

  # Look for CADMESH_DIR
  if(CADMESH_DIR) 
    message(STATUS "--> CADMESH_DIR=${CADMESH_DIR}")
  else()
    message(STATUS "BRROW2 Searching for CADMESH in 'cresta/external/cadmesh/install/'")
    message(STATUS "BRROW2 To overwrite directory, use [-DCADMESH_DIR=...]")
    set(CADMESH_DIR "${PROJECT_SOURCE_DIR}/external/cadmesh/install/")
  endif()

  # Check it exists
  if(EXISTS ${CADMESH_DIR})
  else()
    message(STATUS "BRROW2 Need CADMESH build to build with -DUSE_CADMESH=1")
    message(FATAL_ERROR "BRROW2 Scripts to download and build CADMESH are in cresta/external.")
  endif()

  # Add libraries and headers if so.
  add_definitions(-D__USE_CADMESH__=1)
  set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH} ${CADMESH_DIR}/lib/")
  set(CADMESH_DIRS "-L${CADMESH_DIR}/lib/")
  set(CADMESH_INCS "-I${CADMESH_DIR}/include/")
  SET(CADMESH_LIBS ";assimp;cadmesh;tet")
  include_directories(${CADMESH_DIR}/include/)

else()
  message(STATUS "No CADMESH Support : [-DUSE_CADMESH=0]")
endif()


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
# USE PUMAS FLAG
#set(pumas_sources "")
#set(pumas_headers "")
#if(USE_PUMAS)
#  message(STATUS "Including PUMAS in build.")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__USE_PUMAS__=1")
#  file(GLOB pumas_sources ${PROJECT_SOURCE_DIR}/src/pumas/*.c ${PROJECT_SOURCE_DIR}/src/pumas/*.cc)
#  file(GLOB pumas_headers ${PROJECT_SOURCE_DIR}/src/pumas/*.h ${PROJECT_SOURCE_DIR}/src/pumas/*.hh)
#add_library (pumas ${pumas_sources} ${pumas_headers})
#set(DEPEND_LIBS "${DEPEND_LIBS} pumas")
#endif()



#----------------------------------------------------------------------------
message(STATUS "Consolidating variables.")

SET(DEPEND_DIRS_STR "${CRESTA_DIRS} ${GEANT4_DIRS} ${ROOTSYS_DIRS} ${CADMESH_DIRS} ${GPERF_DIRS}")

SET(DEPEND_LIBS "${CRESTA_LIBS} ${GEANT4_LIBS} ${ROOTSYS_LIBS} ${CADMESH_LIBS} ${GPERF_LIBS}")
string(REPLACE ";"  " "    DEPEND_LIBS  "${DEPEND_LIBS}")
string(REPLACE "  "  " "   DEPEND_LIBS  "${DEPEND_LIBS}")
string(STRIP "${DEPEND_LIBS}" DEPEND_LIBS)
string(REPLACE " "  ";"    DEPEND_LIBS  "${DEPEND_LIBS}")
string(REPLACE ";"   " -l" DEPEND_LIBS_STR ";${DEPEND_LIBS}")

SET(DEPEND_INCS_STR "${CRESTA_INCS} ${GEANT4_INCS} ${ROOTSYS_INCS} ${CADMESH_INCS} ${GPERF_INCS}")

message(STATUS "--> DEPEND_DIRS = ${DEPEND_DIRS_STR}")
message(STATUS "--> DEPEND_LIBS = ${DEPEND_LIBS}")
message(STATUS "--> DEPEND_LIBS_STR = ${DEPEND_LIBS_STR}")
message(STATUS "--> DEPEND_INCS = ${DEPEND_INCS_STR}")

SET(BUILDER_DIRS_STR "${CRESTA_DIRS} ${GEANT4_DIRS} ${ROOTSYS_DIRS} ${CADMESH_DIRS} ${GPERF_DIRS}")

SET(BUILDER_LIBS "${CRESTA_LIBS} ${GEANT4_LIBS} ${ROOTSYS_LINKS} ${CADMESH_LIBS} ${GPERF_LIBS}")
string(REPLACE ";"  " "    BUILDER_LIBS  "${BUILDER_LIBS}")
string(REPLACE "  "  " "   BUILDER_LIBS  "${BUILDER_LIBS}")
string(STRIP "${BUILDER_LIBS}" BUILDER_LIBS)
string(REPLACE " "  ";"    BUILDER_LIBS  "${BUILDER_LIBS}")
string(REPLACE ";"   " -l" BUILDER_LIBS_STR ";${BUILDER_LIBS}")


string(REPLACE ";"  " -I"    BUILDER_SOURCES  ";${sources}")
string(REPLACE ";"  " -I"    BUILDER_HEADERS  ";${headers}")
SET(BUILDER_INCS_STR "${BUILDER_HEADERS} ${CRESTA_INCS} ${GEANT4_INCS} ${ROOTSYS_INCS} ${CADMESH_INCS} ${GPERF_INCS}")


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
message(STATUS "Generating Dynamic Builder Script")
file(MAKE_DIRECTORY  ${PROJECT_BINARY_DIR}/bin)

configure_file(cmake/cresta_buildlib.in
  "${PROJECT_BINARY_DIR}/bin/cresta_buildlib" @ONLY)


#----------------------------------------------------------------------------
# Build the CRESTA Library
message(STATUS "Building CRESTA Library")
file(MAKE_DIRECTORY  ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

add_library(${PROJECT_NAME} SHARED ${sources} ${headers})
target_link_libraries(${PROJECT_NAME} ${DEPEND_DIRS_STR} ${DEPEND_LIBS})

#----------------------------------------------------------------------------
# Add the executables, and link them to the libraries
message(STATUS "Building CRESTA Applications")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(cosmicraysim ${PROJECT_SOURCE_DIR}/app/standard/cosmicraysim.cc)
target_link_libraries(cosmicraysim ${DEPEND_DIRS_STR} ${DEPEND_LIBS})

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






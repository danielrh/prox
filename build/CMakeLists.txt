#CMakeLists.txt
#Copyright (c) 2008, Ewen Cheslack-Postava
#All rights reserved.
#
#Redistribution and use in source and binary forms, with or without
#modification, are permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright notice,
#      this list of conditions and the following disclaimer in the documentation
#      and/or other materials provided with the distribution.
#    * Neither the name of the libprox nor the names of its contributors
#      may be used to endorse or promote products derived from this software
#      without specific prior written permission.
#
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
#ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
#ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#cmake options

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
SET(CMAKE_VERBOSE_MAKEFILE OFF)
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Default CACHE STRING
      "Build types include: Default Debug Release None RelWithDebInfo." FORCE
      )
ENDIF()
#-fno-rtti
SET(CMAKE_CXX_FLAGS "-Wall -Wno-unused-function -Wno-unused-variable -fvisibility=hidden")
#-fno-rtti
SET(CMAKE_CXX_FLAGS_DEFAULT "-O0 -g2 -Wall -Wno-unused-function -Wno-unused-variable -fvisibility=hidden" CACHE STRING
    "Built cxxflags with default settings."
    FORCE )
SET( CMAKE_C_FLAGS_DEFAULT "-g2 -Wall -Wno-unused-function -Wno-unused-variable -fvisibility=hidden" CACHE STRING
    "Built cflags with default settings."
    FORCE )
SET( CMAKE_EXE_LINKER_FLAGS_DEFAULT
    "" CACHE STRING
    "Linking binaries with default settings."
    FORCE )
SET( CMAKE_SHARED_LINKER_FLAGS_DEFAULT
    "" CACHE STRING
    "Building shared libraries with default settings."
    FORCE )
MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_DEFAULT
    CMAKE_C_FLAGS_DEFAULT
    CMAKE_EXE_LINKER_FLAGS_DEFAULT
    CMAKE_SHARED_LINKER_FLAGS_DEFAULT )

#project global flags

PROJECT(libprox CXX)

IF(NOT TOP_LEVEL)
  SET(TOP_LEVEL ${CMAKE_CURRENT_SOURCE_DIR}/..)
ENDIF()
GET_FILENAME_COMPONENT(TOP_LEVEL ${TOP_LEVEL} ABSOLUTE)

#add-ins

IF(NOT CMAKE_MODULE_PATH)
  SET(CMAKE_MODULE_PATH ${TOP_LEVEL}/build/modules)
ENDIF()
GET_FILENAME_COMPONENT(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ABSOLUTE)

MACRO(VERIFY_VERSION package_name major minor subminor)
  IF (${package_name}_MAJOR_VERSION LESS ${major})
    MESSAGE(FATAL_ERROR "${package_name} out of date: ${major}.${minor}.${subminor}")
  ELSEIF(${package_name}_MAJOR_VERSION EQUAL ${major})
    IF(${package_name}_MINOR_VERSION LESS ${minor})
       MESSAGE(FATAL_ERROR "${package_name} out of date: ${major}.${minor}.${subminor}")
    ELSEIF(${package_name}_MINOR_VERSION EQUAL ${major})
      IF(${package_name}_SUBMINOR_VERSION LESS ${subminor})
        MESSAGE(FATAL_ERROR "${package_name} out of date: ${major}.${minor}.${subminor}")
      ENDIF()
    ENDIF()
  ENDIF()
ENDMACRO(VERIFY_VERSION)


#libraries

#dependency: boost > 1.35
IF(NOT BOOST_ROOT)
  IF(WIN32)
    SET(BOOST_ROOT ${PLATFORM_LIBS})
  ENDIF()
  IF(APPLE)
    SET(BOOST_ROOT ${PLATFORM_LIBS})
  ENDIF()
ENDIF()
SET(Boost_FIND_REQUIRED TRUE)
IF(WIN32)
  FIND_PACKAGE(Boost COMPONENTS thread date_time)
ELSE()
  FIND_PACKAGE(Boost COMPONENTS system)
  STRING(REPLACE "boost_system" "boost_thread" Boost_THREAD_LIBRARY ${Boost_SYSTEM_LIBRARY})
  STRING(REPLACE "boost_system" "boost_date_time" Boost_DATE_TIME_LIBRARY ${Boost_SYSTEM_LIBRARY})
ENDIF()
VERIFY_VERSION(Boost 1 35 0)

#dependency: glut
FIND_PACKAGE(GLUT)
IF(NOT GLUT_FOUND)
  MESSAGE(SEND_ERROR "Couldn't find GLUT.")
ENDIF(NOT GLUT_FOUND)

#project locations
SET(LIBPROX_ROOT ${TOP_LEVEL}/libprox)
SET(PROXSIM_ROOT ${TOP_LEVEL}/proxsim)

#include/source file location
SET(LIBPROX_INCLUDE_DIR ${LIBPROX_ROOT}/include)
SET(LIBPROX_SOURCE_DIR ${LIBPROX_ROOT}/src)

SET(PROXSIM_SOURCE_DIR ${PROXSIM_ROOT}/src)


#cxx flags
INCLUDE_DIRECTORIES(BEFORE ${LIBPROX_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${GLUT_INCLUDE_DIR})

#source files
SET(LIBPROX_SOURCES
  ${LIBPROX_SOURCE_DIR}/ArcAngle.cpp
  ${LIBPROX_SOURCE_DIR}/BruteForceQueryHandler.cpp
  ${LIBPROX_SOURCE_DIR}/Duration.cpp
  ${LIBPROX_SOURCE_DIR}/Object.cpp
  ${LIBPROX_SOURCE_DIR}/Quaternion.cpp
  ${LIBPROX_SOURCE_DIR}/Query.cpp
  ${LIBPROX_SOURCE_DIR}/QueryCache.cpp
  ${LIBPROX_SOURCE_DIR}/RTreeQueryHandler.cpp
  ${LIBPROX_SOURCE_DIR}/SolidAngle.cpp
  ${LIBPROX_SOURCE_DIR}/Time.cpp
)


SET(PROXSIM_SOURCES
  ${PROXSIM_SOURCE_DIR}/GLRenderer.cpp
  ${PROXSIM_SOURCE_DIR}/Simulator.cpp
  ${PROXSIM_SOURCE_DIR}/Timer.cpp
  ${PROXSIM_SOURCE_DIR}/main.cpp
)

#link flags
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

#libraries
ADD_LIBRARY(prox STATIC ${LIBPROX_SOURCES})
TARGET_LINK_LIBRARIES(prox ${LIBPROX_LIBRARIES} ${Boost_THREAD_LIBRARY})


#binaries
ADD_EXECUTABLE(proxsim ${PROXSIM_SOURCES})
TARGET_LINK_LIBRARIES(proxsim prox ${Boost_THREAD_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${GLUT_LIBRARIES})

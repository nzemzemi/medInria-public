################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2014. All rights reserved.
# See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

cmake_minimum_required(VERSION 3.0)

if(NOT DEFINED ${medInria_VERSION})
    set(medInria_VERSION 2.1.0)
endif()

project(MEDINRIA VERSION ${medInria_VERSION})


## #############################################################################
## Set version
## #############################################################################

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UP)
add_definitions(-D${PROJECT_NAME_UP}_VERSION="${${PROJECT_NAME}_VERSION}")

## #############################################################################
## Macro
## #############################################################################

add_definitions( -DPROJECT_NAME="MUSIC" )
add_definitions( -DPROJECT_CONTACT="music-userfeedback@inria.fr" )

## #############################################################################
## Options
## #############################################################################

option(${PROJECT_NAME}_BUILD_TESTS
  "Build tests"
  OFF
  )
if(${PROJECT_NAME}_BUILD_TESTS)
    enable_testing()
endif()

option(${PROJECT_NAME}_BUILD_DOCUMENTATION
  "Build documentation"
  OFF
  )


## #############################################################################
## Additionals modules
## #############################################################################

set(${PROJECT_NAME}_CMAKE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/module
  )

set(CMAKE_MODULE_PATH
  ${${PROJECT_NAME}_CMAKE_DIR}
  ${CMAKE_MODULE_PATH}
  )

include(list_source_files)
include(list_header_directories_to_include)
include(set_exe_install_rules)
include(set_lib_install_rules)
include(set_plugin_install_rules)
include(add_plugins)
if(APPLE)
  include(FixDCMTKMacLink)
  include(FixDCMTKMacInstall)
  set(CMAKE_MACOSX_RPATH OFF)
endif()

## #############################################################################
## Find package
## #############################################################################

set(QT_USE_QTOPENGL on)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})


find_package(dtk REQUIRED)
include(${dtk_USE_FILE})

if (WIN32)
    find_package(Boost REQUIRED)
    include_directories(${${PROJECT_NAME}_INCLUDE_DIRS}
${Boost_INCLUDE_DIR})
endif()


## #############################################################################
## Qt Automoc
## #############################################################################

set(CMAKE_AUTOMOC ON)
#   Find includes in corresponding build (and source) directories
#   (nedded for generated files)
set(CMAKE_INCLUDE_CURRENT_DIR ON)


## #############################################################################
## add definitions
## #############################################################################

if (CMAKE_CXX_COMPILER MATCHES clang)
    add_definitions(-DCLANG)
endif()

if (NOT MSVC) #TODO :why not this warings ?
  add_definitions(-Wno-write-strings)
  add_definitions(-Wformat=0)
endif()

add_definitions(${QT_DEFINITIONS}) #TODO :wath was it for ? (it is empty)
add_definitions(-DQT_SHARED)

if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  message(STATUS "Add -DQT_NO_DEBUG")
  add_definitions(-DQT_NO_DEBUG)
endif()


## #############################################################################
## Windows specificity
## #############################################################################

if (WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()

## #############################################################################
## Add subdirectory
## #############################################################################

add_subdirectory(src)
add_subdirectory(app)
add_subdirectory(src-plugins)

if(${PROJECT_NAME}_BUILD_DOCUMENTATION)
  add_subdirectory(doc)
endif()


################################################################################
# Setup Configuration files
################################################################################

if(EXISTS ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in)
  configure_file( ## Build tree configure file
    ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
    ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    @ONLY IMMEDIATE
    )
endif()

################################################################################
# Setup use file
################################################################################

if(EXISTS ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Use.cmake.in)
  configure_file( ## Common use file
    ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}Use.cmake.in
    ${${PROJECT_NAME}_BINARY_DIR}/${PROJECT_NAME}Use.cmake
    @ONLY IMMEDIATE
    )
endif()



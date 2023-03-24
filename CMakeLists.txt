cmake_minimum_required(VERSION 3.1.0)

project(flyhigh)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)

set (BUILD_WITH_QT6 FALSE CACHE BOOL "Enable (experimental) Qt6 support")
if (BUILD_WITH_QT6)
  set(QT_MIN_VERSION 6.4.0)
  set(QT_VERSION_BASE "Qt6")
else()
  # at least Qt 5.15 is required.
  set(QT_MIN_VERSION 5.15.0)
  set(QT_VERSION_BASE "Qt5")
endif()

find_package(${QT_VERSION_BASE} ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Gui Network
	PrintSupport SerialPort Sql WebEngineWidgets Widgets Xml)

# default build type is Release
if(NOT CMAKE_BUILD_TYPE)
 set(CMAKE_BUILD_TYPE "Release")
endif()

message("Build type is: " ${CMAKE_BUILD_TYPE})

# verbose makefiles if doing a Debug build
# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#  set(CMAKE_VERBOSE_MAKEFILE TRUE)
# else()
#  set(CMAKE_VERBOSE_MAKEFILE FALSE)
# endif()

# set file locations for installation
# where to install binaries
if(NOT FLYHIGH_BIN_DIR)
 set(FLYHIGH_BIN_DIR "${CMAKE_INSTALL_PREFIX}/usr/bin")
endif()
# where to install desktop link
if(NOT FLYHIGH_APPLNK_DIR)
 set(FLYHIGH_APPLNK_DIR "${CMAKE_INSTALL_PREFIX}/usr/share/applications")
endif()
# where to install icons
if(NOT FLYHIGH_ICON_DIR)
 set(FLYHIGH_ICON_DIR "${CMAKE_INSTALL_PREFIX}/usr/share/pixmaps")
endif()
# where to install documentation
if(NOT FLYHIGH_DOC_DIR)
 set(FLYHIGH_DOC_DIR "${CMAKE_INSTALL_PREFIX}/usr/share/doc/flyhigh")
endif()

add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)
add_definitions(-Wall -fPIC -msse2)

include(Functions.cmake) # custom functions
include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(src)
add_subdirectory(tools)

# install files
install(FILES AUTHORS DESTINATION ${FLYHIGH_DOC_DIR})
install(FILES COPYING DESTINATION ${FLYHIGH_DOC_DIR})
install(FILES INSTALL DESTINATION ${FLYHIGH_DOC_DIR})
install(FILES NEWS DESTINATION ${FLYHIGH_DOC_DIR})
install(FILES README DESTINATION ${FLYHIGH_DOC_DIR})
install(FILES TODO DESTINATION ${FLYHIGH_DOC_DIR})

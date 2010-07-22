include(${QT_USE_FILE})

include_directories( ${QT_INCLUDES} )

set(images_SRCS ImgCollection.cpp)

add_library(images ${images_SRCS})

########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = $(all_includes)
#METASOURCES = AUTO
#lib_LIBRARIES =  libimages.a
#
#
#libimages_a_SOURCES = Images.cpp
#noinst_HEADERS =  Images.h

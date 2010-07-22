include(${QT_USE_FILE})

include_directories(${CMAKE_SOURCE_DIR}/src/common/container
                    ${CMAKE_SOURCE_DIR}/src/common/utils
                    ${CMAKE_SOURCE_DIR}/src/database
                    ${QT_INCLUDES} )

set(dbflytec_SRCS flytec_al.c
                  flytec_ll.c
                  flytec_pl_rs232.c
                  FlytecDevice.cpp)

set(dbflytec_HDRS flytec_al.h
                  flytec_ll.h
                  flytec_pl.h
                  FlytecDevice.h )

QT4_WRAP_CPP(dbflytec_SRCS ${dbflytec_HDRS})

add_library(dbflytec ${dbflytec_SRCS} ${dbflytec_HDRS})

########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/src/common/container \
#	-I$(top_srcdir)/src/common/utils -I$(top_srcdir)/src/database $(all_includes)
#
#lib_LIBRARIES =  libflytec.a
#libflytec_a_SOURCES = flytec_al.c flytec_ll.c flytec_pl_rs232.c FlytecDevice.cpp
#noinst_HEADERS = flytec_al.h flytec_ll.h flytec_pl.h FlytecDevice.h

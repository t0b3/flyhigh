include(${QT_USE_FILE})

include_directories(${CMAKE_SOURCE_DIR}/src/common/container
                    ${CMAKE_SOURCE_DIR}/src/common/utils
                    ${CMAKE_SOURCE_DIR}/src/database
                    ${QT_INCLUDES} )

set(dbgarmin_SRCS GarminDevice.cpp
                  gpsapp.c
                  gpscom.c
                  gpsinput.c
                  gpsmath.c
                  gpsmem.c
                  gpsprot.c
                  gpsread.c
                  gpsrqst.c
                  gpssend.c
                  gpsserial.c
                  gpsutil.c
                  GarminFmt.cpp)

set(dbgarmin_HDRS GarminDevice.h
                  gpsapp.h
                  gpscom.h
                  gpsdatum.h
                  gps.h
                  GarminFmt.h )

QT4_WRAP_CPP(dbgarmin_SRCS ${dbgarmin_HDRS})

add_library(dbgarmin ${dbgarmin_SRCS} ${dbgarmin_HDRS})

########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/src/common/container \
#	-I$(top_srcdir)/src/common/utils -I$(top_srcdir)/src/database $(all_includes)
#METASOURCES = AUTO
#lib_LIBRARIES = libgarmin.a
#
#libgarmin_a_SOURCES = GarminDevice.cpp gpsapp.c gpscom.c gpsinput.c gpsmath.c \
#	gpsmem.c gpsprot.c gpsread.c gpsrqst.c gpssend.c gpsserial.c gpsutil.c \
#	GarminFmt.cpp
#
#noinst_HEADERS = GarminDevice.h gpsapp.h gpscom.h gpsdatum.h gps.h GarminFmt.h

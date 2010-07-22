include(${QT_USE_FILE})

include_directories( ${QT_INCLUDES} )

set(utils_SRCS Error.cpp
               GnuPlot.cpp
               Tokenizer.cpp )

               
set(utils_HDRS Error.h
               GnuPlot.h
               Tokenizer.h )

QT4_WRAP_CPP(utils_SRCS ${utils_HDRS})

add_library(utils ${utils_SRCS} ${utils_HDRS})

########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = $(all_includes)
#METASOURCES = AUTO
#lib_LIBRARIES =  libutils.a
#libutils_a_SOURCES = Error.cpp GnuPlot.cpp Tokenizer.cpp
#noinst_HEADERS = Error.h GnuPlot.h Tokenizer.h

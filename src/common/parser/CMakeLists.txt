include_directories(${CMAKE_SOURCE_DIR}/src/common/container)

set(parser_SRCS IGCFileParser.cpp
                OpenAirFileParser.cpp
                WptFileParser.cpp)

add_library(parser ${parser_SRCS})

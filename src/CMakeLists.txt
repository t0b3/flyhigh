include_directories(${CMAKE_SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}/src/analysis
                    ${CMAKE_SOURCE_DIR}/src/common
                    ${CMAKE_SOURCE_DIR}/src/common/container
                    ${CMAKE_SOURCE_DIR}/src/common/elevation
                    ${CMAKE_SOURCE_DIR}/src/common/form
                    ${CMAKE_SOURCE_DIR}/src/common/utils
                    ${CMAKE_SOURCE_DIR}/src/common/webmap
                    ${CMAKE_SOURCE_DIR}/src/common/window
                    ${CMAKE_SOURCE_DIR}/src/database
                    ${CMAKE_SOURCE_DIR}/src/preparation
                    ${CMAKE_SOURCE_DIR}/src/settings
                    ${CMAKE_SOURCE_DIR}/src/settings/flyhigh
                    ${CMAKE_SOURCE_DIR}/src/settings/flytec
                    ${CMAKE_SOURCE_DIR}/src/settings/flytec6015
                    ${CMAKE_SOURCE_DIR}/src/tools/dbmigrator
                    ${CMAKE_SOURCE_DIR}/src/upload
                    ${Qt5_INCLUDE_DIRS}
                    ${Qt5Core_INCLUDE_DIRS}
                    ${Qt5Gui_INCLUDE_DIRS}
                    ${Qt5Network_INCLUDE_DIRS}
                    ${Qt5PrintSupport_INCLUDE_DIRS}
                    ${Qt5Sql_INCLUDE_DIRS}
                    ${Qt5SerialPort_INCLUDE_DIRS}
                    ${Qt5WebEngine_INCLUDE_DIRS}
                    ${Qt5WebEngineWidgets_INCLUDE_DIRS}
                    ${Qt5Widgets_INCLUDE_DIRS}
                    ${Qt5Xml_INCLUDE_DIRS})

add_subdirectory(analysis)
add_subdirectory(common)
add_subdirectory(database)
add_subdirectory(preparation)
add_subdirectory(res)
add_subdirectory(settings)
add_subdirectory(tools)
add_subdirectory(upload)

set(flyhigh_SRCS main.cpp
                 MainWindow.cpp)

if(WIN32)
 # no console window for windoze builds
 add_executable(flyhigh WIN32 ${flyhigh_SRCS})
else(WIN32)
 add_executable(flyhigh ${flyhigh_SRCS})
endif(WIN32)

target_link_libraries(flyhigh
  analysis
  preparation
  cfgflyhigh
  cfgflytec
  cfgflytec6015
  dbmigrator
  window
  database
  dbflytec
  dbgarmin
  dbsql
  container
  elevation
  form
  map
  webmap
  parser
  utils
  res
  qextmedia
  upload
  Qt5Core Qt5Gui Qt5Network Qt5PrintSupport Qt5Sql Qt5SerialPort Qt5WebEngine
  Qt5WebEngineWidgets Qt5Widgets Qt5Xml)

# install executable
install(TARGETS flyhigh DESTINATION ${FLYHIGH_BIN_DIR} )

# desktop file
install(FILES flyhigh.desktop DESTINATION ${FLYHIGH_APPLNK_DIR})


cmake_minimum_required (VERSION 2.6)

set (PROJECT syntaxanalyzer)
project (${PROJECT})
add_definitions (-g -Wall -pedantic -std=c++0x)

set (SOURCES_GENHASHTABLE
        genhashtable.cpp)
set (SOURCES_MAIN
        model.cpp model.h main.cpp)

find_package (Qt4 REQUIRED)
set(QT_DONT_USE_QTGUI false)
include (${QT_USE_FILE})

add_executable (genhashtable ${SOURCES_GENHASHTABLE})
target_link_libraries (genhashtable ${QT_LIBRARIES})

add_executable (main ${SOURCES_MAIN})
target_link_libraries (main ${QT_LIBRARIES})

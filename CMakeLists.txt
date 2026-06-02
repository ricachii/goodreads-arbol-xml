cmake_minimum_required(VERSION 3.16)
project(Tarea2_XML CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# PugiXML — incluir la libreria directamente en el proyecto
set(PUGI_SRC libs/pugixml/pugixml.cpp)

add_executable(tarea2
    src/main.cpp
    src/XMLParser.cpp
    src/GeneralTree.cpp
    ${PUGI_SRC}
)

target_include_directories(tarea2 PRIVATE src libs/pugixml)

# Tests
add_executable(tests
    tests/test_main.cpp
    src/XMLParser.cpp
    src/GeneralTree.cpp
    ${PUGI_SRC}
)
target_include_directories(tests PRIVATE src libs/pugixml)

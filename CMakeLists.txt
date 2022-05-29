cmake_minimum_required(VERSION 3.15)
project(webserver)

find_package(BISON)
find_package(FLEX)

include_directories(${PROJECT_SOURCE_DIR}/src/parser)

BISON_TARGET(HttpParser 
${PROJECT_SOURCE_DIR}/src/parser/parser.y 
${PROJECT_SOURCE_DIR}/src/parser/y.tab.cpp)

FLEX_TARGET(HttpScanner 
${PROJECT_SOURCE_DIR}/src/parser/parser.l 
${PROJECT_SOURCE_DIR}/src/parser/lex.tab.cpp)

ADD_FLEX_BISON_DEPENDENCY(HttpScanner HttpParser)

add_executable(main
${PROJECT_SOURCE_DIR}/src/parser/example.cc
${BISON_HttpParser_OUTPUTS}
${FLEX_HttpScanner_OUTPUTS}
)
target_link_libraries(main ${FLEX_LIBRARIES})
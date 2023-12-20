cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModule)

project(Automaton)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
SET(CMAKE_CXX_FLAGS_DEBUG "-std=c++23 -Og -Wall -Werror -fsanitize=address,leak -Wl,-rpath-link,/usr/bin/x86_64-linux-gnu/libFLAC.so.8")
SET(CMAKE_CXX_FLAGS_RELEASE "-std=c++23 -O3 -Wall")

add_subdirectory(Finite/Automaton)
add_subdirectory(Finite/Expression)
add_subdirectory(Pushdown/Grammar)
add_subdirectory(Pushdown/Parser)

if(DEFINED TEST_EARLEY OR DEFINED TEST_LR1 OR DEFINED TEST_COVERAGE)
  add_subdirectory(Test)
endif()

add_executable(Automaton Algo.cpp)
target_include_directories(Automaton
  PRIVATE ${CMAKE_SOURCE_DIR}/Finite/Automaton
  PRIVATE ${CMAKE_SOURCE_DIR}/Finite/Expression
  PRIVATE ${CMAKE_SOURCE_DIR}/Pushdown/Grammar
  PRIVATE ${CMAKE_SOURCE_DIR}/Pushdown/Parser
)
target_link_libraries(Automaton PRIVATE FiniteAutomaton FiniteExpression PushdownGrammar PushdownParser)
install(TARGETS Automaton DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

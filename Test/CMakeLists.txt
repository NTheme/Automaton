
if(DEFINED TEST_EARLEY)
  add_executable(TestEarley TestEarley.cpp)
  target_include_directories(TestEarley
    PRIVATE ${CMAKE_SOURCE_DIR}/Finite/Automaton
    PRIVATE ${CMAKE_SOURCE_DIR}/Finite/Expression
    PRIVATE ${CMAKE_SOURCE_DIR}/Pushdown/Grammar
    PRIVATE ${CMAKE_SOURCE_DIR}/Pushdown/Parser
  )
  target_link_libraries(TestEarley PRIVATE FiniteAutomaton FiniteExpression PushdownGrammar PushdownParser)
endif()

if(DEFINED TEST_LR1)
  add_executable(TestLR1 TestLR1.cpp)
  target_include_directories(TestLR1
    PRIVATE ${CMAKE_SOURCE_DIR}/Finite/Automaton
    PRIVATE ${CMAKE_SOURCE_DIR}/Finite/Expression
    PRIVATE ${CMAKE_SOURCE_DIR}/Pushdown/Grammar
    PRIVATE ${CMAKE_SOURCE_DIR}/Pushdown/Parser
  )
  target_link_libraries(TestLR1 PRIVATE FiniteAutomaton FiniteExpression PushdownGrammar PushdownParser)
endif()

if(DEFINED TEST_COVERAGE)
  set(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage --coverage")
  set(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage --coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
  
  find_package(GTest REQUIRED)
  include(CodeCoverage)
  
  add_executable(TestCoverage TestCoverage.cpp)
  target_include_directories(TestCoverage
    PRIVATE ${CMAKE_SOURCE_DIR}/Finite/Automaton
    PRIVATE ${CMAKE_SOURCE_DIR}/Finite/Expression
    PRIVATE ${CMAKE_SOURCE_DIR}/Pushdown/Grammar
    PRIVATE ${CMAKE_SOURCE_DIR}/Pushdown/Parser
  )
  target_link_libraries(TestCoverage PRIVATE FiniteAutomaton FiniteExpression PushdownGrammar PushdownParser GTest::gtest GTest::gtest_main)
  setup_target_for_coverage(test_coverage TestCoverage coverage)
endif()

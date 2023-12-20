set(SOURCES
    Sources/Expression.cpp
)

add_library(FiniteExpression STATIC ${SOURCES})

target_include_directories(FiniteExpression
    PRIVATE ${CMAKE_SOURCE_DIR}/Finite/Automaton
    PRIVATE ${CMAKE_SOURCE_DIR}/Finite/Expression
)

target_link_libraries(FiniteExpression PRIVATE FiniteAutomaton)

set(SOURCES
    Sources/DFA.cpp
    Sources/CDFA.cpp
    Sources/Automaton.cpp
    Sources/NFA.cpp
)

add_library(FiniteAutomaton STATIC ${SOURCES})

target_include_directories(FiniteAutomaton
    PRIVATE ${CMAKE_SOURCE_DIR}/Finite/Automaton
    PRIVATE ${CMAKE_SOURCE_DIR}/Finite/Expression
)

target_link_libraries(FiniteAutomaton PRIVATE FiniteExpression)

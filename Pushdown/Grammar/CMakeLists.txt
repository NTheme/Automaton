set(SOURCES
    Sources/Grammar.cpp
    Sources/Rule.cpp
)

add_library(PushdownGrammar ${SOURCES})

target_include_directories(PushdownGrammar
    PRIVATE ${CMAKE_SOURCE_DIR}/Pushdown/Grammar
)

set(SOURCES
    Sources/ParserEarley.cpp
    Sources/ParserLR1.cpp
)

add_library(PushdownParser ${SOURCES})

target_include_directories(PushdownParser
    PRIVATE ${CMAKE_SOURCE_DIR}/Pushdown/Grammar
    PRIVATE ${CMAKE_SOURCE_DIR}/Pushdown/Parser
)

target_link_libraries(PushdownParser PRIVATE PushdownGrammar)

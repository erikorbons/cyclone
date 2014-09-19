enable_testing ()

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable (TestParser TestParser.cc TestLexer.cc)

target_link_libraries(TestParser ${LIBS} CycloneCore CycloneParser ${Boost_LIBRARIES})

add_test (TestParser ${RUNTIME_OUTPUT_DIRECTORY}/TestParser)
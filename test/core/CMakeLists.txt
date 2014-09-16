enable_testing ()

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_executable (TestCore TestCore.cc TestTextBuffer.cc)

target_link_libraries(TestCore ${LIBS} CycloneCore ${Boost_LIBRARIES})

add_test (TestCore ${RUNTIME_OUTPUT_DIRECTORY}/TestCore)
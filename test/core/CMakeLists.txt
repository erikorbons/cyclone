enable_testing ()

add_executable (TestCore TestCore.cc)

target_link_libraries(TestCore ${LIBS})

add_test (TestCore ${EXECUTABLE_OUTPUT_PATH}/TestCore)
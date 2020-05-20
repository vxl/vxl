# acal/io/tests/CMakeLists.txt

set(acal_io_test_sources
    test_driver.cxx
    test_generic_io.h

    test_match_graph_io.cxx
    test_match_utils_io.cxx
)

add_executable(acal_io_test_all ${acal_io_test_sources})
target_link_libraries(acal_io_test_all ${VXL_LIB_PREFIX}acal_io ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}testlib )

add_test( NAME acal_test_match_graph_io COMMAND $<TARGET_FILE:acal_io_test_all> test_match_graph_io )
add_test( NAME acal_test_match_utils_io COMMAND $<TARGET_FILE:acal_io_test_all> test_match_utils_io )

add_executable( acal_io_test_include test_include.cxx )
target_link_libraries( acal_io_test_include ${VXL_LIB_PREFIX}acal_io )

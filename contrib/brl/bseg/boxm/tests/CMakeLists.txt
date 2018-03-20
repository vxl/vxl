add_executable( boxm_test_all
  test_driver.cxx
  test_binary_io.cxx
  test_block_iter.cxx
# FAILS on DASHBOARD FIX_ME
#  test_region_finder.cxx
  test_cell_iterator.cxx
  test_load_neighboring_blocks.cxx
 )

set(boxm_test_all_sources
    test_utils.h    test_utils.cxx
    )

vxl_add_library(LIBRARY_NAME boxm_tests LIBRARY_SOURCES  ${boxm_test_all_sources})
target_link_libraries(boxm_tests boxm boct ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul)

target_link_libraries( boxm_test_all boxm_tests boxm boct ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_xio ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/camera0.txt ${CMAKE_CURRENT_BINARY_DIR}/camera0.txt COPYONLY )

add_test( NAME boxm_test_binary_io COMMAND $<TARGET_FILE:boxm_test_all>    test_binary_io  )
add_test( NAME boxm_test_block_iter COMMAND $<TARGET_FILE:boxm_test_all>    test_block_iter  )
#add_test( NAME boxm_test_region_finder COMMAND $<TARGET_FILE:boxm_test_all>    test_region_finder )
if( VXL_RUN_FAILING_TESTS ) ## SEGFAULT when testing with ctest -j16 (perhaps files are shared between tests)
add_test( NAME boxm_test_cell_iterator COMMAND $<TARGET_FILE:boxm_test_all>    test_cell_iterator )
endif()
add_test( NAME boxm_test_load_neighboring_blocks COMMAND $<TARGET_FILE:boxm_test_all>    test_load_neighboring_blocks )

add_executable( boxm_test_include test_include.cxx )
target_link_libraries( boxm_test_include boxm)
add_executable( boxm_test_template_include test_template_include.cxx )
target_link_libraries( boxm_test_template_include boxm)

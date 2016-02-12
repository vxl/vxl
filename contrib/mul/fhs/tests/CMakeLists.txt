# This is mul/fhs/tests/CMakeLists.txt

add_executable( fhs_test_all
  test_driver.cxx

  test_arc.cxx
  test_searcher.cxx
)
target_link_libraries( fhs_test_all fhs ${VXL_LIB_PREFIX}testlib )

add_test( NAME fhs_test_arc COMMAND $<TARGET_FILE:fhs_test_all> test_arc )
add_test( NAME fhs_test_searcher COMMAND $<TARGET_FILE:fhs_test_all> test_searcher )

add_executable( fhs_test_include test_include.cxx )
target_link_libraries( fhs_test_include fhs )

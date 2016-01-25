# This is core/vnl/xio/tests/CMakeLists.txt

add_executable( vnl_xio_test_all
  test_driver.cxx
  test_vnl_xio.cxx
)

target_link_libraries(vnl_xio_test_all vnl_xio testlib)

add_test( NAME vnl_test_xio COMMAND $<TARGET_FILE:vnl_xio_test_all> test_vnl_xio )

add_executable(vnl_xio_test_include test_include.cxx)
target_link_libraries(vnl_xio_test_include vnl_xio)
add_executable(vnl_xio_test_template_include test_template_include.cxx)
target_link_libraries(vnl_xio_test_template_include vnl_xio)

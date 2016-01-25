# This is core/vgl/xio/tests/CMakeLists.txt

add_executable( vgl_xio_test_all
  test_driver.cxx
  test_vgl_xio.cxx
)

target_link_libraries(vgl_xio_test_all vgl_xio testlib)

add_test( NAME vgl_test_xio COMMAND $<TARGET_FILE:vgl_xio_test_all> test_vgl_xio )

add_executable(vgl_xio_test_include test_include.cxx)
target_link_libraries(vgl_xio_test_include vgl_xio)
add_executable(vgl_xio_test_template_include test_template_include.cxx)
target_link_libraries(vgl_xio_test_template_include vgl_xio)

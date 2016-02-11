add_executable( vdgl_test_all
  test_driver.cxx
  test_vdgl.cxx
  test1_vdgl.cxx
  test_vdgl_io.cxx
)
target_link_libraries( vdgl_test_all vsol vdgl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}testlib )

add_test( NAME vdgl_test_vdgl COMMAND $<TARGET_FILE:vdgl_test_all> test_vdgl)
add_test( NAME vdgl_test1_vdgl COMMAND $<TARGET_FILE:vdgl_test_all> test1_vdgl)
add_test( NAME vdgl_test_io COMMAND $<TARGET_FILE:vdgl_test_all> test_vdgl_io)

add_executable( vdgl_test_include test_include.cxx )

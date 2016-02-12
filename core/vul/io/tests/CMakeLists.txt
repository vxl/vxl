# This is core/vul/io/tests/CMakeLists.txt

add_executable( vul_io_test_all
  test_driver.cxx

  test_user_info_io.cxx
)
target_link_libraries( vul_io_test_all ${VXL_LIB_PREFIX}vul_io ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}testlib ${VXL_LIB_PREFIX}vpl )

add_test( NAME vul_io_test_user_info_io COMMAND $<TARGET_FILE:vul_io_test_all> test_user_info_io )

add_executable( vul_io_test_include test_include.cxx )
target_link_libraries( vul_io_test_include ${VXL_LIB_PREFIX}vul_io )

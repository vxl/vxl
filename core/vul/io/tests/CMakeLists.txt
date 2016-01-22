# This is core/vul/io/tests/CMakeLists.txt

ADD_EXECUTABLE( vul_io_test_all
  test_driver.cxx

  test_user_info_io.cxx
)
TARGET_LINK_LIBRARIES( vul_io_test_all  vul_io vul vsl testlib vpl )

ADD_TEST( NAME vul_io_test_user_info_io COMMAND $<TARGET_FILE:vul_io_test_all> test_user_info_io )

ADD_EXECUTABLE( vul_io_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vul_io_test_include vul_io )

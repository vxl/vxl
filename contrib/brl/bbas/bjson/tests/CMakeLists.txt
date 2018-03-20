# bbas/bjson/tests/CMakeLists.txt

add_executable( bjson_test_all
  test_driver.cxx
  test_bjson.cxx
)

target_link_libraries( bjson_test_all bjson testlib)

add_test( NAME bjson_test_bjson COMMAND $<TARGET_FILE:bjson_test_all> test_bjson)


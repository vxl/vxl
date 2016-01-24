#add_executable( brad_io_test_all
#  test_driver.cxx
#)
#target_link_libraries( brad_io_test_all brad_io brad vnl vpl vbl_io vsl vbl vgl testlib )

add_executable( brad_io_test_include test_include.cxx )
target_link_libraries(brad_io_test_include brad_io )

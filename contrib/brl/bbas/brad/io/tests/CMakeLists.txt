#ADD_EXECUTABLE( brad_io_test_all
#  test_driver.cxx
#)
#TARGET_LINK_LIBRARIES( brad_io_test_all brad_io brad vnl vpl vbl_io vsl vbl vgl testlib )

ADD_EXECUTABLE( brad_io_test_include test_include.cxx )
TARGET_LINK_LIBRARIES(brad_io_test_include brad_io )

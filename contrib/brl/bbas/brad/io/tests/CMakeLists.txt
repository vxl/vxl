#add_executable( brad_io_test_all
#  test_driver.cxx
#)
#target_link_libraries( brad_io_test_all brad_io brad ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}testlib )

add_executable( brad_io_test_include test_include.cxx )
target_link_libraries(brad_io_test_include brad_io )

add_executable( baio_test_all
                test_driver.cxx
                test_read.cxx
                test_write.cxx
              )

target_link_libraries( baio_test_all baio ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}testlib)

add_test( NAME baio_test_read COMMAND $<TARGET_FILE:baio_test_all> test_read )
add_test( NAME baio_test_write COMMAND $<TARGET_FILE:baio_test_all> test_write )

add_executable( baio_test_include test_include.cxx )
target_link_libraries( baio_test_include baio)

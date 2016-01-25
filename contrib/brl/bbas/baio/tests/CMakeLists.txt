ADD_EXECUTABLE( baio_test_all
                test_driver.cxx
                test_read.cxx
                test_write.cxx
              )

TARGET_LINK_LIBRARIES( baio_test_all baio vnl vpl testlib)

ADD_TEST( NAME baio_test_read COMMAND $<TARGET_FILE:baio_test_all> test_read )
ADD_TEST( NAME baio_test_write COMMAND $<TARGET_FILE:baio_test_all> test_write )

ADD_EXECUTABLE( baio_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( baio_test_include baio)

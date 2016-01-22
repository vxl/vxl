ADD_EXECUTABLE( baio_test_all
                test_driver.cxx
                test_read.cxx
                test_write.cxx
              )

TARGET_LINK_LIBRARIES( baio_test_all baio vnl vpl testlib)

ADD_TEST( baio_test_read ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/baio_test_all test_read )
ADD_TEST( baio_test_write ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/baio_test_all test_write )

ADD_EXECUTABLE( baio_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( baio_test_include baio)

ADD_EXECUTABLE( baio_test_all
                test_driver.cxx
                test_read.cxx
              )

TARGET_LINK_LIBRARIES( baio_test_all baio testlib)

ADD_TEST( baio_test_read ${EXECUTABLE_OUTPUT_PATH}/baio_test_read  )

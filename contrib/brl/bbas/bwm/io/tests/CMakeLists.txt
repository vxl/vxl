ADD_EXECUTABLE( bwm_io_test_all
                test_driver.cxx
		test_kml_parser.cxx
              )
TARGET_LINK_LIBRARIES(bwm_io_test_all bwm_io testlib)

ADD_EXECUTABLE( bwm_io_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bwm_io_test_include bwm_io)

ADD_TEST(bwm_io_test_kml_parser ${EXECUTABLE_OUTPUT_PATH}/bwm_io_test_all test_kml_parser ${CMAKE_CURRENT_SOURCE_DIR} )

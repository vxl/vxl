add_executable( bwm_io_test_all
                test_driver.cxx
                test_kml_parser.cxx
              )
target_link_libraries(bwm_io_test_all bwm_io ${VXL_LIB_PREFIX}testlib)

add_executable( bwm_io_test_include test_include.cxx )
target_link_libraries( bwm_io_test_include bwm_io)

add_test( NAME bwm_io_test_kml_parser COMMAND $<TARGET_FILE:bwm_io_test_all> test_kml_parser ${CMAKE_CURRENT_SOURCE_DIR} )

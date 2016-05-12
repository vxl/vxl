# basic/bxml/tests/CMakeLists.txt
# Tests for bxml library
if( EXPAT_FOUND )
 if(EXPATPP_FOUND)
  add_executable( bxml_test_all
   test_driver.cxx
   test_io.cxx
   test_find.cxx
  )

  target_link_libraries( bxml_test_all bxml ${VXL_LIB_PREFIX}testlib expat expatpp)

  add_test( NAME bxml_test_io COMMAND $<TARGET_FILE:bxml_test_all>
                                 test_io ${CMAKE_CURRENT_SOURCE_DIR}/ )
  add_test( NAME bxml_test_find COMMAND $<TARGET_FILE:bxml_test_all>
                                 test_find ${CMAKE_CURRENT_SOURCE_DIR}/ )
 endif()
endif()
add_executable( bxml_test_include test_include.cxx )
target_link_libraries( bxml_test_include bxml )

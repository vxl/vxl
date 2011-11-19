INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bmdl )
IF( EXPAT_FOUND )
 IF(EXPATPP_FOUND)

 ADD_EXECUTABLE( bmdl_pro_test_all
  test_driver.cxx
  test_bmdl_classify_process.cxx
  test_bmdl_trace_boundaries_process.cxx
  test_bmdl_generate_mesh_process.cxx
 )

TARGET_LINK_LIBRARIES( bmdl_pro_test_all testlib bmdl_pro bmdl vil_pro bprb brdb sdet vbl vil vil_io vcl expat expatpp)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/first_ret.tif       ${CMAKE_CURRENT_BINARY_DIR}/first_ret.tif       COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/last_ret.tif        ${CMAKE_CURRENT_BINARY_DIR}/last_ret.tif        COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/ground.tif          ${CMAKE_CURRENT_BINARY_DIR}/ground.tif          COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/height.tif          ${CMAKE_CURRENT_BINARY_DIR}/height.tif          COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/label.tif           ${CMAKE_CURRENT_BINARY_DIR}/label.tif           COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/classify_params.xml ${CMAKE_CURRENT_BINARY_DIR}/classify_params.xml COPYONLY )

IF(SEGFAULT_FIXED)
ADD_TEST( bmdl_pro_classify_process         ${EXECUTABLE_OUTPUT_PATH}/bmdl_pro_test_all test_bmdl_classify_process )
ENDIF(SEGFAULT_FIXED)
ADD_TEST( bmdl_pro_trace_boundaries_process ${EXECUTABLE_OUTPUT_PATH}/bmdl_pro_test_all test_bmdl_trace_boundaries_process ${CMAKE_CURRENT_SOURCE_DIR} )
ADD_TEST( bmdl_pro_generate_mesh_process    ${EXECUTABLE_OUTPUT_PATH}/bmdl_pro_test_all test_bmdl_generate_mesh_process    ${CMAKE_CURRENT_SOURCE_DIR} )
 ENDIF( EXPATPP_FOUND )
ENDIF( EXPAT_FOUND )

ADD_EXECUTABLE( bmdl_pro_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bmdl_pro_test_include bmdl_pro bprb brdb vil vil_io vgl vgl_io vpgl)

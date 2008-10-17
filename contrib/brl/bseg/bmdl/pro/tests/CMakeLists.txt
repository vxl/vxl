
INCLUDE_DIRECTORIES( ${BRL_SOURCE_DIR}/bseg/bmdl )

ADD_EXECUTABLE( bmdl_pro_test_all
  test_driver.cxx
  test_bmdl_classify_process.cxx
  test_bmdl_trace_boundaries_process.cxx
  test_bmdl_generate_mesh_process.cxx	
)

TARGET_LINK_LIBRARIES( bmdl_pro_test_all testlib bmdl_pro bmdl vcl vbl bprb brdb vil vil_io vil_pro)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/first_ret.tif ${CMAKE_CURRENT_BINARY_DIR}/first_ret.tif COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/last_ret.tif ${CMAKE_CURRENT_BINARY_DIR}/last_ret.tif COPYONLY )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/ground.tif ${CMAKE_CURRENT_BINARY_DIR}/ground.tif COPYONLY )

ADD_TEST( bmdl_classify_process    ${EXECUTABLE_OUTPUT_PATH}/bmdl_pro_test_all   test_bmdl_classify_process )
ADD_TEST( bmdl_trace_boundaries_process    ${EXECUTABLE_OUTPUT_PATH}/bmdl_pro_test_all   test_trace_boundaries_process )
ADD_TEST( bmdl_generate_mesh_process    ${EXECUTABLE_OUTPUT_PATH}/bmdl_pro_test_all   test_generate_mesh_process )

ADD_EXECUTABLE( bmdl_pro_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bmdl_pro_test_include bmdl_pro )

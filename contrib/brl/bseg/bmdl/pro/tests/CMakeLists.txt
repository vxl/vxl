include_directories( ${BRL_INCLUDE_DIR}/bseg/bmdl )
if( EXPAT_FOUND )
 if(EXPATPP_FOUND)

 add_executable( bmdl_pro_test_all
  test_driver.cxx
  test_bmdl_classify_process.cxx
  test_bmdl_trace_boundaries_process.cxx
  test_bmdl_generate_mesh_process.cxx
 )

target_link_libraries( bmdl_pro_test_all ${VXL_LIB_PREFIX}testlib bmdl_pro bmdl vil_pro bprb brdb sdet ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vcl expat expatpp)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/first_ret.tif       ${CMAKE_CURRENT_BINARY_DIR}/first_ret.tif       COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/last_ret.tif        ${CMAKE_CURRENT_BINARY_DIR}/last_ret.tif        COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ground.tif          ${CMAKE_CURRENT_BINARY_DIR}/ground.tif          COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/height.tif          ${CMAKE_CURRENT_BINARY_DIR}/height.tif          COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/label.tif           ${CMAKE_CURRENT_BINARY_DIR}/label.tif           COPYONLY )
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/classify_params.xml ${CMAKE_CURRENT_BINARY_DIR}/classify_params.xml COPYONLY )

if(SEGFAULT_FIXED)
add_test( NAME bmdl_pro_classify_process COMMAND $<TARGET_FILE:bmdl_pro_test_all> test_bmdl_classify_process )
endif()
add_test( NAME bmdl_pro_trace_boundaries_process COMMAND $<TARGET_FILE:bmdl_pro_test_all> test_bmdl_trace_boundaries_process ${CMAKE_CURRENT_SOURCE_DIR} )
add_test( NAME bmdl_pro_generate_mesh_process COMMAND $<TARGET_FILE:bmdl_pro_test_all> test_bmdl_generate_mesh_process    ${CMAKE_CURRENT_SOURCE_DIR} )
 endif()
endif()

add_executable( bmdl_pro_test_include test_include.cxx )
target_link_libraries( bmdl_pro_test_include bmdl_pro bprb brdb ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vgl_io ${VXL_LIB_PREFIX}vpgl)

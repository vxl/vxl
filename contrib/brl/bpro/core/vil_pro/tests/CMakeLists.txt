include_directories( ${BRL_INCLUDE_DIR}/bpro/core )
if(EXPAT_FOUND)
 if(EXPATPP_FOUND)

 add_executable( vil_pro_test_all
  test_driver.cxx
  test_vil_convert_to_n_planes_process.cxx
  test_vil_crop_image_process.cxx
 )

target_link_libraries(vil_pro_test_all ${VXL_LIB_PREFIX}testlib vil_pro bprb sdet ${VXL_LIB_PREFIX}vil expat expatpp)

add_test( NAME bpro_vil_test_convert_to_n_planes_process COMMAND $<TARGET_FILE:vil_pro_test_all> test_vil_convert_to_n_planes_process )
add_test( NAME bpro_vil_test_crop_image_process COMMAND $<TARGET_FILE:vil_pro_test_all> test_vil_crop_image_process )

 endif()
endif()

add_executable( vil_pro_test_include test_include.cxx )
target_link_libraries( vil_pro_test_include vil_pro sdet )

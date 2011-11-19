INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )
IF(EXPAT_FOUND)
 IF(EXPATPP_FOUND)

 ADD_EXECUTABLE( vil_pro_test_all
  test_driver.cxx
  test_vil_convert_to_n_planes_process.cxx
  test_vil_crop_image_process.cxx
 )

TARGET_LINK_LIBRARIES(vil_pro_test_all testlib vil_pro bprb sdet vil expat expatpp)

ADD_TEST( bpro_vil_test_convert_to_n_planes_process ${EXECUTABLE_OUTPUT_PATH}/vil_pro_test_all test_vil_convert_to_n_planes_process )
ADD_TEST( bpro_vil_test_crop_image_process          ${EXECUTABLE_OUTPUT_PATH}/vil_pro_test_all test_vil_crop_image_process )

 ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)

ADD_EXECUTABLE( vil_pro_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil_pro_test_include vil_pro sdet )

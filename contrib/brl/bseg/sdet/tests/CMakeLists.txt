# brl/bbas/bseg/sdet/tests/CMakeLists.txt
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas)

ADD_EXECUTABLE(sdet_test_all
  test_driver.cxx
  test_detector.cxx
  test_region_proc.cxx
  test_gauss_fit.cxx
  test_denoise_mrf.cxx
  test_denoise_mrf_bp.cxx
  test_segmentation.cxx
  test_sel.cxx
)
TARGET_LINK_LIBRARIES(sdet_test_all sdet imesh_algo vtol vil1 vnl_algo vnl vgl vil testlib)

ADD_TEST(sdet_test_detector    ${EXECUTABLE_OUTPUT_PATH}/sdet_test_all test_detector    ${CMAKE_CURRENT_SOURCE_DIR}/jar-closeup.tif)
ADD_TEST(sdet_test_region_proc ${EXECUTABLE_OUTPUT_PATH}/sdet_test_all test_region_proc ${CMAKE_CURRENT_SOURCE_DIR}/jar-closeup.tif)
ADD_TEST(sdet_test_gauss_fit   ${EXECUTABLE_OUTPUT_PATH}/sdet_test_all test_gauss_fit )
ADD_TEST(sdet_test_denoise_mrf   ${EXECUTABLE_OUTPUT_PATH}/sdet_test_all test_denoise_mrf )
ADD_TEST(sdet_test_denoise_mrf_bp   ${EXECUTABLE_OUTPUT_PATH}/sdet_test_all test_denoise_mrf_bp )
ADD_TEST(sdet_test_segmentation   ${EXECUTABLE_OUTPUT_PATH}/sdet_test_all test_segmentation )
ADD_TEST(sdet_test_sel   ${EXECUTABLE_OUTPUT_PATH}/sdet_test_all test_sel)
ADD_EXECUTABLE(sdet_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(sdet_test_include sdet)

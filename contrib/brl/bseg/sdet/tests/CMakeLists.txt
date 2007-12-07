# brl/bbas/bseg/sdet/tests/CMakeLists.txt
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR})

ADD_EXECUTABLE(sdet_test_all
  test_driver.cxx
  test_detector.cxx
  test_region_proc.cxx
  test_gauss_fit.cxx
)
TARGET_LINK_LIBRARIES(sdet_test_all sdet vtol vil1 testlib)

ADD_TEST(sdet_test_detector    ${EXECUTABLE_OUTPUT_PATH}/sdet_test_all test_detector ${CMAKE_CURRENT_SOURCE_DIR}/jar-closeup.tif)
ADD_TEST(sdet_test_region_proc ${EXECUTABLE_OUTPUT_PATH}/sdet_test_all test_region_proc ${CMAKE_CURRENT_SOURCE_DIR}/jar-closeup.tif)
ADD_TEST(sdet_test_gauss_fit ${EXECUTABLE_OUTPUT_PATH}/sdet_test_all test_gauss_fit )
ADD_EXECUTABLE(sdet_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(sdet_test_include sdet)

# brl/bbas/bseg/sdet/tests/CMakeLists.txt
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg)

ADD_EXECUTABLE(sdet_test_all
  test_driver.cxx
  test_detector.cxx
  test_region_proc.cxx
)
TARGET_LINK_LIBRARIES(sdet_test_all sdet vtol vil1 testlib)

ADD_TEST(sdet_test_detector    sdet_test_all test_detector ${CMAKE_CURRENT_SOURCE_DIR}/jar-closeup.tif)
ADD_TEST(sdet_test_region_proc sdet_test_all test_region_proc ${CMAKE_CURRENT_SOURCE_DIR}/jar-closeup.tif)

ADD_EXECUTABLE(sdet_test_include test_include.cxx)

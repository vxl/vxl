# brl/bbas/bdgl/tests/CMakeLists.txt
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg)

LINK_LIBRARIES(sdet brip btol bdgl vdgl vil1 vnl testlib)

ADD_EXECUTABLE(sdet_test_all
  test_driver.cxx
  sdet_test_detector.cxx
  sdet_test_region_proc.cxx
  brip_test_lucas_kanade.cxx
)

ADD_TEST(sdet_test_detector     sdet_test_all sdet_test_detector ${CMAKE_CURRENT_SOURCE_DIR}/jar-closeup.tif)
ADD_TEST(sdet_test_region_proc  sdet_test_all sdet_test_region_proc ${CMAKE_CURRENT_SOURCE_DIR}/jar-closeup.tif)
ADD_TEST(brip_test_lucas_kanade sdet_test_all brip_test_lucas_kanade)


ADD_EXECUTABLE(brip_test_fourier brip_test_fourier.cxx)
ADD_TEST(brip_test_fourier brip_test_fourier)

# brl/bbas/bdgl/tests/CMakeLists.txt
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg)

ADD_EXECUTABLE(bseg_test_all
  test_driver.cxx
  sdet_test_detector.cxx
  sdet_test_region_proc.cxx
  brip_test_lucas_kanade.cxx
  brip_test_fourier.cxx
  brip_test_homography.cxx
)
TARGET_LINK_LIBRARIES(bseg_test_all sdet brip btol bdgl vdgl vil1 vnl testlib)

ADD_TEST(sdet_test_detector     bseg_test_all sdet_test_detector ${CMAKE_CURRENT_SOURCE_DIR}/jar-closeup.tif)
ADD_TEST(sdet_test_region_proc  bseg_test_all sdet_test_region_proc ${CMAKE_CURRENT_SOURCE_DIR}/jar-closeup.tif)
ADD_TEST(brip_test_lucas_kanade bseg_test_all brip_test_lucas_kanade)
ADD_TEST(brip_test_fourier      bseg_test_all brip_test_fourier)
ADD_TEST(brip_test_homography   bseg_test_all brip_test_homography)

ADD_EXECUTABLE( bseg_test_include test_include.cxx)
TARGET_LINK_LIBRARIES( bseg_test_include bmrf brip sdet segv strk vpro)

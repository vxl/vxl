# brl/bbas/bdgl/tests/CMakeLists.txt
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg)

LINK_LIBRARIES(sdet brip btol bdgl vdgl vil1 vnl)

ADD_EXECUTABLE(sdet_test_detector  sdet_test_detector.cxx)
ADD_TEST(sdet_test_detector sdet_test_detector ${CMAKE_CURRENT_SOURCE_DIR}/jar-closeup.tif)
ADD_EXECUTABLE(sdet_test_region_proc  sdet_test_region_proc.cxx)
ADD_TEST(sdet_test_region_proc sdet_test_region_proc ${CMAKE_CURRENT_SOURCE_DIR}/jar-closeup.tif)

ADD_EXECUTABLE( bseg_test_include test_include.cxx )

ADD_EXECUTABLE(brip_test_lucas_kanade brip_test_lucas_kanade.cxx)
ADD_TEST(brip_test_lucas_kanade brip_test_lucas_kanade)

ADD_EXECUTABLE(sdet_test_tracker sdet_test_tracker.cxx)
ADD_TEST(sdet_test_tracker sdet_test_tracker)

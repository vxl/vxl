# brl/bbas/bdgl/tests/CMakeLists.txt
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas)
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg)

LINK_LIBRARIES(sdet btol bdgl vdgl vil vnl)

ADD_EXECUTABLE(sdet_test_detector  sdet_test_detector.cxx)
ADD_TEST(sdet_test_detector sdet_test_detector ${BRL_INCLUDE_DIR}/bseg/tests/jar-closeup.tif)
ADD_EXECUTABLE(sdet_test_region_proc  sdet_test_region_proc.cxx)
ADD_TEST(sdet_test_region_proc sdet_test_region_proc ${BRL_INCLUDE_DIR}/bseg/tests/jar-closeup.tif)

ADD_EXECUTABLE( bseg_test_include test_include.cxx )

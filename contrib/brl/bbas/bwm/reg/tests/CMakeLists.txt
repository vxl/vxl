# brl/bbas/bwm/reg/tests/CMakeLists.txt
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas)

ADD_EXECUTABLE(bwm_reg_test_all
  test_driver.cxx
  test_matcher.cxx
)
TARGET_LINK_LIBRARIES(bwm_reg_test_all bwm_reg vsol testlib)

ADD_TEST(bwm_reg_test_matcher    ${EXECUTABLE_OUTPUT_PATH}/bwm_reg_test_all test_matcher)


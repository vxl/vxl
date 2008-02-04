# brl/bpro/bprb/tests/CMakeLists.txt

ADD_EXECUTABLE( bprb_test_all
  test_driver.cxx
  test_process.cxx
  bprb_test_process.h bprb_test_process.cxx
)

TARGET_LINK_LIBRARIES( bprb_test_all bprb testlib )

ADD_TEST( bprb_test_process ${EXECUTABLE_OUTPUT_PATH}/bprb_test_all test_process )


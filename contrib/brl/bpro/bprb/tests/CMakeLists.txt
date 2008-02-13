# brl/bpro/bprb/tests/CMakeLists.txt

ADD_EXECUTABLE( bprb_test_all
  test_driver.cxx
  test_process.cxx
  test_process_params.cxx
  bprb_test_process.h bprb_test_process.cxx
)

TARGET_LINK_LIBRARIES( bprb_test_all bprb testlib )

ADD_TEST( bprb_test_process ${EXECUTABLE_OUTPUT_PATH}/bprb_test_all test_process )
ADD_TEST( bprb_test_process ${EXECUTABLE_OUTPUT_PATH}/bprb_test_all test_process_params ${CMAKE_CURRENT_SOURCE_DIR}/params.xml)



# brl/bpro/bprb/tests/CMakeLists.txt
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/params.xml
                ${CMAKE_CURRENT_BINARY_DIR}/params.xml)

ADD_EXECUTABLE( bprb_test_all
  test_driver.cxx
  test_process.cxx
  test_process_params.cxx
  bprb_test_process.h bprb_test_process.cxx
)
TARGET_LINK_LIBRARIES( bprb_test_all bprb testlib )

ADD_TEST( bprb_test_process ${EXECUTABLE_OUTPUT_PATH}/bprb_test_all test_process )
ADD_TEST( bprb_test_process_params ${EXECUTABLE_OUTPUT_PATH}/bprb_test_all test_process_params )

ADD_EXECUTABLE(bprb_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(bprb_test_include bprb)

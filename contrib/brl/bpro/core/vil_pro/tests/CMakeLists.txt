INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )

ADD_EXECUTABLE( vil_pro_test_all
  test_driver.cxx
  test_vil_convert_to_n_planes_process.cxx
)

TARGET_LINK_LIBRARIES(vil_pro_test_all testlib vil vil_pro bprb)

ADD_TEST( vil_convert_to_n_planes_process    ${EXECUTABLE_OUTPUT_PATH}/vil_pro_test_all   test_vil_convert_to_n_planes_process )

ADD_EXECUTABLE( vil_pro_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( vil_pro_test_include vil_pro )

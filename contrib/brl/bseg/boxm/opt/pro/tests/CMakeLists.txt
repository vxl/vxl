INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boxm/opt )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boxm/opt/pro)

ADD_EXECUTABLE( boxm_opt_pro_test_all
  test_driver.cxx
  test_boxm_plane_propagation_process.cxx
)

TARGET_LINK_LIBRARIES( boxm_opt_pro_test_all testlib boxm_opt boxm_opt_pro boxm boxm_pro boct bvpl bprb brdb vcl )

ADD_TEST( boxm_plane_propagation_process     ${EXECUTABLE_OUTPUT_PATH}/boxm_opt_pro_test_all   test_boxm_plane_propagation_process)

ADD_EXECUTABLE( boxm_opt_pro_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm_opt_pro_test_include boxm_pro )

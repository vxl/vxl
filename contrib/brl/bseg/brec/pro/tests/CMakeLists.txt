INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/bvxm )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro/core )

ADD_EXECUTABLE( brec_pro_test_all
  test_driver.cxx
  test_brec_update_changes_process.cxx
  test_brec_create_mog_image_process.cxx
)

TARGET_LINK_LIBRARIES( brec_pro_test_all testlib brec_pro bvxm_pro bbgm_pro bvxm_io vcl vbl vpgl_algo vpgl vpgl_io vpgl_pro bprb brdb vil vnl vgl vul vil_io vil_pro)

ADD_TEST( brec_pro_test_brec_update_changes_process    ${EXECUTABLE_OUTPUT_PATH}/brec_pro_test_all   test_brec_update_changes_process )
ADD_TEST( brec_pro_test_brec_create_mog_image_process  ${EXECUTABLE_OUTPUT_PATH}/brec_pro_test_all   test_brec_create_mog_image_process )

ADD_EXECUTABLE( brec_pro_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( brec_pro_test_include brec_pro )

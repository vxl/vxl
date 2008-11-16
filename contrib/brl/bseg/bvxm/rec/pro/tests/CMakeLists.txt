INCLUDE_DIRECTORIES( ${BRL_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_SOURCE_DIR}/bseg/bvxm )
INCLUDE_DIRECTORIES( ${BRL_SOURCE_DIR}/bpro/core )

ADD_EXECUTABLE( bvxm_rec_pro_test_all
  test_driver.cxx
  test_bvxm_rec_update_changes_process.cxx
)

TARGET_LINK_LIBRARIES( bvxm_rec_pro_test_all testlib bvxm_rec_pro bvxm_pro bvxm_io vcl vbl vpgl_algo vpgl vpgl_io vpgl_pro bprb brdb vil vil_io vil_pro)

ADD_TEST( bvxm_test_bvxm_rec_update_changes_process    ${EXECUTABLE_OUTPUT_PATH}/bvxm_rec_pro_test_all   test_bvxm_rec_update_changes_process )

ADD_EXECUTABLE( bvxm_rec_pro_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bvxm_rec_pro_test_include bvxm_rec_pro bvxm_pro )

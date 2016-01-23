include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bseg/bvxm )
include_directories( ${BRL_INCLUDE_DIR}/bpro/core )

add_executable( brec_pro_test_all
  test_driver.cxx
  test_brec_update_changes_process.cxx
  test_brec_create_mog_image_process.cxx
)

target_link_libraries( brec_pro_test_all testlib brec_pro bvxm_pro bbgm_pro bvxm_io vcl vbl vpgl_algo vpgl vpgl_io vpgl_pro bprb brdb vil vnl vgl vul vil_io vil_pro)

add_test( NAME brec_pro_test_brec_update_changes_process COMMAND $<TARGET_FILE:brec_pro_test_all>   test_brec_update_changes_process )
add_test( NAME brec_pro_test_brec_create_mog_image_process COMMAND $<TARGET_FILE:brec_pro_test_all>   test_brec_create_mog_image_process )

add_executable( brec_pro_test_include test_include.cxx )
target_link_libraries( brec_pro_test_include brec_pro )

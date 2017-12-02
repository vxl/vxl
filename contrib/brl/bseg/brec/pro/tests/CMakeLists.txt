include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bseg/bvxm )
include_directories( ${BRL_INCLUDE_DIR}/bpro/core )

add_executable( brec_pro_test_all
  test_driver.cxx
  test_brec_update_changes_process.cxx
  test_brec_create_mog_image_process.cxx
)

target_link_libraries( brec_pro_test_all ${VXL_LIB_PREFIX}testlib brec_pro bvxm_pro bbgm_pro bvxm_io ${VXL_LIB_PREFIX}vcl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_io vpgl_pro bprb brdb ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil_io vil_pro)

add_test( NAME brec_pro_test_brec_update_changes_process COMMAND $<TARGET_FILE:brec_pro_test_all>   test_brec_update_changes_process )
add_test( NAME brec_pro_test_brec_create_mog_image_process COMMAND $<TARGET_FILE:brec_pro_test_all>   test_brec_create_mog_image_process )

add_executable( brec_pro_test_include test_include.cxx )
target_link_libraries( brec_pro_test_include brec_pro )

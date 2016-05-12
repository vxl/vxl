# contrib/brl/bbas/volm/conf/tests/CMakeLists.txt

add_executable( volm_conf_test_all
  test_driver.cxx
  test_volm_conf_object.cxx
  test_volm_conf_score.cxx
  test_volm_conf_query.cxx
  test_volm_conf_buffer.cxx
)

target_link_libraries( volm_conf_test_all volm_conf ${VXL_LIB_PREFIX}testlib )

add_test( NAME volm_conf_test_object COMMAND $<TARGET_FILE:volm_conf_test_all> test_volm_conf_object )
add_test( NAME volm_conf_test_score COMMAND $<TARGET_FILE:volm_conf_test_all> test_volm_conf_score )
add_test( NAME volm_conf_test_query COMMAND $<TARGET_FILE:volm_conf_test_all> test_volm_conf_query  )
add_test( NAME volm_conf_test_buffer COMMAND $<TARGET_FILE:volm_conf_test_all> test_volm_conf_buffer )

add_executable( volm_conf_test_include test_include.cxx )
target_link_libraries( volm_conf_test_include volm_conf )

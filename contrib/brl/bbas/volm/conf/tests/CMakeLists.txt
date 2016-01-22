# contrib/brl/bbas/volm/conf/tests/CMakeLists.txt

ADD_EXECUTABLE( volm_conf_test_all
  test_driver.cxx
  test_volm_conf_object.cxx
  test_volm_conf_score.cxx
  test_volm_conf_query.cxx
  test_volm_conf_buffer.cxx
)

TARGET_LINK_LIBRARIES( volm_conf_test_all volm_conf testlib )

ADD_TEST( NAME volm_conf_test_object COMMAND $<TARGET_FILE:volm_conf_test_all> test_volm_conf_object )
ADD_TEST( NAME volm_conf_test_score COMMAND $<TARGET_FILE:volm_conf_test_all> test_volm_conf_score )
ADD_TEST( NAME volm_conf_test_query COMMAND $<TARGET_FILE:volm_conf_test_all> test_volm_conf_query  )
ADD_TEST( NAME volm_conf_test_buffer COMMAND $<TARGET_FILE:volm_conf_test_all> test_volm_conf_buffer )

ADD_EXECUTABLE( volm_conf_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( volm_conf_test_include volm_conf )

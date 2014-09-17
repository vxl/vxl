#This is contrib/brl/bseg/boxm2/volm/conf/tests/CMakeLists.txt

ADD_EXECUTABLE( boxm2_volm_conf_test_all
  test_driver.cxx
  test_boxm2_volm_conf_matcher.cxx
)

TARGET_LINK_LIBRARIES( boxm2_volm_conf_test_all boxm2_volm_conf testlib )

ADD_TEST( boxm2_volm_conf_matcher ${EXECUTABLE_OUTPUT_PATH}/boxm2_volm_conf_test_all test_conf_matcher )

ADD_EXECUTABLE( boxm2_volm_conf_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( boxm2_volm_conf_test_include boxm2_volm_conf )
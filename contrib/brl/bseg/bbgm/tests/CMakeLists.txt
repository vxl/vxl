# brl/bseg/bbgm/tests/CMakeLists.txt
# Tests for bbgm library

add_executable( bbgm_test_all
  test_driver.cxx
  test_bg_model_speed.cxx
  test_measure.cxx
)

target_link_libraries( bbgm_test_all bbgm bsta_algo bsta ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}testlib )

add_test( NAME bbgm_test_bg_model_speed COMMAND $<TARGET_FILE:bbgm_test_all> test_bg_model_speed )
add_test( NAME bbgm_test_measure COMMAND $<TARGET_FILE:bbgm_test_all> test_measure )

add_executable( bbgm_test_include test_include.cxx )
target_link_libraries( bbgm_test_include bbgm)
add_executable( bbgm_test_template_include test_template_include.cxx )
target_link_libraries( bbgm_test_template_include bbgm)

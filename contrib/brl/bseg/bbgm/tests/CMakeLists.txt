# brcv/seg/bbgm/tests/CMakeLists.txt
# Tests for dbbgm library

ADD_EXECUTABLE( bbgm_test_all
  test_driver.cxx
  test_bg_model_speed.cxx
)

TARGET_LINK_LIBRARIES( bbgm_test_all bbgm bsta_algo bsta vul vil testlib )

ADD_TEST( bbgm_test_bg_model_speed           bbgm_test_all test_bg_model_speed )

ADD_EXECUTABLE( bbgm_test_include test_include.cxx )

TARGET_LINK_LIBRARIES( bbgm_test_include dbbgm)

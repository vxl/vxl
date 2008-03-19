# brl/bseg/bbgm/tests/CMakeLists.txt
# Tests for bbgm library

ADD_EXECUTABLE( bbgm_pro_test_all
  test_driver.cxx
  test_io.cxx
  test_update_dist_image.cxx
)

TARGET_LINK_LIBRARIES( bbgm_pro_test_all bbgm_pro bbgm bsta_algo bsta vil_bpro brdb vpl vul vil testlib )

ADD_TEST( bbgm_test_io  bbgm_pro_test_all test_io )
ADD_TEST( bbgm_test_io  bbgm_pro_test_all test_update_dist_image )

ADD_EXECUTABLE( bbgm_pro_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bbgm_pro_test_include bbgm_pro bbgm)

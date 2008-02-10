# brl/bseg/bbgm/tests/CMakeLists.txt
# Tests for bbgm library

ADD_EXECUTABLE( bbgm_bpro_test_all
  test_driver.cxx
  test_io.cxx
  test_update_dist_image.cxx
)

TARGET_LINK_LIBRARIES( bbgm_bpro_test_all bbgm_bpro bbgm bsta_algo bsta vil_bpro vpl vul vil testlib )

ADD_TEST( bbgm_test_io  bbgm_bpro_test_all test_io )
ADD_TEST( bbgm_test_io  bbgm_bpro_test_all test_update_dist_image )


#ADD_EXECUTABLE( bbgm_bpro_test_include test_include.cxx )

#TARGET_LINK_LIBRARIES( bbgm_bpro_test_include bbgm_pro bbgm)

# brl/bseg/bbgm/tests/CMakeLists.txt
# Tests for bbgm library
IF( EXPAT_FOUND )
 IF(EXPATPP_FOUND)

 ADD_EXECUTABLE( bbgm_pro_test_all
  test_driver.cxx
  test_io.cxx
  test_update_dist_image.cxx
#  test_neighborhood_ops.cxx
)

TARGET_LINK_LIBRARIES( bbgm_pro_test_all bbgm_pro bbgm bsta_algo bsta sdet vil_pro brdb vnl vil vpl testlib expat expatpp )

ADD_TEST( NAME bbgm_pro_test_io COMMAND $<TARGET_FILE:bbgm_pro_test_all> test_io )
ADD_TEST( NAME bbgm_pro_test_update_dist_image COMMAND $<TARGET_FILE:bbgm_pro_test_all> test_update_dist_image )
#ADD_TEST( NAME bbgm_pro_test_neighborhood_ops COMMAND $<TARGET_FILE:bbgm_pro_test_all> test_neighborhood_ops )
 ENDIF( EXPATPP_FOUND )
ENDIF( EXPAT_FOUND )

ADD_EXECUTABLE( bbgm_pro_test_include test_include.cxx )
TARGET_LINK_LIBRARIES( bbgm_pro_test_include bbgm_pro bbgm sdet)

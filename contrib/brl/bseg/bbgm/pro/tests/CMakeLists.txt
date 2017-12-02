# brl/bseg/bbgm/tests/CMakeLists.txt
# Tests for bbgm library
if( EXPAT_FOUND )
 if(EXPATPP_FOUND)

 add_executable( bbgm_pro_test_all
  test_driver.cxx
  test_io.cxx
  test_update_dist_image.cxx
#  test_neighborhood_ops.cxx
)

target_link_libraries( bbgm_pro_test_all bbgm_pro bbgm bsta_algo bsta sdet vil_pro brdb ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}testlib expat expatpp )

add_test( NAME bbgm_pro_test_io COMMAND $<TARGET_FILE:bbgm_pro_test_all> test_io )
add_test( NAME bbgm_pro_test_update_dist_image COMMAND $<TARGET_FILE:bbgm_pro_test_all> test_update_dist_image )
#add_test( NAME bbgm_pro_test_neighborhood_ops COMMAND $<TARGET_FILE:bbgm_pro_test_all> test_neighborhood_ops )
 endif()
endif()

add_executable( bbgm_pro_test_include test_include.cxx )
target_link_libraries( bbgm_pro_test_include bbgm_pro bbgm sdet)

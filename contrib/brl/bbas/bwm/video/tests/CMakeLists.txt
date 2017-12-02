# brl/bbas/bwm/video/tests/CMakeLists.txt
if(EXPAT_FOUND)
 if(EXPATPP_FOUND)
  include_directories(${EXPAT_INCLUDE_DIRS} )
  include_directories(${EXPATPP_INCLUDE_DIRS} )

add_executable( bwm_video_test_all
                test_driver.cxx
                test_cam_iostream.cxx
                test_video_site_io.cxx
                test_video_corr_processor.cxx
              )
target_link_libraries(bwm_video_test_all bwm_video ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vpl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}testlib expat expatpp)

add_test( NAME bwm_video_test_cam_iostream COMMAND $<TARGET_FILE:bwm_video_test_all> test_cam_iostream)
add_test( NAME bwm_video_test_site_io COMMAND $<TARGET_FILE:bwm_video_test_all> test_video_site_io)
add_test( NAME bwm_video_test_corr_processor COMMAND $<TARGET_FILE:bwm_video_test_all> test_video_corr_processor)
 endif()
endif()

add_executable(bwm_video_test_include test_include.cxx)
target_link_libraries(bwm_video_test_include bwm_video)

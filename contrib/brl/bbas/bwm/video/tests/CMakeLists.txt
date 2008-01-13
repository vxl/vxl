# /contrib/bwm/video/tests/CMakeLists.txt


ADD_EXECUTABLE( bwm_video_test_all   
                test_driver.cxx  
		test_cam_iostream.cxx
		test_video_site_io.cxx
		test_video_corr_processor.cxx
              )

TARGET_LINK_LIBRARIES(bwm_video_test_all bwm_video bwm bwm_algo vpgl_algo vpgl testlib)

ADD_TEST(test_cam_iostream ${EXECUTABLE_OUTPUT_PATH}/bwm_video_test_all test_cam_iostream)
ADD_TEST(test_video_site_io ${EXECUTABLE_OUTPUT_PATH}/bwm_video_test_all test_video_site_io)
ADD_TEST(test_video_corr_processor ${EXECUTABLE_OUTPUT_PATH}/bwm_video_test_all test_video_corr_processor)



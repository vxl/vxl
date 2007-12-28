# /contrib/bwm/video/tests/CMakeLists.txt


ADD_EXECUTABLE( bwm_video_test_all   
                test_driver.cxx  
		test_cam_iostream.cxx
		test_video_site_io.cxx
              )

TARGET_LINK_LIBRARIES(bwm_video_test_all bwm_video bwm bwm_algo vpgl testlib)

ADD_TEST(test_cam_iostream bwm_video_test_all)
ADD_TEST(test_video_site_io bwm_video_test_all)



# brl/bbas/bwm/video/tests/CMakeLists.txt

ADD_EXECUTABLE( bwm_video_test_all
                test_driver.cxx
                test_cam_iostream.cxx
                test_video_site_io.cxx
                test_video_corr_processor.cxx
              )
TARGET_LINK_LIBRARIES(bwm_video_test_all bwm_video vpgl_algo vpgl vnl vgl_algo vgl vpl vul testlib)

ADD_TEST(bwm_video_test_cam_iostream   ${EXECUTABLE_OUTPUT_PATH}/bwm_video_test_all test_cam_iostream)
ADD_TEST(bwm_video_test_site_io        ${EXECUTABLE_OUTPUT_PATH}/bwm_video_test_all test_video_site_io)
ADD_TEST(bwm_video_test_corr_processor ${EXECUTABLE_OUTPUT_PATH}/bwm_video_test_all test_video_corr_processor)

ADD_EXECUTABLE(bwm_video_test_include test_include.cxx)
TARGET_LINK_LIBRARIES(bwm_video_test_include bwm_video)

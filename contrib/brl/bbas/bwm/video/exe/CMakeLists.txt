# This is contrib/bwm/exe/CMakeLists.txt
# World Modeling GUI executables

ADD_EXECUTABLE(bwm_corr_proc bwm_video_corr_proc.cxx)
TARGET_LINK_LIBRARIES(bwm_corr_proc  bwm_video vnl vpl vul)

ADD_EXECUTABLE(bwm_planar_reg_proc bwm_video_planar_reg_proc.cxx)
TARGET_LINK_LIBRARIES(bwm_planar_reg_proc  bwm_video vnl vgl vpl vul)

ADD_EXECUTABLE(bwm_extract_snippets bwm_extract_snippets.cxx)
TARGET_LINK_LIBRARIES(bwm_extract_snippets  bwm_video vsol vnl vil vpl vul vsl)

ADD_EXECUTABLE(bwm_transform_video_site_proc bwm_transform_video_site_proc.cxx)
TARGET_LINK_LIBRARIES(bwm_transform_video_site_proc  bwm_video vsol vil vnl vgl vpl vul vsl)

ADD_EXECUTABLE(bwm_corr_proc_gt bwm_video_corr_proc_gt.cxx)
TARGET_LINK_LIBRARIES(bwm_corr_proc_gt  bwm_video vgl_algo vnl vpl vul)


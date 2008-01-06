# This is contrib/bwm/exe/CMakeLists.txt
# World Modeling GUI executables
#INCLUDE_DIRECTORIES(${lems_SOURCE_DIR}/contrib )


ADD_EXECUTABLE(bwm_corr_proc bwm_video_corr_proc.cxx)
TARGET_LINK_LIBRARIES(bwm_corr_proc  bwm_video vpl vul)

ADD_EXECUTABLE(bwm_planar_reg_proc bwm_video_planar_reg_proc.cxx)
TARGET_LINK_LIBRARIES(bwm_planar_reg_proc  bwm_video vpl vul)

# This is contrib/bwm/exe/CMakeLists.txt
# World Modeling GUI executables

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

IF(EXPAT_FOUND)
 IF(EXPATPP_FOUND)
  INCLUDE_DIRECTORIES (${EXPAT_INCLUDE_DIRS} )
  INCLUDE_DIRECTORIES (${EXPATPP_INCLUDE_DIRS} )

  ADD_EXECUTABLE(bwm_corr_proc bwm_video_corr_proc.cxx)
  TARGET_LINK_LIBRARIES(bwm_corr_proc  bwm_video vnl vpl vul expat  expatpp)

  ADD_EXECUTABLE(bwm_corr_proc_gt bwm_video_corr_proc_gt.cxx)
  TARGET_LINK_LIBRARIES(bwm_corr_proc_gt  bwm_video vgl_algo vnl vpl vul expat  expatpp)

ADD_EXECUTABLE(bwm_transform_video_site_proc bwm_transform_video_site_proc.cxx)
TARGET_LINK_LIBRARIES(bwm_transform_video_site_proc  bwm_video vsol vil vnl vgl vpl vul vsl expat expatpp)

ADD_EXECUTABLE(bwm_video_corr_to_groundtruth_images bwm_video_corr_to_groundtruth_images.cxx)
TARGET_LINK_LIBRARIES(bwm_video_corr_to_groundtruth_images  bwm_video vnl vpl vul expat expatpp)

ADD_EXECUTABLE(bwm_video_normalize bwm_video_normalize.cxx)
TARGET_LINK_LIBRARIES(bwm_video_normalize  bwm_video vnl vpl vul expat expatpp)

 ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)


ADD_EXECUTABLE(bwm_planar_reg_proc bwm_video_planar_reg_proc.cxx)
TARGET_LINK_LIBRARIES(bwm_planar_reg_proc  bwm_video vnl vgl vpl vul )

ADD_EXECUTABLE(bwm_extract_snippets bwm_extract_snippets.cxx)
TARGET_LINK_LIBRARIES(bwm_extract_snippets  bwm_video vsol vnl vil vbl_io vpl vul vsl)

INCLUDE( ${VXL_CMAKE_DIR}/UseVGUI.cmake )
IF(HAS_VGUI)
	ADD_EXECUTABLE(bwm_video_normalize_3d_corr bwm_video_normalize_3d_corr.cxx)
	TARGET_LINK_LIBRARIES(bwm_video_normalize_3d_corr  bwm bwm_algo bwm_video vnl vpl vul)
ENDIF(HAS_VGUI)
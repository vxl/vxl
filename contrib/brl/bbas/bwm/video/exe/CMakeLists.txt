# This is contrib/bwm/exe/CMakeLists.txt
# World Modeling GUI executables

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

if(EXPAT_FOUND)
  if(EXPATPP_FOUND)
    include_directories(${EXPAT_INCLUDE_DIRS} )
    include_directories(${EXPATPP_INCLUDE_DIRS} )

    add_executable(bwm_corr_proc bwm_video_corr_proc.cxx)
    target_link_libraries(bwm_corr_proc  bwm_video vnl vpl vul expat  expatpp)

    add_executable(bwm_corr_proc_gt bwm_video_corr_proc_gt.cxx)
    target_link_libraries(bwm_corr_proc_gt  bwm_video vgl_algo vnl vpl vul expat  expatpp)

    add_executable(bwm_transform_video_site_proc bwm_transform_video_site_proc.cxx)
    target_link_libraries(bwm_transform_video_site_proc  bwm_video vsol vil vnl vgl vpl vul vsl expat expatpp)

    add_executable(bwm_video_corr_to_groundtruth_images bwm_video_corr_to_groundtruth_images.cxx)
    target_link_libraries(bwm_video_corr_to_groundtruth_images  bwm_video vnl vpl vul expat expatpp)

    add_executable(bwm_video_normalize bwm_video_normalize.cxx)
    target_link_libraries(bwm_video_normalize  bwm_video vnl vpl vul expat expatpp)

  endif(EXPATPP_FOUND)
endif(EXPAT_FOUND)


add_executable(bwm_planar_reg_proc bwm_video_planar_reg_proc.cxx)
target_link_libraries(bwm_planar_reg_proc  bwm_video vnl vgl vpl vul )

add_executable(bwm_extract_snippets bwm_extract_snippets.cxx)
target_link_libraries(bwm_extract_snippets  bwm_video vsol vnl vil vbl_io vpl vul vsl)

include( ${VXL_CMAKE_DIR}/UseVGUI.cmake )
if(HAS_VGUI)
  add_executable(bwm_video_normalize_3d_corr bwm_video_normalize_3d_corr.cxx)
  target_link_libraries(bwm_video_normalize_3d_corr bwm_video vgui)
endif(HAS_VGUI)

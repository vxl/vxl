# This is brl/bbas/bwm/exe/CMakeLists.txt
# World Modeling GUI executables

include_directories( ${VTHREEP_INCLUDE_DIR}/rply )
include_directories( ${MUL_INCLUDE_DIR} )
include_directories( ${OXL_INCLUDE_DIR} )

if(EXPAT_FOUND)
 if(EXPATPP_FOUND)
  if(SHAPELIB_FOUND)

   add_executable( bwm_batch_project bwm_batch_project.cxx)
   target_link_libraries( bwm_batch_project bwm bwm_algo ${VXL_LIB_PREFIX}vcl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vul)

   add_executable( bwm_main bwm_main.cxx)
   target_link_libraries( bwm_main bwm bwm_algo bwm_io bwm_reg ${VXL_LIB_PREFIX}vgui ${VXL_LIB_PREFIX}vil)

   add_executable( bwm_batch_project_meshes_to_perspective_video bwm_batch_project_meshes_to_perspective_video.cxx)
   target_link_libraries( bwm_batch_project_meshes_to_perspective_video bwm bwm_algo ${VXL_LIB_PREFIX}vcl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vul)

   add_executable( bwm_bundler_to_video_site bwm_bundler_to_video_site.cxx)
   target_link_libraries( bwm_bundler_to_video_site bwm bwm_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vidl ${VXL_LIB_PREFIX}vcl bxml)

   add_executable( bwm_bundler4_to_video_site bwm_bundler4_to_video_site.cxx)
   target_link_libraries( bwm_bundler4_to_video_site bwm bwm_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vidl ${VXL_LIB_PREFIX}vcl bxml)

   add_executable( bwm_visualsfm_to_vpgl_cams bwm_visualsfm_to_vpgl_cams.cxx)
   target_link_libraries( bwm_visualsfm_to_vpgl_cams bwm bwm_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vidl ${VXL_LIB_PREFIX}vcl bxml)

   add_executable( bwm_nvm_avg_focal_length bwm_nvm_avg_focal_length.cxx)
   target_link_libraries( bwm_nvm_avg_focal_length bwm bwm_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vidl ${VXL_LIB_PREFIX}vcl bxml)

   add_executable( bwm_3d_site_transform  bwm_3d_site_transform.cxx)
   target_link_libraries( bwm_3d_site_transform bwm bwm_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vidl ${VXL_LIB_PREFIX}vcl bxml)

   add_executable( bwm_3d_site_transform_points  bwm_3d_site_transform_points.cxx)
   target_link_libraries( bwm_3d_site_transform_points bwm bwm_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vidl ${VXL_LIB_PREFIX}vcl bxml)

   add_executable( bwm_triangulate_2d_corrs  bwm_triangulate_2d_corrs.cxx)
   target_link_libraries( bwm_triangulate_2d_corrs bwm bwm_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vidl ${VXL_LIB_PREFIX}vcl bxml)

   add_executable( bwm_test_TFT  bwm_test_TFT_constraints.cxx)
   target_link_libraries( bwm_test_TFT bwm bwm_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vidl ${VXL_LIB_PREFIX}vcl bxml ipts mvl)

   add_executable( bwm_3d_fixed_transform  bwm_3d_fixed_transform.cxx)
   target_link_libraries( bwm_3d_fixed_transform bwm bwm_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vidl ${VXL_LIB_PREFIX}vcl bxml)

   add_executable( bwm_compute_orientation_variance bwm_compute_orientation_variance.cxx)
   target_link_libraries( bwm_compute_orientation_variance bwm bwm_algo ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vidl ${VXL_LIB_PREFIX}vcl bxml)

  endif()
 endif()
endif()

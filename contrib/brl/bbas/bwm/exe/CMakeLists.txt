# This is brl/bbas/bwm/exe/CMakeLists.txt
# World Modeling GUI executables

INCLUDE_DIRECTORIES( ${VTHREEP_INCLUDE_DIR}/rply )

IF(EXPAT_FOUND)
 IF(EXPATPP_FOUND)
  IF(SHAPELIB_FOUND)

   ADD_EXECUTABLE( bwm_batch_project bwm_batch_project.cxx)
   TARGET_LINK_LIBRARIES( bwm_batch_project bwm bwm_algo vcl vgl vpgl vul)

   ADD_EXECUTABLE( bwm_main bwm_main.cxx)
   TARGET_LINK_LIBRARIES( bwm_main bwm bwm_algo bwm_io bwm_reg vgui vil)

   ADD_EXECUTABLE( bwm_batch_project_meshes_to_perspective_video bwm_batch_project_meshes_to_perspective_video.cxx)
   TARGET_LINK_LIBRARIES( bwm_batch_project_meshes_to_perspective_video bwm bwm_algo vcl vgl vpgl vul)

   ADD_EXECUTABLE( bwm_bundler_to_video_site bwm_bundler_to_video_site.cxx)
   TARGET_LINK_LIBRARIES( bwm_bundler_to_video_site bwm bwm_algo vpgl vgl_algo vnl_algo vgl vnl vul vidl vcl bxml)

   ADD_EXECUTABLE( bwm_bundler4_to_video_site bwm_bundler4_to_video_site.cxx)
   TARGET_LINK_LIBRARIES( bwm_bundler4_to_video_site bwm bwm_algo vpgl vgl_algo vnl_algo vgl vnl vul vidl vcl bxml)

   ADD_EXECUTABLE( bwm_visualsfm_to_vpgl_cams bwm_visualsfm_to_vpgl_cams.cxx)
   TARGET_LINK_LIBRARIES( bwm_visualsfm_to_vpgl_cams bwm bwm_algo vpgl vgl_algo vnl_algo vgl vnl vul vidl vcl bxml)

   ADD_EXECUTABLE( bwm_nvm_avg_focal_length bwm_nvm_avg_focal_length.cxx)
   TARGET_LINK_LIBRARIES( bwm_nvm_avg_focal_length bwm bwm_algo vpgl vgl_algo vnl_algo vgl vnl vul vidl vcl bxml)

   ADD_EXECUTABLE( bwm_3d_site_transform  bwm_3d_site_transform.cxx)
   TARGET_LINK_LIBRARIES( bwm_3d_site_transform bwm bwm_algo vpgl vgl_algo vnl_algo vgl vnl vul vidl vcl bxml)
   
   ADD_EXECUTABLE( bwm_3d_site_transform_points  bwm_3d_site_transform_points.cxx)
   TARGET_LINK_LIBRARIES( bwm_3d_site_transform_points bwm bwm_algo vpgl vgl_algo vnl_algo vgl vnl vul vidl vcl bxml)

   ADD_EXECUTABLE( bwm_triangulate_2d_corrs  bwm_triangulate_2d_corrs.cxx)
   TARGET_LINK_LIBRARIES( bwm_triangulate_2d_corrs bwm bwm_algo vpgl vgl_algo vnl_algo vgl vnl vul vidl vcl bxml)

  ENDIF(SHAPELIB_FOUND)
 ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)

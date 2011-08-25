# This is brl/bbas/bwm/exe/CMakeLists.txt
# World Modeling GUI executables

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

  ENDIF(SHAPELIB_FOUND)
 ENDIF(EXPATPP_FOUND)
ENDIF(EXPAT_FOUND)

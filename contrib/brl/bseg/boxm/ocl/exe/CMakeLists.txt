# this is an executable to run a process for profiling purposes
ADD_EXECUTABLE( boxm_opencl_render_expected_process
  boxm_opencl_render_expected_process_exe.cxx
 )
TARGET_LINK_LIBRARIES( boxm_opencl_render_expected_process boxm_pro  boxm_ocl boxm_ocl_pro vil_pro vpgl_pro boxm_algo_rt_pro vul )


INCLUDE( ${BRL_MODULE_PATH}/FindGLEW.cmake )

IF(GLEW_FOUND)
 IF(VGUI_FOUND)
  INCLUDE_DIRECTORIES(${GLEW_INCLUDE_DIR} )
  LINK_DIRECTORIES(${OPENGL_LIBRARY_DIR})

  ADD_EXECUTABLE( boxm_ocl_render_view boxm_ocl_render_view.cxx)
  TARGET_LINK_LIBRARIES( boxm_ocl_render_view boxm_ocl  boxm_ocl_view vul vgui ${GLEW_LIBRARY} )

  ADD_EXECUTABLE( boxm_ocl_update_view boxm_ocl_update_view.cxx)
  TARGET_LINK_LIBRARIES( boxm_ocl_update_view boxm_ocl  boxm_ocl_view vul vgui ${GLEW_LIBRARY} )

  ADD_EXECUTABLE( boxm_opencl_change_detection_view boxm_opencl_change_detection_view.cxx)
  TARGET_LINK_LIBRARIES( boxm_opencl_change_detection_view boxm_ocl  boxm_ocl_view vul vgui ${GLEW_LIBRARY} )

  ADD_EXECUTABLE( boxm_ocl_rerender_view boxm_ocl_rerender_view.cxx)
  TARGET_LINK_LIBRARIES( boxm_ocl_rerender_view boxm_ocl  boxm_ocl_view vul vgui ${GLEW_LIBRARY} )

  ADD_EXECUTABLE( boxm_ocl_render_bit_view boxm_ocl_render_bit_view.cxx)
  TARGET_LINK_LIBRARIES( boxm_ocl_render_bit_view boxm_ocl  boxm_ocl_view vul vgui ${GLEW_LIBRARY} )

 ENDIF(VGUI_FOUND)
ENDIF(GLEW_FOUND)


ADD_EXECUTABLE( boxm_opencl_render_ocl_scene_exe boxm_opencl_render_ocl_scene_exe.cxx)
TARGET_LINK_LIBRARIES( boxm_opencl_render_ocl_scene_exe   boxm_ocl vil vpgl_algo  vul )

ADD_EXECUTABLE( boxm_build_foreground_distribution_exe boxm_build_foreground_distribution.cxx)
TARGET_LINK_LIBRARIES( boxm_build_foreground_distribution_exe vcl vul bsta_algo )

ADD_EXECUTABLE( boxm_ocl_rerender_adjacent_frames_exe boxm_ocl_rerender_adjacent_frames.cxx)
TARGET_LINK_LIBRARIES( boxm_ocl_rerender_adjacent_frames_exe boxm_ocl vil vpgl_algo  vul )

ADD_EXECUTABLE( boxm_ocl_change_detection_roc_exe boxm_ocl_change_detection_roc.cxx)
TARGET_LINK_LIBRARIES( boxm_ocl_change_detection_roc_exe  boxm_ocl vil vpgl_algo  vul )

ADD_EXECUTABLE( boxm_ocl_render_bit_exe boxm_ocl_render_bit_exe.cxx)
TARGET_LINK_LIBRARIES( boxm_ocl_render_bit_exe boxm_ocl vil vpgl_algo vul )


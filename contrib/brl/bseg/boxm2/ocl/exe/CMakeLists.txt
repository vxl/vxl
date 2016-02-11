
add_executable( boxm2_render_hemisphere boxm2_render_hemisphere.cxx )
target_link_libraries( boxm2_render_hemisphere boxm2 boxm2_basic boxm2_io boxm2_ocl_pro boxm2_ocl bocl vil_pro vpgl_pro vsph ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul)

add_executable( boxm2_export_scene boxm2_export_scene.cxx )
target_link_libraries( boxm2_export_scene boxm2 boxm2_basic boxm2_io sdet boxm2_ocl_pro boxm2_ocl bocl vil_pro vpgl_pro vsph ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul)

add_executable( boxm2_ocl_refine_scene boxm2_ocl_refine_scene.cxx )
target_link_libraries(  boxm2_ocl_refine_scene boxm2 boxm2_pro boxm2_basic boxm2_io boxm2_ocl boxm2_ocl_pro bocl vil_pro)

add_executable(boxm2_estimate_uncertain_viewpoints boxm2_estimate_uncertain_viewpoints.cxx )
target_link_libraries( boxm2_estimate_uncertain_viewpoints boxm2 boxm2_basic boxm2_io sdet boxm2_ocl_pro boxm2_ocl bocl vil_pro vpgl_pro vsph ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul)

include( ${BRL_MODULE_PATH}/FindGLEW.cmake )

if(GLEW_FOUND)
 if(VGUI_FOUND)
  include_directories(${GLEW_INCLUDE_DIR} )
  link_directories(${OPENGL_LIBRARY_DIR})

  add_executable( boxm2_ocl_render_view boxm2_ocl_render_view.cxx )
  target_link_libraries( boxm2_ocl_render_view boxm2 boxm2_view ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vgui vil_pro vpgl_pro ${GLEW_LIBRARY} )

  add_executable( boxm2_ocl_render_firstperson_view boxm2_ocl_render_firstperson_view.cxx )
  target_link_libraries( boxm2_ocl_render_firstperson_view boxm2 boxm2_view ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vgui vil_pro vpgl_pro ${GLEW_LIBRARY} )

 endif()
endif()


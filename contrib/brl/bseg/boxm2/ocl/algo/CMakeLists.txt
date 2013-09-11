#depends on OPENCL being found...
INCLUDE_DIRECTORIES( ${OPENCL_INCLUDE_PATH} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
SET(boxm2_ocl_algo_sources
    boxm2_ocl_render_expected_image_function.h  boxm2_ocl_render_expected_image_function.cxx
    boxm2_ocl_camera_converter.h                boxm2_ocl_camera_converter.cxx
    boxm2_ocl_cone_update_function.h            boxm2_ocl_cone_update_function.cxx
    boxm2_ocl_paint_batch.h                     boxm2_ocl_paint_batch.cxx
    boxm2_ocl_paint_online.h                    boxm2_ocl_paint_online.cxx
    boxm2_ocl_change_detection.h                boxm2_ocl_change_detection.cxx
    boxm2_ocl_update_color.h                    boxm2_ocl_update_color.cxx
    boxm2_ocl_refine.h                          boxm2_ocl_refine.cxx
    boxm2_ocl_render_scene_uncertainty_map.h    boxm2_ocl_render_scene_uncertainty_map.cxx
    boxm2_ocl_update.h                          boxm2_ocl_update.cxx
    boxm2_ocl_update_view_dep_app.h             boxm2_ocl_update_view_dep_app.cxx
    boxm2_ocl_update_view_dep_app_color.h       boxm2_ocl_update_view_dep_app_color.cxx
    boxm2_ocl_update_auxQ.h						boxm2_ocl_update_auxQ.cxx
    boxm2_ocl_update_sky.h                      boxm2_ocl_update_sky.cxx
   )

ADD_LIBRARY(boxm2_ocl_algo  ${boxm2_ocl_algo_sources})
TARGET_LINK_LIBRARIES(boxm2_ocl_algo boxm2_ocl vgl vil vul vnl_algo)

#tests
IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

INSTALL_TARGETS(/lib boxm2_ocl_algo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/boxm2/ocl/algo ${boxm2_ocl_algo_sources})

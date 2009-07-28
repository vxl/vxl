# contrib/brl/bseg/boxm/CMakeLists.txt
INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)

INCLUDE_DIRECTORIES( ${EXPAT_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/b3p/expatpp )

SET(boxm_sources
    boxm_block.txx                         boxm_block.h
    boxm_scene_base.cxx                    boxm_scene_base.h
    boxm_scene.txx                         boxm_scene.h
    boxm_sample.txx                        boxm_sample.h
    boxm_sample_multi_bin.txx              boxm_sample_multi_bin.h

    boxm_apm_traits.cxx                    boxm_apm_traits.h
    boxm_mog_grey_processor.cxx            boxm_mog_grey_processor.h
    boxm_block_vis_graph_node.h
    boxm_block_vis_graph_iterator.txx      boxm_block_vis_graph_iterator.h
    boxm_cell_vis_graph_iterator.txx       boxm_cell_vis_graph_iterator.h
    boxm_neighb_graph_node.h
    boxm_utils.cxx                         boxm_utils.h
    boxm_render_image.h                    boxm_render_image_triangle.h
    boxm_update_triangle.h
    boxm_refine.h
    boxm_save_block_raw.h
    boxm_quad_scan_iterator.h              boxm_quad_scan_iterator.cxx
    boxm_compute_volume_visibility.h
    boxm_triangle_interpolation_iterator.h boxm_triangle_interpolation_iterator.txx
    boxm_triangle_scan_iterator.h          boxm_triangle_scan_iterator.cxx
    boxm_raytrace_operations.h             boxm_raytrace_operations.cxx

    boxm_rational_camera_utils.h           boxm_rational_camera_utils.cxx
    boxm_upload_mesh.h
    boxm_fill_in_mesh.h
)

AUX_SOURCE_DIRECTORY(Templates boxm_sources)

ADD_LIBRARY(boxm  ${boxm_sources})

TARGET_LINK_LIBRARIES(boxm boct boxm_io vcl vgl vgl_io vgl_xio vbl vsl vpgl vpgl_algo vsol bgeo bsta bsta_algo vul vil imesh)

#processes
SUBDIRS(pro)

#io
SUBDIRS(io)

#tests
SUBDIRS(tests)

ENDIF(EXPAT_FOUND)

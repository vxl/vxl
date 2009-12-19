USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm

SOURCES += boxm_utils.cxx
SOURCES += boxm_mog_grey_processor.cxx
MANPAGE_SOURCES += boxm_cell_vis_graph_node.h
MANPAGE_SOURCES += boxm_block_vis_graph_iterator.txx
MANPAGE_SOURCES += boxm_block_vis_graph_node.h
MANPAGE_SOURCES += boxm_block.h
MANPAGE_SOURCES += boxm_neighb_graph_node.h
MANPAGE_SOURCES += boxm_scene.txx
MANPAGE_SOURCES += boxm_sample.h
MANPAGE_SOURCES += boxm_cell_vis_graph_iterator.h
MANPAGE_SOURCES += boxm_block.txx
MANPAGE_SOURCES += boxm_sample.txx
MANPAGE_SOURCES += boxm_cell_vis_graph_iterator.txx
MANPAGE_SOURCES += boxm_block_vis_graph_node.txx
SOURCES += boxm_apm_traits.cxx
SOURCES += boxm_scene_base.cxx
MANPAGE_SOURCES += boxm_render_image.h
MANPAGE_SOURCES += boxm_save_block_raw.h
MANPAGE_SOURCES += boxm_update.h
MANPAGE_SOURCES += boxm_refine.h
MANPAGE_SOURCES += boxm_compute_volume_visibility.h
SOURCES += boxm_quad_scan_iterator.cxx
MANPAGE_SOURCES += boxm_sample_multi_bin.txx
SOURCES += boxm_raytrace_operations.cxx
SOURCES += boxm_rational_camera_utils.cxx
SOURCES += boxm_triangle_scan_iterator.cxx
MANPAGE_SOURCES += boxm_triangle_interpolation_iterator.txx
MANPAGE_SOURCES += boxm_update_triangle.h
MANPAGE_SOURCES += boxm_render_image_triangle.h
MANPAGE_SOURCES += boxm_upload_mesh.h
MANPAGE_SOURCES += boxm_fill_in_mesh.h
SOURCES += boxm_simple_grey_processor.cxx
MANPAGE_SOURCES += boxm_update_sample.h
MANPAGE_SOURCES += boxm_vis_implicit_sample.h
MANPAGE_SOURCES += boxm_depth_image_triangle.h
MANPAGE_SOURCES += boxm_save_scene_raw.h
SOURCES += boxm_scene_parser.cxx
SOURCES += boxm_mob_grey_processor.cxx
MANPAGE_SOURCES += boxm_cell_data_traits.h

SUBDIRS += io pro opt algo

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

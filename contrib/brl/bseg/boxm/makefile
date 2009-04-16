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
MANPAGE_SOURCES += boxm_scene_base.h
MANPAGE_SOURCES += boxm_render_image.h

SUBDIRS += io pro

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

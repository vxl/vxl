USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm

SOURCES += dummy.cxx

MANPAGE_SOURCES += boxm_block_vis_graph_iterator.txx
MANPAGE_SOURCES += boxm_block_vis_graph_node.h
MANPAGE_SOURCES += boxm_cell_vis_graph_iterator.h
MANPAGE_SOURCES += boxm_seg_length_functor.h
MANPAGE_SOURCES += boxm_raytrace_function.h
MANPAGE_SOURCES += boxm_cell_vis_graph_iterator.txx
MANPAGE_SOURCES += boxm_num_rays_functor.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

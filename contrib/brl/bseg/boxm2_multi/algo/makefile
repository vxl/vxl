USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2_multi-algo

SOURCES += boxm2_multi_render.cxx
SOURCES += boxm2_multi_store_aux.cxx
SOURCES += boxm2_multi_pre_vis_inf.cxx
SOURCES += boxm2_multi_update_cell.cxx
SOURCES += boxm2_multi_update.cxx
SOURCES += boxm2_multi_refine.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

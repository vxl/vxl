USES += contrib/brl contrib/brl/bbas contrib/brl/bpro core

LIBRARY := vil_pro
VERSION := 1.0

SOURCES += vil_io_image_view_base.cxx
SOURCES += vil_load_image_view_process.cxx
SOURCES += vil_save_image_view_process.cxx
SOURCES += vil_binary_image_op_process.cxx
SOURCES += vil_map_image_process.cxx
MANPAGE_SOURCES += vil_math_functors.h
SOURCES += vil_rgbi_to_grey_process.cxx
SOURCES += vil_convert_to_n_planes_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

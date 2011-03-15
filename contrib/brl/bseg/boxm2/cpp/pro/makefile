USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2_cpp_pro

MANPAGE_SOURCES += boxm2_cpp_process_base.h
SOURCES += boxm2_cpp_change_detection_process.cxx
SOURCES += boxm2_cpp_update_process.cxx
SOURCES += boxm2_cpp_render_process.cxx
SOURCES += boxm2_cpp_refine_process.cxx
SOURCES += boxm2_cpp_change_detection_uncertainity_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

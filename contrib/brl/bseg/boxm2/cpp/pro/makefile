USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2_cpp_pro

SOURCES += boxm2_cpp_register.cxx

MANPAGE_SOURCES += boxm2_cpp_processes.h
SOURCES += processes/boxm2_cpp_refine_process2.cxx
SOURCES += processes/boxm2_cpp_render_expected_image_process.cxx
SOURCES += processes/boxm2_cpp_update_image_process.cxx
SOURCES += processes/boxm2_cpp_change_detection_process2.cxx
SOURCES += processes/boxm2_cpp_render_cone_expected_image_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

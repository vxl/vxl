USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := bstm_ocl_pro

SOURCES += bstm_ocl_register.cxx
MANPAGE_SOURCES += bstm_ocl_processes.h

SOURCES += processes/bstm_ocl_render_expected_image_process.cxx
SOURCES += processes/bstm_ocl_render_gl_expected_color_process.cxx
SOURCES += processes/bstm_ocl_clear_cache_process.cxx
SOURCES += processes/bstm_ocl_create_cache_process.cxx
SOURCES += processes/bstm_ocl_render_gl_expected_image_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

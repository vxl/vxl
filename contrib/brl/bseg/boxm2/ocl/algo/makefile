USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2_ocl_algo

SOURCES += boxm2_ocl_render_expected_image_function.cxx
SOURCES += boxm2_ocl_camera_converter.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2_cpp_algo

SOURCES += boxm2_mog3_grey_processor.cxx
MANPAGE_SOURCES += boxm2_refine_block_function.h
MANPAGE_SOURCES += boxm2_render_exp_image_functor.h
MANPAGE_SOURCES += boxm2_cast_ray_function.h
MANPAGE_SOURCES += boxm2_data_serial_iterator.h
MANPAGE_SOURCES += boxm2_update_image_functor.h
MANPAGE_SOURCES += boxm2_change_detection_functor.h
SOURCES += boxm2_refine_block_function.cxx
SOURCES += boxm2_render_exp_image_functor.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

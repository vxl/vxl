USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm-ocl

SOURCES += boxm_ocl_utils.cxx
MANPAGE_SOURCES += boxm_ocl_utils.txx
SOURCES += octree_library_functions.cl
MANPAGE_SOURCES += boxm_ray_trace_manager.txx
MANPAGE_SOURCES += boxm_ocl_render_expected.h
SOURCES += expected_functor.cl
SOURCES += ray_trace_main.cl
SOURCES += ray_stack_trace_main.cl
MANPAGE_SOURCES += boxm_stack_ray_trace_manager.txx
MANPAGE_SOURCES += boxm_stat_manager.txx
SOURCES += refine_main.cl
SOURCES += ray_bundle_library_functions.cl
MANPAGE_SOURCES += boxm_ocl_refine_scene.h
MANPAGE_SOURCES += boxm_ray_bundle_trace_manager.txx
MANPAGE_SOURCES += boxm_refine_manager.txx
SOURCES += update_main.cl
MANPAGE_SOURCES += boxm_render_image_manager.txx

SUBDIRS += pro exe

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

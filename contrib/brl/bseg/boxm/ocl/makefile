USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm-ocl

SOURCES += boxm_ocl_utils.cxx
MANPAGE_SOURCES += boxm_ocl_utils.txx
MANPAGE_SOURCES += boxm_ray_trace_manager.txx
MANPAGE_SOURCES += boxm_ocl_render_expected.h
MANPAGE_SOURCES += boxm_stat_manager.txx
MANPAGE_SOURCES += boxm_ocl_refine_scene.h
MANPAGE_SOURCES += boxm_ray_bundle_trace_manager.txx
MANPAGE_SOURCES += boxm_refine_manager.txx
MANPAGE_SOURCES += boxm_render_image_manager.txx
SOURCES += boxm_online_update_manager.cxx
SOURCES += boxm_refine_scene_manager.cxx
SOURCES += boxm_ocl_scene.cxx
SOURCES += boxm_render_ocl_scene_manager.cxx
SOURCES += boxm_update_ocl_scene_manager.cxx
SOURCES += boxm_ocl_render_expected.cxx
SOURCES += boxm_change_detection_ocl_scene_manager.cxx
MANPAGE_SOURCES += boxm_ocl_camera_manager.h
SOURCES += boxm_ocl_camera_manager.cxx
SOURCES += boxm_render_bit_scene_manager.cxx
SOURCES += boxm_ocl_bit_scene.cxx
SOURCES += boxm_re_render_ocl_scene_manager.cxx
SOURCES += boxm_render_probe_manager.cxx
MANPAGE_SOURCES += boxm_render_probe_manager_sptr.h

SOURCES += cl/octree_library_functions.cl
SOURCES += cl/ray_trace_main.cl
SOURCES += cl/update_main.cl
SOURCES += cl/refine_main.cl
SOURCES += cl/ray_bundle_library_functions.cl
SOURCES += cl/expected_functor.cl

SUBDIRS += pro exe view

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

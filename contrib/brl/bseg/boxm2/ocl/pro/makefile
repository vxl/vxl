USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2_ocl_pro

SOURCES += processes/boxm2_ocl_change_detection_process.cxx

SOURCES += boxm2_ocl_register.cxx
MANPAGE_SOURCES += boxm2_ocl_processes.h
SOURCES += processes/boxm2_ocl_render_expected_image_process.cxx
SOURCES += processes/boxm2_create_opencl_cache_process.cxx
SOURCES += processes/boxm2_ocl_query_hist_data_process.cxx
SOURCES += processes/boxm2_ocl_update_histogram_process.cxx
SOURCES += processes/boxm2_ocl_batch_probability_process.cxx
SOURCES += processes/boxm2_ocl_query_cell_data_process.cxx
SOURCES += processes/boxm2_ocl_render_expected_depth_process.cxx
SOURCES += processes/boxm2_ocl_update_process.cxx
SOURCES += processes/boxm2_ocl_refine_process.cxx
SOURCES += processes/boxm2_ocl_render_gl_expected_image_process.cxx
SOURCES += processes/boxm2_ocl_render_expected_height_map_process.cxx
SOURCES += processes/boxm2_ocl_update_color_process.cxx
SOURCES += processes/boxm2_ocl_render_expected_color_process.cxx
SOURCES += processes/boxm2_ocl_render_gl_expected_color_process.cxx
SOURCES += processes/boxm2_ocl_convert_float_image_to_rgba_process.cxx
SOURCES += processes/boxm2_ocl_probability_of_image_process.cxx
SOURCES += processes/boxm2_ocl_probability_of_image_gl_process.cxx
SOURCES += processes/boxm2_ocl_render_cone_expected_image_process.cxx
SOURCES += processes/boxm2_ocl_filter_process.cxx
SOURCES += processes/boxm2_clear_opencl_cache_process.cxx
SOURCES += processes/boxm2_ocl_cone_update_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

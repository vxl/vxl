USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2_ocl_pro

MANPAGE_SOURCES += boxm2_opencl_process_base.h
SOURCES += boxm2_opencl_update_process.cxx
SOURCES += boxm2_opencl_render_depth_process.cxx
SOURCES += boxm2_opencl_change_detection_process.cxx
SOURCES += boxm2_opencl_render_process.cxx
SOURCES += boxm2_opencl_refine_process.cxx
SOURCES += boxm2_opencl_render_rgb_process.cxx
SOURCES += boxm2_opencl_update_rgb_process.cxx
SOURCES += boxm2_opencl_batch_update_process.cxx

SOURCES += boxm2_ocl_register.cxx
MANPAGE_SOURCES += boxm2_ocl_processes.h
SOURCES += processes/boxm2_ocl_render_expected_image_process.cxx
SOURCES += processes/boxm2_create_opencl_cache_process.cxx
SOURCES += processes/boxm2_ocl_query_hist_data_process.cxx
SOURCES += processes/boxm2_ocl_update_histogram_process.cxx
SOURCES += processes/boxm2_ocl_batch_probability_process.cxx
SOURCES += processes/boxm2_ocl_query_cell_data_process.cxx
SOURCES += processes/boxm2_ocl_render_expected_depth_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

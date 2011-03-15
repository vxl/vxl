USES += contrib/brl contrib/brl/bseg core

LIBRARY := bvpl_pro

SOURCES += bvpl_register.cxx
MANPAGE_SOURCES += bvpl_processes.h

SOURCES += processes/bvpl_neighborhood_operator_process.cxx
SOURCES += processes/bvpl_vector_operator_process.cxx
SOURCES += processes/bvpl_convert_direction_to_hue_process.cxx
SOURCES += processes/bvpl_compare_surface_and_normal_process.cxx
SOURCES += processes/bvpl_susan_opinion_operator_process.cxx
SOURCES += processes/bvpl_kernel_id_to_axis_process.cxx
SOURCES += processes/bvpl_non_max_suppression_process.cxx
SOURCES += processes/bvpl_visualize_response_process.cxx
SOURCES += processes/bvpl_convert_id_to_hue_process.cxx
SOURCES += processes/bvpl_extract_top_response_process.cxx
SOURCES += processes/bvpl_suppress_and_combine_process.cxx
SOURCES += processes/util/bvpl_visualize_corner_pairs_process.cxx
SOURCES += processes/bvpl_operate_ocp_and_app_process.cxx
SOURCES += processes/util/bvpl_find_90_degrees_corner_pairs_process.cxx
SOURCES += processes/util/bvpl_pair_to_float_process.cxx
SOURCES += processes/util/bvpl_find_pairs_process.cxx
SOURCES += processes/util/bvpl_convert_pair_to_hue_process.cxx
SOURCES += processes/util/bvpl_load_pair_grid_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

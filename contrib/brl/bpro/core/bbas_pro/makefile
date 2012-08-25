USES += contrib/brl contrib/brl/bbas contrib/brl/bpro core

LIBRARY := bbas_pro
VERSION := 1.0

SOURCES += bbas_register.cxx
MANPAGE_SOURCES += bbas_processes.h

SOURCES += bbas_1d_array_float.cxx
SOURCES += bbas_1d_array_string.cxx
SOURCES += bbas_1d_array_int.cxx
SOURCES += bbas_1d_array_unsigned.cxx

MANPAGE_SOURCES += bbas_1d_array_string_sptr.h

SOURCES += processes/bbas_atmospheric_corr_process.cxx
SOURCES += processes/bbas_string_array_process.cxx
SOURCES += processes/bsta_clone_joint_hist_3d_process.cxx
SOURCES += processes/bsta_joint_hist_3d_vrml_process.cxx
SOURCES += processes/bsta_load_joint_hist_3d_process.cxx
SOURCES += processes/bsta_save_joint_hist_3d_process.cxx
SOURCES += processes/bbas_merge_string_array_process.cxx
SOURCES += processes/bbas_camera_angles_process.cxx
SOURCES += processes/bbas_remove_from_db_process.cxx
SOURCES += processes/bbas_estimate_irradiance_process.cxx
SOURCES += processes/imesh_ply_bbox_process.cxx
SOURCES += processes/bsl_expected_image_process.cxx
SOURCES += processes/bsl_fusion_process.cxx
SOURCES += processes/bpgl_depth_map_processes.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

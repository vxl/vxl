USES += contrib/brl contrib/brl/bbas core

LIBRARY := brad_pro
VERSION := 1.0

SOURCES += brad_register.cxx
MANPAGE_SOURCES += brad_processes.h

SOURCES += processes/brad_estimate_shadow_model_process.cxx
SOURCES += processes/brad_estimate_phongs_model_process.cxx
SOURCES += processes/brad_display_sun_index_process.cxx
SOURCES += processes/brad_save_sun_index_process.cxx
SOURCES += processes/brad_sun_dir_bin_process.cxx
SOURCES += processes/brad_update_joint_hist_3d_process.cxx
SOURCES += processes/brad_describe_eigenspace_process.cxx
SOURCES += processes/brad_save_eigenspace_process.cxx
SOURCES += processes/brad_create_eigenspace_process.cxx
SOURCES += processes/brad_train_histograms_process.cxx
SOURCES += processes/brad_compute_eigenspace_process.cxx
SOURCES += processes/brad_load_eigenspace_process.cxx
SOURCES += processes/brad_classify_image_process.cxx
SOURCES += processes/brad_classify_image_pixel_process.cxx
SOURCES += processes/brad_eigenimage_pixel_process.cxx
SOURCES += processes/brad_estimate_synoptic_function_1d_process.cxx
SOURCES += processes/brad_estimate_empty_process.cxx
SOURCES += processes/brad_estimate_radiance_values_process.cxx
SOURCES += processes/brad_nitf_abs_radiometric_calibration_process.cxx
SOURCES += processes/brad_nitf_read_metadata_process.cxx
SOURCES += processes/brad_estimate_atmospheric_parameters_process.cxx
SOURCES += processes/brad_estimate_reflectance_process.cxx
SOURCES += processes/brad_save_atmospheric_parameters_process.cxx
SOURCES += processes/brad_load_atmospheric_parameters_process.cxx
SOURCES += processes/brad_save_image_metadata_process.cxx
SOURCES += processes/brad_load_image_metadata_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

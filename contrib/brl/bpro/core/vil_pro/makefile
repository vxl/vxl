USES += contrib/brl contrib/brl/bbas contrib/brl/bpro core

LIBRARY := vil_pro
VERSION := 1.0

MANPAGE_SOURCES += vil_math_functors.h
SOURCES += vil_register.cxx

MANPAGE_SOURCES += vil_processes.h
SOURCES += processes/vil_load_image_view_process.cxx
SOURCES += processes/vil_save_image_view_process.cxx
SOURCES += processes/vil_binary_image_op_process.cxx
SOURCES += processes/vil_map_image_process.cxx
SOURCES += processes/vil_rgbi_to_grey_process.cxx
SOURCES += processes/vil_convert_to_n_planes_process.cxx
SOURCES += processes/vil_combine_grey_images_process.cxx
SOURCES += processes/vil_load_from_ascii_process.cxx
SOURCES += processes/vil_gaussian_process.cxx
SOURCES += processes/vil_gradient_process.cxx
SOURCES += processes/vil_stretch_image_process.cxx
SOURCES += processes/vil_load_image_view_binary_process.cxx
SOURCES += processes/vil_save_image_view_binary_process.cxx
SOURCES += processes/vil_map_image_binary_process.cxx
SOURCES += processes/vil_image_pair_process.cxx
SOURCES += processes/vil_crop_image_process.cxx
SOURCES += processes/vil_threshold_image_process.cxx
SOURCES += processes/vil_image_size_process.cxx
SOURCES += processes/vil_two_planes_composite_process.cxx
SOURCES += processes/vil_filter_image_process.cxx
SOURCES += processes/vil_resample_process.cxx
SOURCES += processes/bil_compass_edge_detector_process.cxx
SOURCES += processes/vil_blob_detection_process.cxx
SOURCES += processes/vil_pixelwise_roc_process.cxx
SOURCES += processes/vil_rgb_to_grey_process.cxx
SOURCES += processes/vil_nitf_date_time_process.cxx
SOURCES += processes/vil_convert_pixel_type_process.cxx
SOURCES += processes/vil_gradient_angle_process.cxx
SOURCES += processes/vil_image_ssd_process.cxx
SOURCES += processes/vil_image_mean_process.cxx
SOURCES += processes/vil_image_range_process.cxx
SOURCES += processes/bil_create_raw_image_istream_process.cxx
SOURCES += processes/vil_median_filter_process.cxx
SOURCES += processes/vil_pixel_value_process.cxx
SOURCES += processes/vil_init_float_image_process.cxx
SOURCES += processes/vil_nitf_bits_per_pixel_process.cxx
SOURCES += processes/vil_scale_and_offset_values_process.cxx
SOURCES += processes/vil_set_float_image_pixel_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

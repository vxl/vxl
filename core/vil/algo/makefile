USES += core

LIBRARY := vil-algo
VERSION := 1.0

SOURCES += vil_sobel_1x3.cxx
MANPAGE_SOURCES += vil_sobel_1x3.txx
SOURCES += vil_sobel_3x3.cxx
MANPAGE_SOURCES += vil_sobel_3x3.txx
SOURCES += vil_gauss_filter.cxx
MANPAGE_SOURCES += vil_gauss_filter.txx
SOURCES += vil_gauss_reduce.cxx
MANPAGE_SOURCES += vil_gauss_reduce.txx
MANPAGE_SOURCES += vil_convolve_1d.h
MANPAGE_SOURCES += vil_convolve_2d.h
MANPAGE_SOURCES += vil_correlate_1d.h
MANPAGE_SOURCES += vil_correlate_2d.h
MANPAGE_SOURCES += vil_exp_filter_1d.h
SOURCES += vil_binary_erode.cxx
SOURCES += vil_binary_dilate.cxx
SOURCES += vil_structuring_element.cxx
MANPAGE_SOURCES += vil_binary_closing.h
MANPAGE_SOURCES += vil_binary_opening.h
MANPAGE_SOURCES += vil_greyscale_dilate.txx
MANPAGE_SOURCES += vil_greyscale_erode.txx
MANPAGE_SOURCES += vil_greyscale_closing.h
MANPAGE_SOURCES += vil_greyscale_opening.h
MANPAGE_SOURCES += vil_median.txx
MANPAGE_SOURCES += vil_normalised_correlation_2d.h
MANPAGE_SOURCES += vil_exp_grad_filter_1d.h
MANPAGE_SOURCES += vil_line_filter.txx
MANPAGE_SOURCES += vil_threshold.txx
MANPAGE_SOURCES += vil_grid_merge.h
MANPAGE_SOURCES += vil_find_4con_boundary.h
MANPAGE_SOURCES += vil_fft.txx
MANPAGE_SOURCES += vil_exp_filter_2d.h
MANPAGE_SOURCES += vil_suppress_non_max.h
MANPAGE_SOURCES += vil_suppress_non_max_edges.txx
SOURCES += vil_histogram.cxx
SOURCES += vil_histogram_equalise.cxx
SOURCES += vil_distance_transform.cxx
SOURCES += vil_blob_finder.cxx
SOURCES += vil_trace_4con_boundary.cxx
SOURCES += vil_trace_8con_boundary.cxx
SOURCES += vil_corners.cxx
MANPAGE_SOURCES += vil_find_peaks.h
MANPAGE_SOURCES += vil_region_finder.txx
MANPAGE_SOURCES += vil_cartesian_differential_invariants.txx
MANPAGE_SOURCES += vil_dog_filter_5tap.h
MANPAGE_SOURCES += vil_dog_pyramid.h
MANPAGE_SOURCES += vil_tile_images.h
SOURCES += vil_orientations.cxx
SOURCES += vil_colour_space.cxx
MANPAGE_SOURCES += vil_abs_shuffle_distance.txx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

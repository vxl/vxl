# video processor library
USES += contrib/brl contrib/gel contrib/brl/bbas contrib/brl/bseg core

LIBRARY := vpro
VERSION := 1.0

IMMEDIATE_LIBS := sdet brip gevd vtol vsol vidl_vil1 vil1 vul vbl

SOURCES += vpro_video_process.cxx
MANPAGE_SOURCES += vpro_video_process_sptr.h
SOURCES += vpro_region_process.cxx
SOURCES += vpro_capture_process.cxx
SOURCES += vpro_frame_diff_process.cxx
SOURCES += vpro_motion_process.cxx
SOURCES += vpro_motion_params.cxx
SOURCES += vpro_lucas_kanade_process.cxx
SOURCES += vpro_epipolar_space_process.cxx
SOURCES += vpro_edge_process.cxx
SOURCES += vpro_harris_corner_process.cxx
SOURCES += vpro_line_fit_process.cxx
SOURCES += vpro_edge_line_process.cxx
SOURCES += vpro_grid_finder_process.cxx
SOURCES += vpro_basis_generator_process.cxx
SOURCES += vpro_fourier_params.cxx
SOURCES += vpro_fourier_process.cxx
SOURCES += vpro_frame_diff_params.cxx
SOURCES += vpro_spatial_filter_params.cxx
SOURCES += vpro_spatial_filter_process.cxx
SOURCES += vpro_ihs_process.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

#   DESCRIPTION:
#
#   This is the vsrl (dense matcher) library.

USES += contrib/gel contrib/brl/bseg contrib/tbl contrib/oxl contrib/rpl core

IMMEDIATE_LIBS := -lvtol -lvdgl -lvgui -lvepl -lvgl -lvil1 -lvnl -lvgel -lrsdl

LIBRARY := vsrl
VERSION := 1.0

MANPAGE_SOURCES += vsrl_token.h
SOURCES += vsrl_null_token.cxx
SOURCES += vsrl_intensity_token.cxx
SOURCES += vsrl_dynamic_program.cxx
SOURCES += vsrl_raster_dp_setup.cxx
SOURCES += vsrl_image_correlation.cxx
SOURCES += vsrl_window_accumulator.cxx
MANPAGE_SOURCES += vsrl_dense_matcher.h
SOURCES += vsrl_stereo_dense_matcher.cxx
SOURCES += vsrl_results_dense_matcher.cxx
SOURCES += vsrl_3d_output.cxx
SOURCES += vsrl_diffusion.cxx
SOURCES += vsrl_step_diffusion.cxx
SOURCES += vsrl_saliency_diffusion.cxx
SOURCES += vsrl_token_saliency.cxx
SOURCES += vsrl_parameters.cxx
SOURCES += vsrl_manager.cxx
SOURCES += vsrl_point_picker.cxx
MANPAGE_SOURCES += vsrl_point_picker_sptr.h
SOURCES += vsrl_menus.cxx
SOURCES += vsrl_region_disparity.cxx

DEFINES += -DBUILDING_VSRL_DLL

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

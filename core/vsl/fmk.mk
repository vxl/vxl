include ${fmk_ROOT}/fmk_params.mk

fmk_USES := vxl

fmk_LIBRARY := vsl
fmk_VERSION := 1.0

# convolution
fmk_LIB_SOURCES += vsl_kernel.cxx

# Harris corner detector
fmk_LIB_SOURCES += vsl_harris_params.cxx
fmk_LIB_SOURCES += vsl_harris.cxx
fmk_LIB_SOURCES += vsl_roi_window.cxx
fmk_LIB_SOURCES += vsl_convolve.cxx
fmk_LIB_SOURCES += internals/droid.cxx

#----------------------------------------

# canny utilities
fmk_LIB_SOURCES += vsl_canny_port.cxx
fmk_LIB_SOURCES += vsl_canny_base.cxx
fmk_LIB_SOURCES += vsl_canny_smooth.cxx
fmk_LIB_SOURCES += vsl_canny_gradient.cxx
fmk_LIB_SOURCES += vsl_canny_nms.cxx
fmk_LIB_SOURCES += vsl_chamfer.cxx
fmk_LIB_SOURCES += internals/vsl_reorder_chain.cxx
# rothwell canny 1
fmk_LIB_SOURCES += vsl_canny_rothwell_params.cxx
fmk_LIB_SOURCES += vsl_canny_rothwell.cxx
# oxford canny
fmk_LIB_SOURCES += vsl_canny_ox_params.cxx
fmk_LIB_SOURCES += vsl_canny_ox.cxx
# rothwell canny 2
fmk_LIB_SOURCES += vsl_edge_detector_params.cxx
fmk_LIB_SOURCES += vsl_edge_detector.cxx

# easy
fmk_LIB_SOURCES += vsl_easy_canny.cxx

#
fmk_LIB_SOURCES += fsm_ortho_regress.cxx
fmk_LIB_SOURCES += vsl_OrthogRegress.cxx
fmk_LIB_SOURCES += vsl_topology.cxx
fmk_LIB_SOURCES += vsl_save_topology.cxx
fmk_LIB_SOURCES += vsl_load_topology.cxx
fmk_LIB_SOURCES += $(wildcard vsl_fit_lines*.cxx)

fmk_FLAGS_vsl_convolve.cxx += $(fmk_optimize)
fmk_FLAGS_internals/vsl_droid.cxx += $(fmk_optimize)
fmk_FLAGS_vsl_harris.cxx += $(fmk_optimize)

ifndef NOTEST
#fmk_SUBDIRS += tests 
#fmk_SUBDIRS += examples
endif

include ${fmk_ROOT}/fmk_rules.mk

USES += contrib/oxl core

LIBRARY := osl
VERSION := 1.0

IMMEDIATE_LIBS = -losl -lvil -lvnl-algo -lnetlib -lvnl -lvbl

# convolution
SOURCES += osl_kernel.cxx
MANPAGE_SOURCES += osl_1d_half_kernel.txx

#----------------------------------------
# Corner detection

# Harris corner detector
SOURCES += osl_harris_params.cxx
SOURCES += osl_harris.cxx
SOURCES += osl_roi_window.cxx
SOURCES += osl_convolve.cxx
SOURCES += internals/droid.cxx

#----------------------------------------
# Edge detection

# canny data structures.
SOURCES += osl_edgel_chain.cxx
SOURCES += osl_topology.cxx
SOURCES += osl_vertex.cxx
SOURCES += osl_edge.cxx
SOURCES += osl_save_topology.cxx
SOURCES += osl_load_topology.cxx

# canny utilities
SOURCES += osl_canny_port.cxx
SOURCES += osl_canny_base.cxx
SOURCES += osl_canny_smooth.cxx
MANPAGE_SOURCES += osl_canny_smooth.txx
SOURCES += osl_canny_gradient.cxx
SOURCES += osl_canny_nms.cxx
SOURCES += osl_chamfer.cxx
SOURCES += internals/osl_reorder_chain.cxx
# rothwell canny 1
SOURCES += osl_canny_rothwell_params.cxx
SOURCES += osl_canny_rothwell.cxx
# oxford canny
SOURCES += osl_canny_ox_params.cxx
SOURCES += osl_canny_ox.cxx
# rothwell canny 2
SOURCES += osl_edge_detector_params.cxx
SOURCES += osl_edge_detector.cxx

# easy
SOURCES += osl_easy_canny.cxx

#----------------------------------------
# Fitting

SOURCES += osl_ortho_regress.cxx
SOURCES += osl_OrthogRegress.cxx # to be deprecated.
SOURCES += osl_fit_lines.cxx
SOURCES += osl_fit_lines_params.cxx
SOURCES += osl_fit_circle.cxx
SOURCES += osl_break_edge.cxx

MANPAGE_SOURCES += osl_hacks.h

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests 
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

FLAGS_osl_harris.cxx := $(optimize)
FLAGS_osl_convolve.cxx := $(optimize)
FLAGS_internals/droid.cxx := $(optimize)

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES := vxl gel

IMMEDIATE_LIBS := -lvsol -lvgl -lvbl -lvnl -lvcl

LIBRARY := vdgl
VERSION := 1.0

# background structure
SOURCES += vdgl_edgel.cxx
SOURCES += vdgl_edgel_chain.cxx
SOURCES += vdgl_interpolator.cxx

# interpolators
SOURCES += vdgl_interpolator_linear.cxx

# and outside interface
SOURCES += vdgl_digital_curve.cxx

SOURCES += vdgl_fit_line.cxx

ifndef NOTEST
SUBDIRS += tests examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

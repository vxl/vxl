#
# crossge@crd.ge.com
#
#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES := vxl gel

IMMEDIATE_LIBS := -lvdgl -lvtol -lvil -lvnl -lvbl

LIBRARY := gevd
VERSION := 1.0

SOURCES += gevd_detector.cxx
SOURCES += gevd_detector_params.cxx
SOURCES += gevd_param_mixin.cxx
SOURCES += gevd_status_mixin.cxx
SOURCES += gevd_memory_mixin.cxx
SOURCES += gevd_bufferxy.cxx
SOURCES += gevd_float_operators.cxx
SOURCES += gevd_noise.cxx
SOURCES += gevd_step.cxx
SOURCES += gevd_contour.cxx
SOURCES += gevd_clean_edgels.cxx
MANPAGE_SOURCES += gevd_pixel.h
MANPAGE_SOURCES += gevd_xpixel.h

ifeq "$(TJ_GCC30)$(optimize)" "1-O2"
# gcc 3.0 needs too much virtual memory with -O2
FLAGS_gevd_contour.cxx += -O1
endif

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

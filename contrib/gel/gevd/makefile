#
# crossge@crd.ge.com
#
#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES := vxl

IMMEDIATE_LIBS := vxl

LIBRARY := gevd
VERSION := 1.0

TRY_TIFF := 1
TRY_JPEG := 1
TRY_PNG  := 1

# in order of dependence
SOURCES += gevd_param_mixin.cxx
SOURCES += gevd_detector_params.cxx
SOURCES += gevd_detector.cxx
SOURCES += gevd_contour.cxx
SOURCES += gevd_status_mixin.cxx
SOURCES += gevd_memory_mixin.cxx
SOURCES += gevd_bufferxy.cxx
SOURCES += gevd_float_operators.cxx
SOURCES += gevd_noise.cxx
SOURCES += gevd_step.cxx


########################
USES += v3p
LDLIBS += -lvnl -lvil -lvcl -lvbl
PROGRAM := gevd
SOURCES += main.cc
######################## 

INCDIRS = ..

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

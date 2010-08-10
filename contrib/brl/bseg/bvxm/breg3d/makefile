USES += contrib/brl contrib/brl/bseg contrib/brl/bseg/bvxm core

LIBRARY := breg3d
VERSION := 1.0

SOURCES += breg3d_lm_direct_homography_generator.cxx
SOURCES += breg3d_gdbicp_homography_generator.cxx
SOURCES += breg3d_ekf_camera_optimizer.cxx
SOURCES += breg3d_ekf_camera_optimizer_state.cxx
MANPAGE_SOURCES += breg3d_homography_generator.h

SUBDIRS += pro

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

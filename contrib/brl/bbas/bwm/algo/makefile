USES += contrib/brl contrib/brl/bbas core

LIBRARY := bwm_algo
VERSION := 1.0

ifeq ($(HAS_BGUI3D),1)
SOURCES += bwm_algo.cxx
SOURCES += bwm_image_processor.cxx
SOURCES += bwm_lidar_algo.cxx
SOURCES += bwm_plane_fitting_lsf.cxx
SOURCES += bwm_rat_proj_camera.cxx
SOURCES += bwm_soview2D_cross.cxx
MANPAGE_SOURCES += bwm_soview2D_vertex.h
SOURCES += bwm_utils.cxx
SOURCES += bwm_utm_lat_lon.cxx
endif

################################
include ${VXLROOT}/config/top.mk
################################

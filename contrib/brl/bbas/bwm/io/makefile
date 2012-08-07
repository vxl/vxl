USES += contrib/brl contrib/brl/bbas core
USE_EXPAT := 1

LIBRARY := bwm_io
VERSION := 1.0

SOURCES += bwm_io_config_parser.cxx
MANPAGE_SOURCES += bwm_io_structs.h
SOURCES += bwm_site.cxx
MANPAGE_SOURCES += bwm_site_sptr.h
SOURCES += bwm_io_kml_parser.cxx
SOURCES += bwm_io_kml_camera.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

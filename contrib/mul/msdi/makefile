USES += contrib/mul core

LIBRARY := msdi
VERSION := 1.0

SOURCES += msdi_marked_images_from_files.cxx
SOURCES += msdi_array_of_marked_images.cxx
SOURCES += msdi_reflected_marked_images.cxx
MANPAGE_SOURCES += msdi_marked_images.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

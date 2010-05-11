USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm

SOURCES += dummy.cxx

MANPAGE_SOURCES += boxm_update_triangle.h
MANPAGE_SOURCES += boxm_compute_volume_visibility.h
MANPAGE_SOURCES += boxm_render_image_triangle.h
MANPAGE_SOURCES += boxm_update.h
MANPAGE_SOURCES += boxm_render_image.h
MANPAGE_SOURCES += boxm_depth_image_triangle.h

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

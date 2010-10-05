USES := # nothing

LIBRARY := shapelib

SOURCES += dbfopen.c
SOURCES += shpopen.c
SOURCES += shptree.c
MANPAGE_SOURCES += shapefil.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

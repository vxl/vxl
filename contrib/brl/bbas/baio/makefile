USES += contrib/brl contrib/brl/bbas core

LIBRARY := baio
VERSION := 1.0

MANPAGE_SOURCES += baio.h
ifdef UNIX
SOURCES += baio_unix.cxx
endif
ifdef WINDOWS
SOURCES += baio_windows.cxx
endif
ifdef OSX
SOURCES += baio_osx.cxx
endif

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

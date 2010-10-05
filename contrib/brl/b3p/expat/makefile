USES := # nothing

LIBRARY := expat
VERSION := 1.95.2

SOURCES += xmlparse.c
SOURCES += xmlrole.c
SOURCES += xmltok.c
MANPAGE_SOURCES += ascii.h
MANPAGE_SOURCES += asciitab.h
MANPAGE_SOURCES += expat.h
MANPAGE_SOURCES += iasciitab.h
MANPAGE_SOURCES += latin1tab.h
MANPAGE_SOURCES += nametab.h
MANPAGE_SOURCES += utf8tab.h
MANPAGE_SOURCES += xmltok_impl.h
MANPAGE_SOURCES += cm_expat_mangle.h

MANPAGE_SOURCES += expatConfig.h.in expatConfig.h
MANPAGE_SOURCES += expatDllConfig.h.in expatDllConfig.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

ifeq ($(OS),win32)
APP_CFLAGS += -DXML_STATIC -DCOMPILED_FROM_DSP
USE_WINAPI := 1
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

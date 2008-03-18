USES := # nothing

LIBRARY := expat
VERSION := 1.95.6

SOURCES += xmlparse.c
SOURCES += xmlrole.c
SOURCES += xmltok.c
MANPAGE_SOURCES += ascii.h
MANPAGE_SOURCES += asciitab.h
MANPAGE_SOURCES += expat.h
MANPAGE_SOURCES += iasciitab.h
MANPAGE_SOURCES += internal.h
MANPAGE_SOURCES += latin1tab.h
MANPAGE_SOURCES += macconfig.h
MANPAGE_SOURCES += nametab.h
MANPAGE_SOURCES += utf8tab.h
MANPAGE_SOURCES += winconfig.h
MANPAGE_SOURCES += xmltok_impl.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifeq ($(OS),win32)
APP_CFLAGS += -DXML_STATIC -DCOMPILED_FROM_DSP
USE_WINAPI := 1
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

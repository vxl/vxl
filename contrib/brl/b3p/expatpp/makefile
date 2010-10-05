USES := contrib/brl/b3p/expat

LIBRARY := expatpp
VERSION := 1.95.6

SOURCES += expatpp.cpp
MANPAGE_SOURCES += expatpplib.h

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

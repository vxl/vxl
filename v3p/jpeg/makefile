USES = # nothing
USE_STDC++ := 0

LIBRARY := jpeg
VERSION := 1.0

SOURCES += jcapimin.c
SOURCES += jcapistd.c 
SOURCES += jccoefct.c 
SOURCES += jccolor.c 
SOURCES += jcdctmgr.c 
SOURCES += jchuff.c
SOURCES += jcinit.c 
SOURCES += jcmainct.c 
SOURCES += jcmarker.c 
SOURCES += jcmaster.c 
SOURCES += jcomapi.c 
SOURCES += jcparam.c 
SOURCES += jcphuff.c 
SOURCES += jcprepct.c 
SOURCES += jcsample.c 
SOURCES += jctrans.c 
SOURCES += jdapimin.c 
SOURCES += jdapistd.c
SOURCES += jdatadst.c 
SOURCES += jdatasrc.c 
SOURCES += jdcoefct.c 
SOURCES += jdcolor.c 
SOURCES += jddctmgr.c 
SOURCES += jdhuff.c
SOURCES += jdinput.c 
SOURCES += jdmainct.c 
SOURCES += jdmarker.c 
SOURCES += jdmaster.c 
SOURCES += jdmerge.c 
SOURCES += jdphuff.c
SOURCES += jdpostct.c 
SOURCES += jdsample.c 
SOURCES += jdtrans.c 
SOURCES += jerror.c 
SOURCES += jfdctflt.c 
SOURCES += jfdctfst.c
SOURCES += jfdctint.c 
SOURCES += jidctflt.c 
SOURCES += jidctfst.c 
SOURCES += jidctint.c 
SOURCES += jidctred.c 
SOURCES += jquant1.c
SOURCES += jquant2.c 
SOURCES += jutils.c 
SOURCES += jmemmgr.c 
SOURCES += jmemansi.c

MANPAGE_SOURCES += jdct.h
MANPAGE_SOURCES += jconfig.h
MANPAGE_SOURCES += jinclude.h
MANPAGE_SOURCES += jmemsys.h
MANPAGE_SOURCES += jmorecfg.h
MANPAGE_SOURCES += jpegint.h
MANPAGE_SOURCES += jpeglib.h
MANPAGE_SOURCES += jversion.h

################################
include ${VXLROOT}/config/top.mk
################################

ifeq ($(OS),win32)
CFLAGS += -D_jpeg_mak
endif

# Always optimize this lib
debug = $(optimize)

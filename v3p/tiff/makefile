#   DESCRIPTION:
#
#   This is the TIFF directory for Image.  It contains the code and class for read/write
#   of TIFF images.  It is a copy of the TIFF library found in InterViews and is provided 
#   to replace InterViews.
#   Several object files are not longer built because its functionality is not needed
#   elsewhere (more specifically, here and in ImageClasses, which is the only interface
#   to the TIFF library).  The source code remains in place, though.
#
#   DEPENDENCIES:  libm libc

USES = # completely independent!

LIBRARY := TIFF
VERSION := 1.0

SOURCES += tif_close.c
SOURCES += tif_compat.c
SOURCES += tif_compress.c
SOURCES += tif_dir.c
SOURCES += tif_dirinfo.c
SOURCES += tif_dirread.c
SOURCES += tif_dirwrite.c
SOURCES += tif_dumpmode.c
SOURCES += tif_error.c
SOURCES += tif_jpeg.c
SOURCES += tif_flush.c
SOURCES += tif_lzw.c
SOURCES += tif_open.c
SOURCES += tif_packbits.c
SOURCES += tif_read.c
SOURCES += tif_swab.c
SOURCES += tif_strip.c
SOURCES += tif_tile.c
SOURCES += tif_warning.c
SOURCES += tif_write.c
SOURCES += tif_aux.c

DOCFILES = tiff

APP_CFLAGS += -DCOLORIMETRY_SUPPORT -DYCBCR_SUPPORT -DHAVE_IEEEFP=1

##########################################
include ${SYS_IUPACKAGES}/config/top.mk
##########################################

ifeq ($(OS),irix)
APP_CFLAGS += -DJPEG_SUPPORT -DCMYK_SUPPORT \
    -DMMAP_SUPPORT -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1 -ansi
endif

ifeq ($(OS),sunos4)
APP_CFLAGS += -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1
endif

ifeq ($(OS),aix)
APP_CFLAGS += -DJPEG_SUPPORT -DCMYK_SUPPORT \
    -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1
endif

ifeq ($(OS),solaris)
APP_CFLAGS +=    -DCOLORIMETRY_SUPPORT -DJPEG_SUPPORT -DYCBCR_SUPPORT -DCMYK_SUPPORT \
    -DHAVE_IEEEFP=1 -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1 -DSYSV
endif

ifeq ($(OS),win32)
APP_CFLAGS +=    -DCOLORIMETRY_SUPPORT -DJPEG_SUPPORT -DYCBCR_SUPPORT -DCMYK_SUPPORT \
    -DHAVE_IEEEFP=1 -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1 -DWIN32
endif

ifeq ($(OS),hp)
APP_CFLAGS += -DUSE_VARARGS=1 -DUSE_PROTOTYPES=0
endif

ifeq ($(OS),ultrix)
APP_CFLAGS += -DUSE_VARARGS=1 -DUSE_PROTOTYPES=0
endif

ifeq ($(OS),next)
APP_CFLAGS += -DJPEG_SUPPORT -DCMYK_SUPPORT \
    -DUSE_VARARGS=1 -DUSE_PROTOTYPES=0
endif

ifeq ($(OS), apollo)
APP_CFLAGS += -DUSE_VARARGS=1 -DUSE_PROTOTYPES=0 -A nansi
endif

ifeq ($(OS), linux)
APP_CFLAGS += -DJPEG_SUPPORT -DCMYK_SUPPORT \
    -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1
endif

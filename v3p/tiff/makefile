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

USES := # nothing
USE_STDC++ := 0
USE_JPEG := 1

LIBRARY := tiff
VERSION := 3.6

MANPAGE_SOURCES += tiff.h
SOURCES += tif_aux.c
SOURCES += tif_close.c
SOURCES += tif_codec.c
SOURCES += tif_color.c
SOURCES += tif_compress.c
SOURCES += tif_dir.c
SOURCES += tif_dirinfo.c
SOURCES += tif_dirread.c
SOURCES += tif_dirwrite.c
SOURCES += tif_dumpmode.c
SOURCES += tif_error.c
SOURCES += tif_extension.c
SOURCES += tif_fax3.c
SOURCES += tif_fax3sm.c
SOURCES += fax3sm_winnt.c
SOURCES += tif_getimage.c
SOURCES += tif_jpeg.c
SOURCES += tif_flush.c
SOURCES += tif_luv.c
SOURCES += tif_lzw.c
SOURCES += tif_next.c
SOURCES += tif_open.c
SOURCES += tif_packbits.c
SOURCES += tif_pixarlog.c
SOURCES += tif_predict.c
SOURCES += tif_print.c
SOURCES += tif_ojpeg.c
SOURCES += tif_read.c
SOURCES += tif_swab.c
SOURCES += tif_strip.c
SOURCES += tif_thunder.c
SOURCES += tif_tile.c
SOURCES += tif_version.c
SOURCES += tif_warning.c
SOURCES += tif_write.c
SOURCES += tif_zip.c
MANPAGE_SOURCES += port.h
MANPAGE_SOURCES += t4.h
MANPAGE_SOURCES += tiffcomp.h
MANPAGE_SOURCES += tiffconf.h
MANPAGE_SOURCES += tiffio.h
MANPAGE_SOURCES += tiffiop.h
MANPAGE_SOURCES += tiffvers.h
MANPAGE_SOURCES += uvcode.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifeq ($(OS),win32)
SOURCES += tif_win32.c
USE_WINAPI := 1
else
SOURCES += tif_unix.c
endif

DOCFILES = tiff

APP_CFLAGS += -DHAVE_IEEEFP=1

ifeq ($(OS),win32)
APP_CFLAGS += -D_tiff_mak
endif

####### MAKE SURE THESE STAY IN CORRESPONDENCE WITH config/identify_os.mk #####

ifeq ($(OS),IRIX5)
APP_CFLAGS += -DJPEG_SUPPORT \
    -DMMAP_SUPPORT -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1 -ansi
endif

ifeq ($(OS),IRIX6)
APP_CFLAGS += -DJPEG_SUPPORT \
#    -DMMAP_SUPPORT -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1 -ansi
endif

ifeq ($(OS),SunOS4)
APP_CFLAGS += -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1
endif

ifeq ($(OS),aix)
APP_CFLAGS += -DJPEG_SUPPORT \
    -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1
endif

ifeq ($(OS),SunOS5)
APP_CFLAGS +=  -DJPEG_SUPPORT \
    -DHAVE_IEEEFP=1 -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1 -DSYSV
endif

ifeq ($(OS),win32)
APP_CFLAGS +=  -DJPEG_SUPPORT \
    -DHAVE_IEEEFP=1 -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1 -DWIN32
USE_WINAPI := 1
endif

ifeq "HP-UX" "$(findstring HP-UX,$(OS))"
APP_CFLAGS += -DUSE_VARARGS=1 -DUSE_PROTOTYPES=0
endif

ifeq ($(OS),ULTRIX)
APP_CFLAGS += -DUSE_VARARGS=1 -DUSE_PROTOTYPES=0
endif

ifeq ($(OS),next)
APP_CFLAGS += -DJPEG_SUPPORT \
    -DUSE_VARARGS=1 -DUSE_PROTOTYPES=0
endif

ifeq ($(OS), apollo)
APP_CFLAGS += -DUSE_VARARGS=1 -DUSE_PROTOTYPES=0 -A nansi
endif

ifeq ($(OS), Linux2)
APP_CFLAGS += -DJPEG_SUPPORT \
    -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1
endif

ifeq ($(OS), freebsd)
APP_CFLAGS += -DJPEG_SUPPORT \
    -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1
endif

ifeq ($(OS),OSF1V4)
APP_CFLAGS +=  -DJPEG_SUPPORT \
    -DHAVE_IEEEFP=1 -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1
endif

ifeq ($(OS),OSF1V3.0)
APP_CFLAGS +=  -DJPEG_SUPPORT \
    -DHAVE_IEEEFP=1 -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1
endif

ifeq ($(OS),OSF1V3.2)
APP_CFLAGS +=  -DJPEG_SUPPORT \
    -DHAVE_IEEEFP=1 -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1
endif


IULIBS += -ljpeg

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

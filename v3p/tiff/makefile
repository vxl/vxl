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

USES = Image

LIBRARY := TIFF
VERSION := 1.0

##########################################
include ${SYS_IUPACKAGES}/config/top-params.mk
##########################################

SOURCES += tif_aux.c
SOURCES += tif_close.c
SOURCES += tif_codec.c
SOURCES += tif_compress.c
SOURCES += tif_dir.c
SOURCES += tif_dirinfo.c
SOURCES += tif_dirread.c
SOURCES += tif_dirwrite.c
SOURCES += tif_dumpmode.c
SOURCES += tif_error.c
SOURCES += tif_fax3.c
SOURCES += tif_fax3sm.c
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
SOURCES += tif_read.c
SOURCES += tif_swab.c
SOURCES += tif_strip.c
SOURCES += tif_thunder.c
SOURCES += tif_tile.c
SOURCES += tif_version.c
SOURCES += tif_warning.c
SOURCES += tif_write.c
SOURCES += tif_zip.c

ifeq ($(OS),win32)
SOURCES += tif_win32.c
USE_WINAPI := 1
else
SOURCES += tif_unix.c
endif


DOCFILES = tiff

APP_CFLAGS += -DHAVE_IEEEFP=1


####### MAKE SURE THESE STAY IN CORRESPONDENCE WITH config/identify_os.mk #####

ifeq ($(OS),irix5)
APP_CFLAGS += -DJPEG_SUPPORT \
    -DMMAP_SUPPORT -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1 -ansi
endif

ifeq ($(OS),irix6)
APP_CFLAGS += -DJPEG_SUPPORT \
#    -DMMAP_SUPPORT -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1 -ansi
endif


ifeq ($(OS),sunos4)
APP_CFLAGS += -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1
endif

ifeq ($(OS),aix)
APP_CFLAGS += -DJPEG_SUPPORT \
    -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1
endif

ifeq ($(OS),solaris)
APP_CFLAGS +=  -DJPEG_SUPPORT \
    -DHAVE_IEEEFP=1 -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1 -DSYSV
endif

ifeq ($(OS),win32)
APP_CFLAGS +=  -DJPEG_SUPPORT \
    -DHAVE_IEEEFP=1 -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1 -DWIN32
USE_WINAPI := 1
endif

ifeq "hpux" "$(findstring hpux,$(OS))"
APP_CFLAGS += -DUSE_VARARGS=1 -DUSE_PROTOTYPES=0
endif

ifeq ($(OS),hp)
APP_CFLAGS += -DUSE_VARARGS=1 -DUSE_PROTOTYPES=0
endif


ifeq ($(OS),ultrix)
APP_CFLAGS += -DUSE_VARARGS=1 -DUSE_PROTOTYPES=0
endif

ifeq ($(OS),next)
APP_CFLAGS += -DJPEG_SUPPORT \
    -DUSE_VARARGS=1 -DUSE_PROTOTYPES=0
endif

ifeq ($(OS), apollo)
APP_CFLAGS += -DUSE_VARARGS=1 -DUSE_PROTOTYPES=0 -A nansi
endif

ifeq ($(OS), linux)
APP_CFLAGS += -DJPEG_SUPPORT \
    -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1
endif

ifeq ($(OS), freebsd)
APP_CFLAGS += -DJPEG_SUPPORT \
    -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1
endif

ifeq ($(OS),alpha4)
APP_CFLAGS +=  -DJPEG_SUPPORT \
    -DHAVE_IEEEFP=1 -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1
endif

ifeq ($(OS),alpha3.0)
APP_CFLAGS +=  -DJPEG_SUPPORT \
    -DHAVE_IEEEFP=1 -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1
endif

ifeq ($(OS),alpha3.2)
APP_CFLAGS +=  -DJPEG_SUPPORT \
    -DHAVE_IEEEFP=1 -DUSE_VARARGS=0 -DUSE_PROTOTYPES=1 -DUSE_CONST=1
endif


IULIBS += -lJPEG

##########################################
include ${SYS_IUPACKAGES}/config/top-rules.mk
##########################################

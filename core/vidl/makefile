#   DESCRIPTION:
#
#   This is the vidl library.  It is in the vidl Package.
#   It contains basic Video classes.
#
#   DEPENDENCIES: vbl vil vidl

LIBRARY := vidl
VERSION := 1.0

USES += vxl
TRY_MPEG := 1

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

MANPAGE_SOURCES += vidl_codec.h
SOURCES += vidl_clip.cxx
SOURCES += vidl_movie.cxx
SOURCES += vidl_frame.cxx
SOURCES += vidl_frame_as_image.cxx
SOURCES += vidl_image_list_codec.cxx
SOURCES += vidl_io.cxx

ifdef HAS_MPEG
SOURCES += vidl_mpegcodec.cxx
SOURCES += vidl_mpegcodec_helper.cxx
SOURCES += vidl_yuv_2_rgb.cxx
endif

SOURCES += vidl_test.cxx

MANPAGE_SOURCES += vidl_avicodec_sptr.h
MANPAGE_SOURCES += vidl_clip_sptr.h
MANPAGE_SOURCES += vidl_codec_sptr.h
MANPAGE_SOURCES += vidl_frame_as_image_sptr.h
MANPAGE_SOURCES += vidl_frame_sptr.h
MANPAGE_SOURCES += vidl_image_list_codec_sptr.h
MANPAGE_SOURCES += vidl_movie_sptr.h
ifdef HAS_MPEG
MANPAGE_SOURCES += vidl_mpegcodec_sptr.h
endif

# I think we should ask for the microsoft
# compiler instead of $(OS); if you know how to
# do it, feel free to participate.
ifeq ($(OS),win32)
# avi codec which works under NT
SOURCES += vidl_avicodec.cxx
endif # win32

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

# I think we should ask for the microsoft
# compiler instead of $(OS); if you know how to
# do it, feel free to participate.
ifeq ($(OS),win32)
LDLIBS += vfw32.lib gdi32.lib user32.lib
endif # win32

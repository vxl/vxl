#   DESCRIPTION:
#
#   This is the vidl_vil1 library.
#   It contains basic Video classes.
#   See also the vidl library.
#
#   DEPENDENCIES: vbl vil1

USES += core
TRY_MPEG2 := 1

LIBRARY := vidl_vil1
VERSION := 1.0

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

MANPAGE_SOURCES += vidl_vil1_codec.h
SOURCES += vidl_vil1_clip.cxx
SOURCES += vidl_vil1_movie.cxx
SOURCES += vidl_vil1_frame.cxx
SOURCES += vidl_vil1_frame_as_image.cxx
SOURCES += vidl_vil1_image_list_codec.cxx
SOURCES += vidl_vil1_file_sequence.cxx
SOURCES += vidl_vil1_vob_frame_index.cxx
SOURCES += vidl_vil1_io.cxx

ifdef HAS_MPEG2
SOURCES += vidl_vil1_mpegcodec.cxx
SOURCES += vidl_vil1_mpegcodec_helper.cxx
MANPAGE_SOURCES += vidl_vil1_yuv_2_rgb.h
endif

MANPAGE_SOURCES += vidl_vil1_avicodec_sptr.h
MANPAGE_SOURCES += vidl_vil1_clip_sptr.h
MANPAGE_SOURCES += vidl_vil1_codec_sptr.h
MANPAGE_SOURCES += vidl_vil1_frame_as_image_sptr.h
MANPAGE_SOURCES += vidl_vil1_frame_sptr.h
MANPAGE_SOURCES += vidl_vil1_image_list_codec_sptr.h
MANPAGE_SOURCES += vidl_vil1_movie_sptr.h
ifdef HAS_MPEG2
MANPAGE_SOURCES += vidl_vil1_mpegcodec_sptr.h
endif

# I think we should ask for the microsoft
# compiler instead of $(OS); if you know how to
# do it, feel free to participate.
ifeq ($(OS),win32)
# avi codec which works under NT
SOURCES += vidl_vil1_avicodec.cxx
endif # win32

ifndef NOTEST
SUBDIRS += tests examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

# I think we should ask for the microsoft
# compiler instead of $(OS); if you know how to
# do it, feel free to participate.
ifeq ($(OS),win32)
LDLIBS += vfw32.lib gdi32.lib user32.lib
endif # win32

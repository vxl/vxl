#   DESCRIPTION:
#
#   This is the vidl library.
#   It contains basic Video classes.
#
#   DEPENDENCIES: vbl vil

USES += core
TRY_AVI := 1
TRY_MPEG2 := 1

LIBRARY := vidl
VERSION := 1.0

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

MANPAGE_SOURCES += vidl_codec.h
SOURCES += vidl_clip.cxx
SOURCES += vidl_movie.cxx
SOURCES += vidl_frame.cxx
SOURCES += vidl_image_list_codec.cxx
SOURCES += vidl_file_sequence.cxx
SOURCES += vidl_vob_frame_index.cxx
SOURCES += vidl_io.cxx

ifdef HAS_MPEG2
SOURCES += vidl_mpegcodec.cxx
SOURCES += vidl_mpegcodec_helper.cxx
SOURCES += vidl_yuv_2_rgb.cxx
endif

MANPAGE_SOURCES += vidl_clip_sptr.h
MANPAGE_SOURCES += vidl_codec_sptr.h
MANPAGE_SOURCES += vidl_frame_sptr.h
MANPAGE_SOURCES += vidl_image_list_codec_sptr.h
MANPAGE_SOURCES += vidl_movie_sptr.h
ifdef HAS_MPEG2
MANPAGE_SOURCES += vidl_mpegcodec_sptr.h
endif

ifdef HAS_AVI
MANPAGE_SOURCES += vidl_avicodec.h
# I think we should ask for the microsoft
# compiler instead of $(OS); if you know how to
# do it, feel free to participate.
ifeq ($(OS),win32)
# avi codec which works under NT/Xp
SOURCES += vidl_win_avicodec.cxx
else
# avi codec which works under Linux
SOURCES += vidl_avifile_avicodec.cxx
endif # win32
endif # HAS_AVI

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

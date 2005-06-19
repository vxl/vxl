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

SOURCES += vidl_vob_frame_index.cxx
SOURCES += vidl_file_sequence.cxx
SOURCES += vidl_codec.cxx
MANPAGE_SOURCES += vidl_codec_sptr.h
SOURCES += vidl_clip.cxx
MANPAGE_SOURCES += vidl_clip_sptr.h
SOURCES += vidl_movie.cxx
MANPAGE_SOURCES += vidl_movie_sptr.h
SOURCES += vidl_frame.cxx
MANPAGE_SOURCES += vidl_frame_sptr.h
SOURCES += vidl_frame_resource.cxx
SOURCES += vidl_image_list_codec.cxx
MANPAGE_SOURCES += vidl_image_list_codec_sptr.h
SOURCES += vidl_io.cxx

ifdef HAS_MPEG2
SOURCES += vidl_mpegcodec.cxx
MANPAGE_SOURCES += vidl_mpegcodec_sptr.h
SOURCES += vidl_mpegcodec_helper.cxx
MANPAGE_SOURCES += vidl_yuv_2_rgb.h
endif

ifdef HAS_FFMPEG
SOURCES += vidl_ffmpeg_codec.cxx
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

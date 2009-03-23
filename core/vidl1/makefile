#   DESCRIPTION:
#
#   This is the vidl1 library.
#   It contains basic Video classes.
#
#   DEPENDENCIES: vbl vil

USES += core
TRY_AVI := 1
TRY_MPEG2 := 1

LIBRARY := vidl1
VERSION := 1.0
DEFINES += -DBUILDING_VIDL_DLL

MANPAGE_SOURCES += dll.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

SOURCES += vidl1_vob_frame_index.cxx
SOURCES += vidl1_file_sequence.cxx
SOURCES += vidl1_codec.cxx
MANPAGE_SOURCES += vidl1_codec_sptr.h
SOURCES += vidl1_clip.cxx
MANPAGE_SOURCES += vidl1_clip_sptr.h
SOURCES += vidl1_movie.cxx
MANPAGE_SOURCES += vidl1_movie_sptr.h
SOURCES += vidl1_frame.cxx
MANPAGE_SOURCES += vidl1_frame_sptr.h
SOURCES += vidl1_frame_resource.cxx
SOURCES += vidl1_image_list_codec.cxx
MANPAGE_SOURCES += vidl1_image_list_codec_sptr.h
SOURCES += vidl1_io.cxx

ifdef HAS_MPEG2
SOURCES += vidl1_mpegcodec.cxx
MANPAGE_SOURCES += vidl1_mpegcodec_sptr.h
SOURCES += vidl1_mpegcodec_helper.cxx
MANPAGE_SOURCES += vidl1_yuv_2_rgb.h
endif

ifdef HAS_FFMPEG
SOURCES += vidl1_ffmpeg_codec.cxx
endif

ifdef HAS_AVI
MANPAGE_SOURCES += vidl1_avicodec.h
# I think we should ask for the microsoft
# compiler instead of $(OS); if you know how to
# do it, feel free to participate.
ifeq ($(OS),win32)
# avi codec which works under NT/Xp
SOURCES += vidl1_win_avicodec.cxx
else
# avi codec which works under Linux
SOURCES += vidl1_avifile_avicodec.cxx
endif # win32
endif # HAS_AVI

ifndef NOTEST
SUBDIRS += tests examples
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

# I think we should ask for the microsoft
# compiler instead of $(OS); if you know how to
# do it, feel free to participate.
ifeq ($(OS),win32)
LDLIBS += vfw32.lib gdi32.lib user32.lib
endif # win32

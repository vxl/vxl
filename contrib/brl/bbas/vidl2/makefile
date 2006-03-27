USES += contrib/brl contrib/brl/bbas core

LIBRARY := vidl2
VERSION := 1.0

SOURCES += vidl2_color.cxx
SOURCES += vidl2_pixel_format.cxx
SOURCES += vidl2_frame.cxx
MANPAGE_SOURCES += vidl2_frame_sptr.h
SOURCES += vidl2_pixel_iterator.cxx
MANPAGE_SOURCES += vidl2_pixel_iterator.txx
SOURCES += vidl2_convert.cxx

MANPAGE_SOURCES += vidl2_istream.h
SOURCES += vidl2_image_list_istream.cxx
MANPAGE_SOURCES += vidl2_ostream.h
SOURCES += vidl2_image_list_ostream.cxx
SOURCES += vidl2_iidc1394_params.cxx

SOURCES += vidl2_ffmpeg_init.cxx
SOURCES += vidl2_ffmpeg_istream.cxx
SOURCES += vidl2_ffmpeg_ostream.cxx
SOURCES += vidl2_ffmpeg_ostream_params.cxx
SOURCES += vidl2_ffmpeg_convert.cxx

SOURCES += vidl2_dc1394_istream.cxx

SOURCES += vidl2_dshow.cxx
SOURCES += vidl2_dshow_file_istream.cxx
SOURCES += vidl2_dshow_istream_params.cxx
SOURCES += vidl2_dshow_live_istream.cxx
SOURCES += vidl2_dshow_istream_params_esf.cxx

SOURCES += vidl2_v4l_istream.cxx
SOURCES += vidl2_v4l_params.cxx

MANPAGE_SOURCES += vidl2_exception.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
 SUBDIRS += examples
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

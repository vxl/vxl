USES += contrib/brl contrib/brl/bbas core

LIBRARY := vidl2
VERSION := 1.0

MANPAGE_SOURCES += vidl2_color.h
SOURCES += vidl2_pixel_format.cxx
SOURCES += vidl2_frame.cxx
MANPAGE_SOURCES += vidl2_frame_sptr.h
MANPAGE_SOURCES += vidl2_pixel_iterator.h
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

USES += contrib/brl contrib/brl/bbas core

LIBRARY := vidl2
VERSION := 1.0

SOURCES += vidl2_ffmpeg_init.cxx
SOURCES += vidl2_ffmpeg_istream.cxx
SOURCES += vidl2_ffmpeg_ostream.cxx
SOURCES += vidl2_ffmpeg_ostream_params.cxx
SOURCES += vidl2_image_list_istream.cxx
SOURCES += vidl2_image_list_ostream.cxx
MANPAGE_SOURCES += vidl2_istream.h
MANPAGE_SOURCES += vidl2_ostream.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += examples
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

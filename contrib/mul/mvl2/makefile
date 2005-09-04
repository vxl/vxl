USES += contrib/mul core
TRY_AVI := 1

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

LIBRARY := mvl2
VERSION := 1.0

MANPAGE_SOURCES += mvl2_video_writer.h
SOURCES += mvl2_video_reader.cxx
SOURCES += mvl2_video_from_sequence.cxx

ifeq ($(HAS_AVI),1)
USE_AVI := 1
 SOURCES += mvl2_image_format_plugin.cxx
 MANPAGE_SOURCES += mvl2_video_from_avi.h
 MANPAGE_SOURCES += mvl2_video_to_avi.h

 ifeq ($(OS),win32)
  SOURCES += mvl2_video_from_avi_windows.cxx
  SOURCES += mvl2_video_to_avi_windows.cxx
 else
  SOURCES += mvl2_video_from_avi_linux.cxx
  SOURCES += mvl2_video_to_avi_linux.cxx
 endif
endif

ifndef NOTEST
 SUBDIRS += examples
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

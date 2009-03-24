USES += core

LIBRARY := vidl
VERSION := 1.0

SOURCES += vidl_color.cxx
SOURCES += vidl_pixel_format.cxx
SOURCES += vidl_frame.cxx
MANPAGE_SOURCES += vidl_frame_sptr.h
SOURCES += vidl_pixel_iterator.cxx
MANPAGE_SOURCES += vidl_pixel_iterator.txx
SOURCES += vidl_convert.cxx

MANPAGE_SOURCES += vidl_istream.h
MANPAGE_SOURCES += vidl_istream_sptr.h
MANPAGE_SOURCES += vidl_ostream.h
MANPAGE_SOURCES += vidl_ostream_sptr.h
SOURCES += vidl_istream_image_resource.cxx
SOURCES += vidl_image_list_istream.cxx
SOURCES += vidl_image_list_ostream.cxx

SOURCES += vidl_iidc1394_params.cxx

SOURCES += vidl_ffmpeg_init.cxx
SOURCES += vidl_ffmpeg_istream.cxx
SOURCES += vidl_ffmpeg_ostream.cxx
SOURCES += vidl_ffmpeg_ostream_params.cxx
SOURCES += vidl_ffmpeg_convert.cxx

SOURCES += vidl_dc1394_istream.cxx

SOURCES += vidl_dshow.cxx
SOURCES += vidl_dshow_file_istream.cxx
SOURCES += vidl_dshow_istream_params.cxx
SOURCES += vidl_dshow_live_istream.cxx
SOURCES += vidl_dshow_istream_params_esf.cxx

SOURCES += vidl_v4l_istream.cxx
SOURCES += vidl_v4l_params.cxx

MANPAGE_SOURCES += vidl_exception.h
MANPAGE_SOURCES += vidl_config.h.in

MANPAGE_SOURCES += vidl_ffmpeg_istream_stub.txx
MANPAGE_SOURCES += vidl_ffmpeg_istream_v1.txx
MANPAGE_SOURCES += vidl_ffmpeg_istream_v2.txx
MANPAGE_SOURCES += vidl_ffmpeg_ostream_stub.txx
MANPAGE_SOURCES += vidl_ffmpeg_ostream_v1.txx
MANPAGE_SOURCES += vidl_ffmpeg_ostream_v2.txx
MANPAGE_SOURCES += vidl_ffmpeg_ostream_v3.txx

SOURCES += v4l2_pixel_format.cxx
SOURCES += vidl_v4l2_device.cxx
SOURCES += vidl_v4l2_devices.cxx
SOURCES += vidl_v4l2_istream.cxx
MANPAGE_SOURCES += vidl_v4l2_device_sptr.h
SOURCES += vidl_v4l2_control.cxx

# vgui add-on library
SUBDIRS += gui

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

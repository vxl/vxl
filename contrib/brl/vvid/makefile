USES += contrib/brl contrib/gel contrib/brl/bbas contrib/brl/bseg core
include ${VXLROOT}/core/vgui/config.mk
USE_OPENGL:= 1
TRY_MPEG2 := 1

LIBRARY := vvid
VERSION := 1.0

IMMEDIATE_LIBS := bgui vgui sdet strk vpro brip btol vtol vsol vidl vidl_vil1 vul vil vil1

# sources that don't depend on the camera being present
SOURCES += vvid_frame_trail.cxx
SOURCES += vvid_vil_file_manager.cxx
SOURCES += vvid_file_manager.cxx
SOURCES += vvid_command_line_player.cxx

# sources that do depend on the camera being present
SOURCES += cmu_1394_camera_params.cxx
ifdef HAS_CMU1394CAMERA
DEFINES += -DHAS_CMU1394CAMERA
SOURCES += cmu_1394_camera.cxx
SOURCES += vvid_live_video_manager.cxx
SOURCES += vvid_live_video_tableau.cxx
MANPAGE_SOURCES += vvid_live_video_tableau_sptr.h
SOURCES += vvid_live_stereo_manager.cxx
endif

SUBDIRS += bin

################################
include ${VXLROOT}/config/top.mk
################################

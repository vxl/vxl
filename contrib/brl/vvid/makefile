USES += contrib/brl contrib/gel contrib/brl/bbas contrib/brl/bseg core
USE_OPENGL:= 1
TRY_MPEG2 := 1

LIBRARY := vvid
VERSION := 1.0

IMMEDIATE_LIBS := vvid gevd vgui vidl vul vil

# sources that don't depend on the camera being present
SOURCES += vvid_video_process.cxx
MANPAGE_SOURCES += vvid_video_process_sptr.h
SOURCES += vvid_region_process.cxx
SOURCES += vvid_capture_process.cxx
SOURCES += vvid_frame_diff_process.cxx
SOURCES += vvid_motion_process.cxx
SOURCES += vvid_lucas_kanade_process.cxx
SOURCES += vvid_epipolar_space_process.cxx
SOURCES += vvid_edge_process.cxx
SOURCES += vvid_harris_corner_process.cxx
SOURCES += vvid_file_manager.cxx

# sources that do depend on the camera being present
SOURCES += cmu_1394_camera_params.cxx
ifdef HAS_CMU1394CAMERA
DEFINES += -DHAS_CMU1394CAMERA
SOURCES += cmu_1394_camera.cxx
SOURCES += vvid_live_video_frame.cxx
MANPAGE_SOURCES += vvid_live_video_frame_sptr.h
SOURCES += vvid_live_video_manager.cxx
SOURCES += vvid_live_video_tableau.cxx
MANPAGE_SOURCES += vvid_live_video_tableau_sptr.h
SOURCES += vvid_live_stereo_manager.cxx
endif

SUBDIRS += bin

################################
include ${IUEROOT}/config/top.mk
################################

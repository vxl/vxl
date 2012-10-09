USES += contrib/brl contrib/brl/bbas core

LIBRARY := bhdfs_pro
VERSION := 1.0

SOURCES += bhdfs_register.cxx
MANPAGE_SOURCES += bhdfs_processes.h

SOURCES += processes/bhdfs_fs_manager_processes.cxx
SOURCES += processes/bhdfs_load_image_view_process.cxx
SOURCES += processes/bhdfs_save_image_view_process.cxx
SOURCES += processes/bhdfs_mapper_scripting_processes.cxx
SOURCES += processes/bhdfs_boxm2_load_scene_process.cxx
SOURCES += processes/bhdfs_load_perspective_camera_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

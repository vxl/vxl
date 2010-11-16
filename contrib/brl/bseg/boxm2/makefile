USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2

SOURCES += boxm2_block.cxx
SOURCES += boxm2_disk_io_mgr.cxx
SOURCES += boxm2_scene_parser.cxx
SOURCES += boxm2_scene.cxx
SOURCES += boxm2_cache.cxx
SOURCES += boxm2_block_id.cxx
SOURCES += boxm2_dumb_cache.cxx
SOURCES += boxm2_asio_mgr.cxx
SOURCES += boxm2_sio_mgr.cxx
MANPAGE_SOURCES += boxm2_array_2d.txx
MANPAGE_SOURCES += boxm2_array_1d.txx
MANPAGE_SOURCES += boxm2_array_3d.txx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

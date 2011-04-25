USES += contrib/brl contrib/brl/bseg core

LIBRARY := boxm2_pro

SOURCES += boxm2_register.cxx

MANPAGE_SOURCES += boxm2_processes.h
SOURCES += processes/boxm2_load_scene_process.cxx
SOURCES += processes/boxm2_create_cache_process.cxx
SOURCES += processes/boxm2_add_block_process.cxx
SOURCES += processes/boxm2_write_scene_xml_process.cxx
SOURCES += processes/boxm2_create_scene_process.cxx
SOURCES += processes/boxm2_export_textured_mesh_process.cxx
SOURCES += processes/boxm2_save_data_process.cxx
SOURCES += processes/boxm2_texture_mesh_process.cxx
SOURCES += processes/boxm2_export_mesh_process.cxx
SOURCES += processes/boxm2_describe_scene_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

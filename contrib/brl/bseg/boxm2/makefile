USES += contrib/brl contrib/brl/bbas contrib/brl/bseg core

LIBRARY := boxm2

SOURCES += boxm2_block.cxx
SOURCES += boxm2_scene_parser.cxx
SOURCES += boxm2_scene.cxx
SOURCES += boxm2_block_id.cxx
SOURCES += boxm2_opencl_processor.cxx
SOURCES += boxm2_cpp_processor.cxx
MANPAGE_SOURCES += boxm2_data.txx
MANPAGE_SOURCES += boxm2_data_traits.h
MANPAGE_SOURCES += boxm2_process.h
MANPAGE_SOURCES += boxm2_processor.h
MANPAGE_SOURCES += boxm2_opencl_render_process.h
MANPAGE_SOURCES += boxm2_cpp_process_base.h
MANPAGE_SOURCES += boxm2_opencl_process_base.h
SOURCES += boxm2_data_base.cxx
SOURCES += boxm2_cpp_render_process.cxx
SOURCES += boxm2_opencl_render_process.cxx
SOURCES += boxm2_opencl_render_depth_process.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

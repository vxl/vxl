USES += contrib/brl contrib/brl/bbas

LIBRARY := bocl

MANPAGE_SOURCES += bocl_cl.h
MANPAGE_SOURCES += bocl_cl_gl.h
MANPAGE_SOURCES += bocl_manager.h
MANPAGE_SOURCES += bocl_manager.txx
SOURCES += bocl_utils.cxx
SOURCES += bocl_global_memory_bandwidth_manager.cxx
SOURCES += bocl_kernel.cxx
SOURCES += bocl_mem.cxx
MANPAGE_SOURCES += bocl_buffer.h
MANPAGE_SOURCES += bocl_buffer_mgr.h
SOURCES += bocl_command_queue_mgr.cxx
SOURCES += bocl_device_info.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

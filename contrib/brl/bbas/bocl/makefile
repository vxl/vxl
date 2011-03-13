USES += contrib/brl contrib/brl/bbas

LIBRARY := bocl

MANPAGE_SOURCES += bocl_cl.h
MANPAGE_SOURCES += bocl_cl_gl.h
SOURCES += bocl_manager.cxx
MANPAGE_SOURCES += bocl_manager.txx
SOURCES += bocl_utils.cxx
SOURCES += bocl_kernel.cxx
SOURCES += bocl_mem.cxx
MANPAGE_SOURCES += bocl_buffer.h
MANPAGE_SOURCES += bocl_buffer_mgr.h
SOURCES += bocl_device_info.cxx
SOURCES += bocl_device.cxx

SUBDIRS += pro

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

#
# vpl -- the vxl POSIX compatibility layer

USES += core

LIBRARY := vpl
VERSION := 1.0

SOURCES += vpl.cxx # includes one of os_cygwin/vpl.cxx os_win32/vpl.cxx os_unix/vpl.cxx
MANPAGE_SOURCES += vpl_mutex.h

DEFINES += -DBUILDING_VPL_DLL

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS = tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

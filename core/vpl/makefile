#######################################
include ${IUEROOT}/config/top-params.mk
#######################################
#
# vpl -- the vxl POSIX compatibility layer
#

USES := vxl

LIBRARY := vpl
VERSION := 1.0

ifndef NOTEST
SUBDIRS = tests
endif

SOURCES += vpl.cxx # includes one of os_cygwin/vpl.cxx os_win32/vpl.cxx os_unix/vpl.cxx
MANPAGE_SOURCES += vpl_mutex.h
MANPAGE_SOURCES += vpl_unistd.h

DEFINES += -DBUILDING_VPL_DLL

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

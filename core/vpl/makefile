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

SOURCES += vpl_unistd.cxx # includes one of os_cygwin/vpl_unistd.cxx os_win32/vpl_unistd.cxx os_unix/vpl_unistd.cxx

DEFINES += -DBUILDING_VPL_DLL

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

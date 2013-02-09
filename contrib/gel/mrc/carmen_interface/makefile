USES += contrib/gel core
include $(VXLROOT)/core/vbl/config.mk
include $(VXLROOT)/core/vul/config.mk

ifdef HAS_CARMEN
USES += Carmen
PROGRAM := carmen_interface
SOURCES += carmen_interface.cpp
SOURCES += main.cpp
endif

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS = tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

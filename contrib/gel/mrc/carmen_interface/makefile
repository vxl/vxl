USES += contrib/gel core
include $(IUEROOT)/core/vbl/config.mk
include $(IUEROOT)/core/vul/config.mk

ifdef HAS_CARMEN
USES += Carmen
PROGRAM := carmen_interface
SOURCES += carmen_interface.cpp
SOURCES += main.cpp
endif

################################
include ${IUEROOT}/config/top.mk
################################


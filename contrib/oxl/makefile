#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

SUBDIRS += mvl
SUBDIRS += oxp
SUBDIRS += mvox
SUBDIRS += vrml
ifeq (1,$(strip $(HAS_OPENGL)))
ifneq (,$(strip $(wildcard mvl)))
SUBDIRS += ../vxl/xcv
endif
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

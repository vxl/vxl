#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

SUBDIRS += mvl
SUBDIRS += oxp
SUBDIRS += mvox
SUBDIRS += vrml
ifneq (,$(strip $(wildcard mvox)))
SUBDIRS += ../vxl/xcv
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

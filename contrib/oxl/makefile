#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

SUBDIRS += osl

SUBDIRS += mvl
SUBDIRS += oxp
SUBDIRS += ../Oxford/mvox
SUBDIRS += vrml

ifeq (1,$(strip $(HAS_OPENGL)))
SUBDIRS += vgui
SUBDIRS += xcv
SUBDIRS += ../Oxford/xcv_ox
SUBDIRS += VPLAYER
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

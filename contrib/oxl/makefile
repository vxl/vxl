#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

SUBDIRS += mvl
SUBDIRS += oxp
SUBDIRS += mvox
SUBDIRS += vrml

ifeq (1,$(strip $(HAS_OPENGL)))
SUBDIRS += vgui
SUBDIRS += xcv
#no makefile?? SUBDIRS += VPLAYER
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

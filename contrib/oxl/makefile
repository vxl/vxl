#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

SUBDIRS += osl

SUBDIRS += mvl
SUBDIRS += oxp
#SUBDIRS += ../Oxford/mvox
SUBDIRS += vrml

ifeq (1,$(strip $(HAS_OPENGL)))
SUBDIRS += vgui
SUBDIRS += xcv
SUBDIRS += VPLAYER
SUBDIRS += apps
endif

ifndef NOTEST
SUBDIRS += oxltest
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

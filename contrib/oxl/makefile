# These four libraries are mutually independent:
SUBDIRS += osl
SUBDIRS += mvl
SUBDIRS += oxp
#SUBDIRS += mvox
SUBDIRS += vrml

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# using vgui:
ifeq (1,$(strip $(HAS_OPENGL)))
SUBDIRS += xcv
SUBDIRS += VPLAYER
SUBDIRS += apps
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

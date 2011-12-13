# These three libraries are mutually independent:
SUBDIRS += osl
SUBDIRS += mvl
SUBDIRS += vrml

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

# using vgui:
ifeq (1,$(strip $(HAS_OPENGL)))
SUBDIRS += xcv
SUBDIRS += apps
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################

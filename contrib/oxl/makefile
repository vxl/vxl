#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

SUBDIRS += mvl
SUBDIRS += oxp
SUBDIRS += mvox
SUBDIRS += vrml

# xcv is built from this directory because
# it depends on the libraries from oxl.
ifeq (1,$(strip $(HAS_OPENGL)))
ifneq (,$(strip $(wildcard mvl)))
SUBDIRS += ../vxl/xcv
endif
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

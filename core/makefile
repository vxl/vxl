#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# standard C/C++
SUBDIRS += vcl

# abstract
SUBDIRS += val

# basics
SUBDIRS += vbl

# numerics
ifneq (1,$(strip $(HAS_NATIVE_NETLIB)))
SUBDIRS += ../Numerics/netlib
endif
SUBDIRS += vnl

# geometry
SUBDIRS += vgl

# spatial objects
SUBDIRS += vsol

# topology
SUBDIRS += vtol

# image
ifneq (1,$(strip $(HAS_NATIVE_JPEG)))
SUBDIRS += ../Image/JPEG
endif
ifneq (1,$(strip $(HAS_NATIVE_TIFF)))
SUBDIRS += ../Image/TIFF
endif
SUBDIRS += vil

# segmentation
SUBDIRS += vsl

ifeq (1,$(strip $(HAS_OPENGL)))
# OpenGL ui library
SUBDIRS += vgui

#xcv must be compiled *after* mvox.
# # new xcv
# SUBDIRS += xcv
endif

# examples
SUBDIRS += examples


######################################
include ${IUEROOT}/config/top-rules.mk
######################################

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

# OpenGL ui library
SUBDIRS += vgui

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

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
SUBDIRS += vnl

# geometry
SUBDIRS += vgl

# spatial objects
SUBDIRS += vsol

# topology
SUBDIRS += vtol

# image
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

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# standard C/C++
SUBDIRS += vcl

# abstract
SUBDIRS += val

# templates
SUBDIRS += vtl

# "basics"
SUBDIRS += vbl

# numerics
SUBDIRS += vnl

# geometry
SUBDIRS += vgl

# image
SUBDIRS += vil

# image processing
SUBDIRS += vipl

# video
SUBDIRS += vvl

# spatial objects
SUBDIRS += vsol

# topology
SUBDIRS += vtol

# segmentation
SUBDIRS += vsl

# coordinate systems
SUBDIRS += vcsl

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

# standard C/C++
SUBDIRS += vcl

# POSIX layer
SUBDIRS += vpl

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
SUBDIRS += vidl

# spatial objects
SUBDIRS += vsol

# topology
SUBDIRS += vtol

# computational geometry
SUBDIRS += vcgl

# segmentation
SUBDIRS += vsl

# coordinate systems
SUBDIRS += vcsl

# digital geometry
SUBDIRS += vdgl

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifeq (1,$(strip $(HAS_OPENGL)))
# OpenGL ui library
SUBDIRS += vgui

# new xcv
# (must be compiled *after* mvl)
#ifeq (,$(strip $(wildcard $(IUELOCALROOT)/oxl/mvox)))
#SUBDIRS += xcv
#endif
endif

# examples
SUBDIRS += examples

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

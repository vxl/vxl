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

# New xcv. This must be compiled *after* mvl because
# it depends on mvl. The alternative is that when
# building from scratch (e.g. releases) the build
# of xcv fails due to missing libraries.
#
# Ultimately, xcv should be moved to oxl.
#
# -- fsm@robots.ox.ac.uk
#
#ifeq (,$(strip $(wildcard $(IUELOCALROOT)/oxl/mvl)))
#SUBDIRS += xcv
#endif
endif

# examples
SUBDIRS += examples

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

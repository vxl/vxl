#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# POSIX layer
SUBDIRS += vpl

# "basics" (templates)
SUBDIRS += vbl

# numerics
SUBDIRS += vnl

# geometry
SUBDIRS += vgl

# image
SUBDIRS += vil

# utilities
SUBDIRS += vul

# video
SUBDIRS += vidl

# computational geometry
SUBDIRS += vcgl

# coordinate systems
SUBDIRS += vcsl

# examples
ifndef NOTEST
SUBDIRS += examples
SUBDIRS += test_allvxl
##needs libvgui-vrml from oxl: SUBDIRS += apps/vxl_vrml_viewer
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

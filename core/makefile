#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

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
SUBDIRS += apps/vxl_vrml_viewer
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

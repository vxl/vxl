#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

MANPAGE_SOURCES += vxl_copyright.h
MANPAGE_SOURCES += vxl_config.h.in

# common test executable
SUBDIRS += testlib

# POSIX layer
SUBDIRS += vpl

# stream I/O
SUBDIRS += vsl

# utilities
SUBDIRS += vul

# "basics" (templates)
SUBDIRS += vbl

# numerics
SUBDIRS += vnl

# geometry (including computational geometry)
SUBDIRS += vgl

# image
SUBDIRS += vil

# video
SUBDIRS += vidl

# coordinate systems
SUBDIRS += vcsl

# examples
ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
##currently empty: SUBDIRS += test_allvxl
##needs libvgui-vrml from oxl: SUBDIRS += apps/vxl_vrml_viewer
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

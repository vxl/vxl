#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

MANPAGE_SOURCES += vxl_copyright.h
MANPAGE_SOURCES += vxl_config.h.in

# utilities
SUBDIRS += vul

# stream I/O
SUBDIRS += vsl

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

# video
SUBDIRS += vidl

# computational geometry
SUBDIRS += vcgl

# coordinate systems
SUBDIRS += vcsl

# examples
ifndef NOTEST
SUBDIRS += examples
##currently empty: SUBDIRS += test_allvxl
##needs libvgui-vrml from oxl: SUBDIRS += apps/vxl_vrml_viewer
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

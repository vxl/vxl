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

# multi-platform GUI toolkit
SUBDIRS += vgui

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# global examples and tests
ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

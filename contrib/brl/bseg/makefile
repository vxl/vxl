# This is brl/bseg/makefile
# main package for segmentation-related code and applications

# segmentation feature detection library
SUBDIRS += sdet

# segmentation view library
SUBDIRS += segv

# float image operations
SUBDIRS := brip

# segmentation application executables
SUBDIRS += sbin

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# segmentation tests
ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

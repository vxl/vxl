# This is brl/bseg/makefile
# main package for segmentation-related code and applications

# float image operations
SUBDIRS += brip

# segmentation feature detection library
SUBDIRS += sdet

# video processors
SUBDIRS += vpro

# tracking
SUBDIRS += strk

# markov random fields
SUBDIRS += bmrf

# misc 
#SUBDIRS += bcons

# segmentation view library
SUBDIRS += segv

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

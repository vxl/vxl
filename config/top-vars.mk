# Module: top-vars.mk
# Purpose: Ensure major environment variables are set properly
#   Sets $(configdir) to local area if it contains a config directory 

#----------------------------------------------------------------------
# Set IUEROOT if not set.
#----------------------------------------------------------------------
ifeq ($(strip $(IUEROOT)),)
xall::
	@echo " " 1>&2
	@echo "ERROR: You must define IUEROOT" 1>&2
	@echo " " 1>&2
	false

    IUEROOT := ENVIRONMENT_MUST_DEFINE_IUEROOT
endif


#----------------------------------------------------------------------
# Set IUELOCALROOT to $IUEROOT if not set.
#----------------------------------------------------------------------
ifeq ($(strip $(IUELOCALROOT)),)
  IUELOCALROOT := $(IUEROOT)
endif

#----------------------------------------------------------------------
# Set IUEOSTYPE if not set.
# Warn if both are set to different values
#----------------------------------------------------------------------
ifeq ($(strip $(IUEOSTYPE)),)
  # anything to avoid shelling...
  ifeq ($(OS),Windows_NT)
    IUEOSTYPE := win32
  else
    IUEOSTYPE := $(shell echo `uname -s``uname -r | cut -d. -f1`)
  endif
endif

#----------------------------------------------------------------------
# Set configdir to IUELOCALROOT/config if exists, else to IUEROOT/config
configdir := $(firstword $(wildcard $(IUELOCALROOT)/config $(IUEROOT)/config))

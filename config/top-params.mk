# Module: top-params.mk
# Purpose: Top-level target makefile
#   All client makefiles should include this file,
#   or $(configdir)/top.mk in order to use
#   the target make rules.

ifeq ($(strip $(MAKE_VERSION)),)
This makefile requires a recent (3.69+) version of GNU Make.
endif

ifndef IUE_TOP_PARAMS_INCLUDED 

# Look for top-vars.mk
topdir := $(firstword $(wildcard $(IUEROOT)/config/top-vars.mk))
ifeq ($(strip $(topdir)),)
xall::
	@echo "top-params.mk:$.: no $$IUEROOT/config/top-vars.mk found" ; false
else
topdir := $(IUEROOT)/config
endif

include $(topdir)/top-vars.mk

include $(configdir)/params.mk

IUE_TOP_PARAMS_INCLUDED := 1

endif

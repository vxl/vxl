# Module: top.mk
# Purpose: Top-level IUE makefile
#   All client makefiles should include this file,
#   $(configdir)/top.mk in order to use
#   the IUE make rules.

ifeq ($(strip $(MAKE_VERSION)),)
This makefile requires a recent (3.69+) version of GNU Make.
endif
# Look for top-params.mk
ifeq ($(strip $(IUEROOT)),)
xall::
	@echo " " 1>&2
	@echo "ERROR: You must define IUEROOT" 1>&2
	@echo " " 1>&2
	false

endif
include $(IUEROOT)/config/top-params.mk
include $(configdir)/top-rules.mk

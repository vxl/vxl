# Module: top-rules.mk
# Purpose: Top-level target makefile
#   All client makefiles should include this file,
#   or $(configdir)/top.mk in order to use
#   the target make rules.

ifeq ($(strip $(MAKE_VERSION)),)
This makefile requires a recent (3.69+) version of GNU Make.
endif


ifndef IUE_TOP_PARAMS_INCLUDED
You forgot to include $${IUEROOT}/config/top-params.mk
else

include $(IUEROOT)/config/top-vars.mk

include $(configdir)/rules.mk

# include any platform specific parameters which the local directory requires
-include $(OS)-$(COMPILER).mk

endif

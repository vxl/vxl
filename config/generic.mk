include $(configdir)/params.mk
ifndef NORULES
include $(configdir)/rules.mk
endif

# include any platform specific parameters which the local directory requires
-include $(OS)-$(COMPILER).mk

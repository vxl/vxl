link-static := -Wl,-B,static
link-dynamic := -Wl,-B,dynamic
no-link-dynamic := 

include $(configdir)/generic-irix.mk
soname_opt = -Wl,-soname,$(shlibname)

RPATHLIBDIRS = $(strip $(LIBDIRS))
ifeq ($(BUILD),noshared)
RPATHLIBDIRS = $(strip $(filter-out %.noshared,$(LIBDIRS)))
endif
rpath = -Wl,-rpath,$(subst $(spc),:,$(RPATHLIBDIRS))

LDFLAGS += -Wl,-woff,1,-woff,15,-woff,56,-woff,84,-woff,85,-woff,107,-woff,134

# include generic gcc stuff that works for any system with gcc
include $(configdir)/generic-gcc.mk
debug = -g1

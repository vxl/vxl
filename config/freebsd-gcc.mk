RPATHLIBDIRS = $(strip $(LIBDIRS))
ifeq ($(BUILD),noshared)
RPATHLIBDIRS = $(strip $(filter-out %.noshared,$(LIBDIRS)))
endif
# hesitantly included for FreeBSD from the linux file...
# this bit here is used to replace spaces with : 
# the rpath on linux needs to be like that
rpath = -Wl,-R$(subst $(spc),:,$(RPATHLIBDIRS))

MAKE_ARLIB = $(RM) $@ && ar qcv $@ 
USE_LIB_VERSION := 1
link-static := -Xlinker -Bstatic
link-dynamic := -Xlinker -Bdynamic
lib_dl := 
# include generic gcc stuff that works for any system with gcc
include $(IUEROOT)/config/generic-gcc.mk

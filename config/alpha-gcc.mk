RPATHLIBDIRS = $(strip $(LIBDIRS))
ifeq ($(BUILD),noshared)
RPATHLIBDIRS = $(strip $(filter-out %.noshared,$(LIBDIRS)))
endif
rpath = -Wl,-rpath,$(subst $(spc),:,$(RPATHLIBDIRS))
soname_opt = -Wl,-soname,$(shlibname)
MAKE_ARLIB :=
MAKE_ARLIB_CMD := ar qcv
link-static :=
link-dynamic :=
APP_CCFLAGS += # -DSYSV
ifndef TJ_GCC30
APP_CCFLAGS += -D_XOPEN_SOURCE_EXTENDED=1
endif

# include generic gcc stuff that works for any system with gcc
include $(configdir)/generic-gcc.mk
debug :=
ifdef TJ_GCC30
MAKE_SHLIB := $(CC) -shared -Wl,-expect_unresolved,'*'
else
MAKE_SHLIB := $(CC) -shared -Wl,-no_excpt,-expect_unresolved,'*'
endif

ifdef TJ_GCC30
lib_m := -lm -lpthread
endif

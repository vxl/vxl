ifeq ($(HAS_FRESCO),1)
 ifneq (,$(strip $(TJ_EGCS)))
  # This (mis)optimization must currently be disabled under 
  # Linux because Fresco hits the bug all the time...
  C++ := $(C++) -fno-vtable-thunks
 endif
endif

RPATHLIBDIRS = $(strip $(LIBDIRS))
ifeq ($(BUILD),noshared)
RPATHLIBDIRS = $(strip $(filter-out %.noshared,$(LIBDIRS)))
endif
# This bit here is used to replace spaces with colons.
# The rpath on linux needs to be like that.
rpath = -Wl,-rpath,$(subst $(spc),:,$(strip $(RPATHLIBDIRS)))

MAKE_ARLIB = $(RM) $@ && ar qcv $@ 
ifeq ($(IU_COMPILER),egcs)
link-static := -Wl,-Bstatic
link-dynamic := -Wl,-Bdynamic
lib_dl := -ldl
else
link-static := -Xlinker -Bstatic
link-dynamic := -Xlinker -Bdynamic
lib_dl := -ldl -rdynamic
endif
lib_c := -lc

IUE_LD_ALLEXTRACT := -Wl,--whole-archive
IUE_LD_ALLEXTRACT_OFF := -Wl,--no-whole-archive

ifeq "$(HAS_GLUT)" "1"
GLUT_XILIB      := -lXi
endif

# include generic gcc stuff that works for any system with gcc
include $(configdir)/generic-gcc.mk

optimize := -O3 -march=pentium4 -mfpmath=sse -DNDEBUG

ifdef TJ_EGCS
#0. Adding -O or -O2 to GeneralUtility/DEX/makefile generated link errors
#   when using EGCS 1.1.1 on Linux.  So, turn optimization off for EGCS.
#1. You must be joking. Turn it off in GeneralUtility/DEX/makefile if you must.
#2. No, you're not joking. egcs optimization screws up all sorts of code.
#3. Well, then, only do this for EGCS, not for gcc 2.95 !

# capes@robots Feb 2001 - Having optimization turned off is daft.
# If somebody discovers a repeatable problem when using optimization with
# egcs-1.1.2 then please make it known. 
# optimize := 
endif

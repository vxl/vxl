RPATHLIBDIRS = $(strip $(LIBDIRS))
rpath = -Wl,+b:
#or: rpath =  -Wl,+b,$(subst $(spc),:,$(RPATHLIBDIRS))
MAKE_ARLIB :=
MAKE_ARLIB_CMD := ar qcv
LIB_EXT := sl
link-static :=
link-dynamic := 
# -E on the hp linker causes the symbols in the main to be exported
# to the shared libraries, this is needed in TestDriver and tjrun
lib_dl := -ldld -Wl,-E -Wl,+s

# This is needed to get the libm that matches math.h
LIBDIRS += /lib/pa1.1
# include generic gcc stuff that works for any system with gcc
include $(configdir)/generic-gcc.mk
pic := -fPIC

OS_DEFINES := 
rpath = 

MAKE_ARLIB = rm -f $@ && ar qcv $@ 

link-static := -Bstatic
link-dynamic := -Bdynamic

lib_sockets := -lnsl
lib_dl := -ldl

soname_opt = 
USE_LIB_VERSION := 1

SunArchitecture := 1

# include generic gcc stuff that works for any system with gcc
include $(configdir)/generic-gcc.mk

# jb  added this by hand, as an experiment
MAKE_SHLIB = /bin/ld -assert pure-text

# redefine pic here.  IUE libs are compiled with PIC, and sunos4 linker
# doesn't allow mixing of pic and PIC.
pic := -fPIC


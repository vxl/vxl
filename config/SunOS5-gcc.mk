OS_DEFINES := -DSVR4 -DSYSV -DSOLARIS -DNeedFunctionPrototypes=1
RPATHLIBDIRS = $(strip $(LIBDIRS))
ifeq ($(BUILD),noshared)
RPATHLIBDIRS = $(strip $(filter-out %.noshared,$(LIBDIRS)))
endif
rpath = $(RPATHLIBDIRS:%=-R%)

MAKE_ARLIB = rm -f $@ && ar qcv $@ 

link-static := -Xlinker -Bstatic
link-dynamic := -Xlinker -Bdynamic

lib_sockets := -lnsl -lsocket
lib_dl := -ldl

profile:= -fprofile-arcs -ftest-coverage

soname_opt = -h $(shlibname)

SolarisArchitecture := 1

ifdef off
ifdef HAVE_GNU_LD
IUE_LD_ALLEXTRACT := -Wl,--whole-archive
IUE_LD_ALLEXTRACT_OFF := -Wl,--no-whole-archive
else
# native linker can do it on 2.6 upwqards
ifeq (,$(findstring $(OS_MINOR),1 2 3 4 5))
IUE_LD_ALLEXTRACT := -Wl,-z,allextract
IUE_LD_ALLEXTRACT_OFF := -Wl,-z,defaultextract
else
# pcp says "fsm says the following works on kate and who am I to disagree :)" 
# Well I'd be more than a teeny bit surprised if they do sez awf
IUE_LD_ALLEXTRACT := -Wl,-z,nodefs  # allow undefined symbols
IUE_LD_ALLEXTRACT_OFF := -Wl,-z,defs   # disallow undefined symbols
endif
endif
endif

ifeq "$(HAS_GLUT)" "1"
GLUT_XILIB	:= -lXi
endif

# # Transitive linking works:
# HAS_SH_LINK := 1
#fsm@robots.ox.ac.uk: no it doesn't, if libstdc++ is an archive
#library, because gcc will try to link the (.o) objects in libstdc++.a
#into the shared library. this results in many undefined symbols.

# include generic gcc stuff that works for any system with gcc
include $(configdir)/generic-gcc.mk

pic := -fPIC
MAKE_SHLIB := gcc -shared -nostdlib 
STRIP := echo # There are problems running stripped versions of the executables on solaris


# File: generic-gcc.mk

# This file defines generic gcc parameters that are hard to guess using
# configure.  Anything defined in here should work on all systems that use gcc.

force_c++ := -x c++

MAKEDEPEND := $(C++) -M
PREPROC_FLAG := -E -dD

# fsm: It suppresses the linking of standard startup files as well, it
# seems, as initialization of library statics. Why would we want that?
#ifeq "$(TJ_GCC27)" "1"
# This works on Solaris 2.5 / GCC 272 and EGCS 1.1.1
#MAKE_SHLIB := $(C++) -shared
#              ^^^^^^
# It doesn't work if libstdc++ is an archive library.
# For the last time: [1] use g++ to link executables, to get libstd++.
#                    [2] use gcc to link shared libraries, to avoid libstdc++.
MAKE_SHLIB := $(CC) -shared
#else
# These do not work on Solaris 2.5 / GCC 272
# MAKE_SHLIB := $(C++) -shared -nostdlib
# MAKE_SHLIB := $(C++) -shared
#endif

# define IUE_EXPAND_LD_UNDEF(symbol)
IUE_EXPAND_LD_UNDEF = -Wl,-u,"$(symbol)"



# Use "=" rather than ":=" as PURIFY is not known yet
ifeq "$(TJ_GCC30)" "1"
MAKE_EXE = $(PURIFY) $(C++)
AC_STDCXX_LIB := 
else
MAKE_EXE = $(PURIFY) $(CC)
AC_STDCXX_LIB := -lstdc++
endif
pic := -fpic
PIC := -fPIC
wall := -Wall -Wnested-externs -Wstrict-prototypes -Wmissing-prototypes -Wpointer-arith
#wall += -Wcast-qual -Wcast-align -Woverloaded-virtual -Winline -Wshadow # annoying warnings

ccflags += -fno-math-errno

# no longer needed with gcc 3.0 ? (and causes virtual memory boost)
ifneq "$(TJ_GCC30)" "1"
ccflags :=  -finline-functions
else
# cannot use -fimplicit-templates without -finline-functions
#USE_IMPLICIT_TEMPLATES :=
endif

ifneq "$(TJ_GCC27)" "1"

# Set binary-incompatible options here
# rick and bill: EGCS 1.1.1 vtable thunks have problems with Fresco
# PVr: removed -fno-vtable-thunks; only needed by Linux' EGCS, see linux-egcs.mk
ifdef TJ_EGCS
#fsm:C++ += -fguiding-decls
endif

ifndef USE_EXCEPTIONS
ifneq (1,$(strip $(GNU_LIBSTDCXX_V3)))
C++ += -fno-exceptions
endif
endif

## it's fun to turn this on.
#C++ += -fcheck-memory-usage

## Do not recognize `classof', `headof', `signature', `sigof' or `typeof' as keywords.
#C++ += -fno-gnu-keywords

# Decide whether or not to compile with rtti.
ifndef USE_RTTI
C++ += -fno-rtti
else
C++ += -frtti
endif

# awf added Wno-unused to workaround Store2DTemplate problem.
#wall += -Wno-unused

# -Wno-return-type is good, cos' -Wreturn-type is expensive and it warns about most of X11.
# fsm: it's bad because it doesn't warn about the line
# IMSTANTIATE_VECTOR(int*);
# , but silently treats it as a function declaration (it's a typo).
#wall += -Wno-return-type 
wall += -Wreturn-type 

# fsm. these are good if you think mixing 'int' and 'unsigned' is not so bad :
wall += -Wno-sign-compare -Wno-sign-promo

# warn about non-virtual dtor in class with virtual methods.
wall += -Wnon-virtual-dtor

# warn about overloaded virtuals whose signatures do not match those in the base class.
#wall += -Woverloaded-virtual

# #pragma ident common on solaris
ifdef TJ_EGCS
wall += -Wno-unknown-pragmas # this is an unknown option for gcc 2.8.1
endif

endif # TJ_GCC27

# -ggdb is a bad idea on some machines as it requires -lc_g which doesn't exist
debug = -g

# fsm
ifneq (,$(strip $(TJ_GCC295)))
optimize := -O3
endif

# capes
ifneq (,$(strip $(TJ_EGCS)))
optimize := -O3
endif

ifneq (,$(strip $(TJ_GCC30)))
optimize := -O3
no_implicit_templates := 
else
no_implicit_templates := -fno-implicit-templates
endif
implicit_templates := -fimplicit-templates -DINSTANTIATE_TEMPLATES

USING_GCC_COMPILER=1
OS_DEFINES += -DNOREPOS

ifeq ($(OS),"OSF1V4")
wall :=
endif

########################
#
# NOTES
#
########################
#
# Currently tested on version 7.2 compiler under irix6.3
#  using the n32 setting
#
#
# For MIPSpro 6.2 upwards there are actually 3 different compilers supported
# CC -32 (or -o32), CC -n32 and CC -64
# The last two use the new SGI ABI and also have slightly
#  different front ends. e.g. bool is not a predefined data type
#  under the old ABI. 
#
# There is some confusion between what is supported under 
# the old ABI in MIPSpro 7.2 and older compilers - particularly
# pre version 7.0. We don't have access to any except 
# version 4.0 and I am loathe to even try using that.
#
# MIPSpro 7.2.1  now supports standard conforming libraries
#		 and the ANSI standard string class 
#
# MIPSpro 7.2    Oxford standard SGI compiler
#                
# MIPSpro 7.1    does not have following keywords "typename",
#                "explicit", "namespace", and "mutable"
#
# MIPSpro 6.2    newer ABI introduced on this version
#
#
# From the relnotes for MIPSpro 7.2
# 
# The n32 and 64-bit versions are based on release 2.34 of the
# Edison frontend.
#
# The 32-bit version is based on release 2.29 of the Edison
# frontend.
#
#
# o32 bugs
#--------------------------
#   ld crashes with -multigot 
#
#
#
# Version 7.1 feature diffs
#--------------------------
#   Unrecognized flag ( ) in "-no_auto_include"
#
#
#
# Version 7.1 bugs
#--------------------------
#   Having a templated operator declaration, and a 
#   separatate definition in the same (header)
#   file causes the compiler front-end to crash 
#   when the definition is declared inline and but the
#   initial declaration is not. 
#
#
# 
#

########################
#
# LOCAL SGI SETTINGS
#
########################

# these include the compiler versions and the
# specification of the cpu (e.g. MIPS R4000) and 
# the instruction code to be generated
# (e.g. MIPS III).

# Not all combinations of flags are valid 
# for example : 
#   CC-o32 will only accept MIPS = 1 or 2
#   CC-n32 and CC-64 will only accept MIPS = 3 or 4
#  

SGI_CC_MAJOR = 7
SGI_CC_MINOR = 2
PROCESSOR = R4000
MIPS = 3
STRIP := true

# check for settings in local-irix.mk files

-include $(IUEROOT)/etc/local-irix.mk
-include $(IUELOCALROOT)/etc/local-irix.mk


########################
#
# GENERAL
#
########################

SGIArchitecture := 1
#fsm: these should not be set by default.
#HAS_MOTIF = 1
#HAS_MOTIFGL = 1
#HAS_INVENTOR = 1

ifeq ($(COMPILER),CC)
  ifneq ($(SGI_ABI),)
    COMPILER := CC$(SGI_ABI)
  endif
endif
ifeq ($(COMPILER),CC-64)
  64bit = 1
  COMPILER_FLAG = -64
endif
ifeq ($(COMPILER),CC-n32)
  32bit = 1
  COMPILER_FLAG = -n32
endif
ifeq ($(COMPILER),CC-o32)
  ABI = old
  COMPILER_FLAG = -o32
else
  ABI = new
endif  

########################
#
# DEPENDENCIES
#
########################

# fsm@robots : the old way worked for me, but the new one doesn't. The symptom is this 
# error message which appears for the first time when depending in VRML_IO. what goes
# wrong is that Qv/QvString.C #includes QvString.h between <..> and not "..". but the 
# real question is why it was trying to preprocess a .C file when making dependencies 
# for another .C file.
#-- Making dependencies for VRML_IO_read_topology.C       ........ due to VRML_IO_read_topology.C
#"/data/target/46/dev/target/SpatialObjects/Qv/QvString.C", line 1: error(1005): 
#          could not open source file "QvString.h"
#  #include <QvString.h>
#                       ^
#
#1 catastrophic error detected in the compilation of "VRML_IO_read_topology.C".
#Compilation terminated.

ifeq ($(ABI), new)
  CPP_MAKEDEPEND = 1
  MAKEDEPEND = $(IUE_PERL) $(IUEROOT)/Scripts/Perl/depend.pl -obj_ext o -relobj $(RELOBJDIR) -compiler '$(C++)' -E 
else
  MAKEDEPEND = $(C++) -M 
endif

########################
#
# COMPILATION
#
########################

no_implicit_templates := -ptnone -no_prelink
implicit_templates := -ptused -no_prelink -DINSTANTIATE_TEMPLATES

ifeq ($(ABI), new)

  ifeq ($(SGI_CC_MAJOR),7)
    ifneq ($(SGI_CC_MINOR),1)
      auto_include_flag = -no_auto_include
    endif
  endif

  C++ = CC $(COMPILER_FLAG) $(MACH_SPEC) $(no_implicit_templates) -ptv -LANG:exceptions=OFF -LANG:ansi-for-init-scope=ON $(auto_include_flag)

  CC = cc $(COMPILER_FLAG) $(MACH_SPEC)
  AS = as $(COMPILER_FLAG)
else
  C++ = CC $(COMPILER_FLAG) $(MACH_SPEC) $(no_implicit_templates) -ptv
  CC = cc $(COMPILER_FLAG) $(MACH_SPEC)

  # Declare that templates are instantiated using sgi-massage-templates
  PRAGMA_INSTANTIATE_TEMPLATES = 1

endif


OS_COMPILE.S = $(AS) -nocpp $(source_for_object) $(extraflags_for_object) $(OUTPUT_OPTION)

####################
# Compiler flags

pic:= -KPIC
debug = -g
debug_for_optimize := -g3
optimize := -O2
profile := -pg
IUE_PASS_TO_LINKER := -Wl,

### --> Please document below any warning which you turn off here!
ifeq ($(ABI),new)
  wall := -fullwarn -woff 1007,1188,1209,1257,1314,1355,1373,1375,1498,1506,1682,1201,1468,1555,3201,3150,1234#,1116,1681,1174
else 
  wall := -fullwarn -woff 3108,3672,3937,3577,3209,3461#,3161
endif

####################
# CPP defines

ifeq ($(ABI), new)
  NO_BOOL = 0
else
  DEFINES += -DNO_STD_BOOL=1
  NO_BOOL = true
endif

OS_DEFINES += -D__SGI_CC_7 -D__SGI_CC -DNOREPOS -DSVR4 -DSYSV

####################
# MACH_SPEC flags


ifeq ($(ABI),new)
  ifeq ($(MIPS),4)
    MACH_SPEC = -mips4
  endif
  ifeq ($(MIPS),3)
    MACH_SPEC = -mips3
  endif
  ifeq ($(PROCESSOR),R5000)
    MACH_SPEC += -r5000
  endif
  ifeq ($(PROCESSOR),R8000)
    MACH_SPEC += -r8000
  endif 
  ifeq ($(PROCESSOR),R10000)
    MACH_SPEC += -r10000
  endif 
endif




########################
#
# LINKING
#
########################

ifeq ($(ABI), new)
LINKER = /usr/lib/ld -call_shared -init _main -fini _fini -no_unresolved -rdata_shared \
	 -transitive_link -demangle -elf -_SYSTYPE_SVR4 \
	 -multigot -LANG:exceptions=OFF -LANG:ansi-for-init-scope=ON -cxx \
	 -wall -woff 1 -woff 15 -woff 56 -woff 84 -woff 85 -woff 134 \
	 -nocount $(COMPILER_FLAG) $(mach_link_start) -count

LDOPTS = $(mach_link_path) -lm -lc -nocount -dont_warn_unused \
	 -lCsup -lC -Bdynamic \
	 -lc $(mach_link_end) -warn_unused
else
LINKER = /usr/lib/ld -no_unresolved \
	 -elf -_SYSTYPE_SVR4 \
	 -require_dynamic_link _rld_new_interface -cxx \
	 -woff 1 -woff 56 -woff 84 -woff 85 -woff 134 \
	 -nocount /usr/lib/crt1.o /usr/lib/c++init.o -count

LDOPTS = $(mach_link_path) -lm -nocount -dont_warn_unused -lC -warn_unused -lc /usr/lib/crtn.o
endif


MAKE_SHLIB := $(C++) -shared -update_registry $(IUELOCALROOT)/so_locations.$(OS)-$(COMPILER) # -Wl,-default_delay_load -Wl,-read
MAKE_EXE = $(PURIFY) $(LINKER)
FORCE_LOAD := -force_load
MAKE_ARLIB = rm -f $@ && ar qcv $@
MAKE_ARLIB_CMD := ar qcv $@ 

LINK_INTO_TMP := 1  # IRIX linker is incredibly slow over NFS.

soname_opt = -soname $(shlibname)

#implicit_templates := -ptnone -prelink -DINSTANTIATE_TEMPLATES
#no_implicit_templates := -ptnone

link-static := -Wl,-B,static
link-dynamic := -Wl,-B,dynamic
no-link-dynamic := 
IUE_LD_ALLEXTRACT = -all
IUE_LD_ALLEXTRACT_OFF = -notall

RPATHLIBDIRS = $(strip $(LIBDIRS))
ifeq ($(BUILD),noshared)
RPATHLIBDIRS = $(strip $(filter-out %.noshared,$(LIBDIRS)))
endif
rpath = -rpath $(subst $(spc),:,$(RPATHLIBDIRS))
LDFLAGS :=

mach_link_start :=
mach_link_end :=
mach_link_path :=

ifeq ($(COMPILER), CC-o32)
  mach_link_start += /usr/lib/crt1.o /usr/lib/c++init.o
  mach_link_path += -L/usr/lib
  mach_link_end += /usr/lib/crtn.o
endif


ifeq ($(COMPILER),CC-n32)
  ifeq ($(MIPS),3) 
    mach_link_start += -mips3
    ifeq ($(PROCESSOR),R5000)
      mach_link_path += -L/usr/lib32/mips3/r5000 
    endif
    ifeq ($(PROCESSOR),R8000)
      mach_link_path += -L/usr/lib32/mips3/r8000 
    endif
    ifeq ($(PROCESSOR),R10000)
      mach_link_path += -L/usr/lib32/mips3/r10000 
    endif
    mach_link_start += /usr/lib32/mips3/crt1.o /usr/lib32/c++init.o
    mach_link_path += -L/usr/lib32/mips3 -L/usr/lib32
    mach_link_end += /usr/lib32/mips3/crtn.o
  endif
  ifeq ($(MIPS),4) 
    mach_link_start += -mips4    
    ifeq ($(PROCESSOR),R5000)
      mach_link_path += -L/usr/lib32/mips4/r5000
    endif
    ifeq ($(PROCESSOR),R8000)
      mach_link_path += -L/usr/lib32/mips4/r8000
    endif
    ifeq ($(PROCESSOR),R10000)
      mach_link_path += -L/usr/lib32/mips4/r10000
    endif
    mach_link_start += /usr/lib32/mips4/crt1.o /usr/lib32/c++init.o
    mach_link_path += -L/usr/lib32/mips4 -L/usr/lib32
    mach_link_end += /usr/lib32/mips4/crtn.o
  endif
endif

ifeq ($(COMPILER),CC-64)
  ifeq ($(MIPS),3) 
    mach_link_start += -mips3    
    ifeq ($(PROCESSOR),R5000)
      mach_link_path += -L/usr/lib64/mips3/r5000
    endif
    ifeq ($(PROCESSOR),R8000)
      mach_link_path += -L/usr/lib64/mips3/r8000
    endif
    ifeq ($(PROCESSOR),R10000)
      mach_link_path += -L/usr/lib64/mips3/r10000
    endif
    mach_link_start += /usr/lib64/mips3/crt1.o /usr/lib64/c++init.o
    mach_link_path += -L/usr/lib64/mips3 -L/usr/lib64
    mach_link_end += /usr/lib64/mips3/crtn.o
  endif
  ifeq ($(MIPS),4) 
    mach_link_start += -mips4    
    ifeq ($(PROCESSOR),R5000)
      mach_link_path += -L/usr/lib64/mips4/r5000
    endif
    ifeq ($(PROCESSOR),R8000)
      mach_link_path += -L/usr/lib64/mips4/r8000
    endif
    ifeq ($(PROCESSOR),R10000)
      mach_link_path += -L/usr/lib64/mips4/r10000
    endif
    mach_link_start += /usr/lib64/mips4/crt1.o /usr/lib64/c++init.o
    mach_link_path += -L/usr/lib64/mips4 -L/usr/lib64
    mach_link_end += /usr/lib64/mips4/crtn.o
  endif
endif

ifeq ($(ABI), new)
  ifdef $(32bit)
    MOTIF_LIB_DIR := /usr/lib32
  else
    MOTIF_LIB_DIR := /usr/lib64
  endif
endif

ifeq ($(OS),IRIX5)
lib_dl := -ldl
lib_c := -lsun -lmalloc -lc
else
lib_dl := 
lib_c := -lc -lmalloc
endif

# # Transitive linking works:
# HAS_SH_LINK := 1
# It may work, but it's not really that useful. Most of the time it
# just causes problems like failure to link shared libraries because
# of missing search paths. We don't need it on UNIX.

# Warnings for -n32 and -64 compilers 
#
# 1007: unrecognized token ... for "@" in template-decls
# 1116: non-void function should return a value. 
#        turning this off is a bit dodgy but alloc.h causes
#        it to pop-up everywhere. tedious. 
# 1174: variable "i" was declared but never referenced
# 1188: type qualifier is meaningless on cast type
# 1201: trailing comma is nonstandard
# 1209: constant in if()  ; e.g., as result of "#define for if (0) {} else for"
# 1234: compiler warns about missing access specification in derivation.
#       this is pointless as it is valid C++ and can never lead to bugs even
#       in the case where it is truly an error.
# 1257: NULL reference is not allowed
# 1314: value copied to temporary, reference to temporary used
# 1355: extra ";"
# 1373: has an operator new() but no default operator delete()
# 1375: nonvirtual base class destructor
# 1401: implicit int
# 1468: inline function "" cannot be instantiated
# 1506: implicit conversion from "unsigned long" to "long":  rounding, sign extension, or loss of accuracy may result
# 1555: external/internal linkage conflict
# 1681: f does not match "Base::f" -- virtual function override intended?
# 1682: overloaded virtual function "Image::GetSection" is only partially overridden in class "BandedImage"
# 3150: class "x" has no copy assignment operator; bitwise copy operator added
# 3201: parameter "x" was never referenced 

# Warnings for -o32 compiler 
#
# -woff sets the warnings *not* to be printed.
# 3108: unrecognized preprocessing directive
# 3262: function "" was declared but never referenced
# 3577: stl problem
# 3672: Mangling of signed character does not match cfront name mangling
# 3937: overloaded virtual function "" is only partially overridden in class ""
# 3161: ???
# 3209: non-void function should return a value
# 3461: extra ";"

# linker warnings
# ld: WARNING 1: Unknown option: fullwarn (ignored).
# ld: WARNING 15: multiply defined: (symb_name) in f1.o and f2.o (2nd definition ignored).
# ld: WARNING 56: Invalid warning number (3108).
# ld: WARNING 84: /homes/az/pcp/work/lib/irix6-CC-o32/libTestLib.so is not used for resolving any symbol.
# ld: WARNING 85: definition of _calloc in /usr/lib/libc.so preempts that definition in /usr/lib/libmalloc.so.
# ld: WARNING 127: Two shared objects with the same soname, /usr/lib32/mips3/libm.so and /usr/lib32/libm.so, have been been linked.
# ld: WARNING 134: weak definition of memalign in /usr/lib/libc.so preempts that weak definition in /usr/lib/libmalloc.so

HAS_WINAPI := 1

HAS_MFC := 1
# dwh - removed these since they are wrong for VC60, and they should be
# defined in either site.mk or etc/user.mk if you need them.
#MFCINCS := C:/PROGRA~1/DevStudio/VC/mfc/include
#MFCLIBS := C:/PROGRA~1/DevStudio/VC/mfc/lib

HAS_OLE := 1
# dwh - removed these since they are wrong for VC60, and they should be
# defined in either site.mk or etc/user.mk if you need them.

#OLEINCS := C:/PROGRA~1/DevStudio/VC/include
#OLELIBS := C:/PROGRA~1/DevStudio/VC/lib

HAS_ATL := 1
# dwh - removed these since they are wrong for VC60, and they should be
# defined in either site.mk or etc/user.mk if you need them.
#ATLINCS =  C:/PROGRA~1/DevStudio/VC/atl/include
#ATLINCS += C:/PROGRA~1/DevStudio/SharedIDE/Template/atl
#ATLLIBS :=

HAS_SGI_OPENGL := 1
SGI_OPENGLINCS = /olgsdk/include/gl
SGI_OPENGLLIBS := /oglsdk/lib

HAS_FRESCO := 1
# the user may override FRESCO_DRIVE in site.mk or etc/user.mk
FRESCO_INC_DIR := $/$/D$/Fresco
FRESCO_LIB_DIR := $/$/D$/Fresco$/Build$/Win95$/lib

HAS_OPENGL := 1
no_x := 1
LIB_EXT := dll
AR_EXT := lib
MKDIR_P := mkdir -p
debug = -MDd -Zi -DDEBUG -Fd$(OBJDIR)/
optimize :=
optimize_for_speed := -Ox -Ob2 -DNDEBUG
debug_c7 := -MTd -Z7 -Od -D_DEBUG -DDEBUG
# dwh - optimization and debug flags to be used within studio stuff
ide_optimize := -Ox -Ob2 -DNDEBUG
ide_debug := -FR -Zi -Od -D_DEBUG -DDEBUG
ide_OS_DEFINES := -DWIN32 -D_WINDOWS -DVC50
profile := 
RANLIB=:
LORDER=:

# there is no -l in win32, you just put the lib name and use -LIBPATH
# there should be no prefix as this will have to be used to specify
# the name of the library
LIB_PREFIX :=

# Variable: ENTRY
#  Set this to define an alt. entry point

# however to link a library it should end with a .lib
LINK_POSTFIX :=.lib
LINK_LIB_FLAG := 
LINK_PREFIX :=
#LIB_PATH := -link -LIBPATH:
LIB_PATH := -LIBPATH:
LDFLAGS += -link -debug $(ENTRY) /STACK:10000000

pic = -DBUILD_DLL -DBUILDING_$(LIBRARY)_DLL
PIC = $(pic)
C++ := CL.exe -TP
OBJBASE:=
CC  := CL.exe -TC -nologo -DWIN32

RESOURCE_COMPILER  := RC.exe
RES_EXT := res

IDL_COMPILER := MIDL.exe
TLB_EXT := tlb

DEFINES += -DWIN32 -DNOREPOS -D_WINDOWS -DVC50   -DDYNAMIC_GLOBALS -DWORDS_LITTLEENDIAN=1

ccflags := -nologo
# -W3 is warnings full
wall := -W3
# set up programs to be called
# following line commented out because mkdir fails if path exists already
#MKDIR_P := mkdir 

IUE_PERL := perl

# awf uncommented tjwd
TJWD := $(IUE_PERL) -I$(shell echo $(foreach file, win32bin, $(sys_or_iu_macro)) | sed -e "s@//\([A-Za-z]\)@\1:@g") -S tjwd.pl
# TJWD := :

STRIP := :
RM := rm -f
CP := cp
MAKE_SHLIB = cl.exe -nologo -LDd -MDd -Zi 

MAKE_EXE := CL.exe -nologo

SEARCH := $(IUE_PERL) -S grep.pl 

# the VC50 and VC60 compilers require a full path
# to the source code to get the debug information correctly
# This variable DOS_FULL_SRC_PATH is prepended to $(source_for_object)
# before it is passed to the compiler, with non-broken compilers,
# this is variable would be empty, and not used.
DOS_PWD = $(shell echo $(PWD) | sed -e "s@//\([A-Za-z]\)@\1:@g")
DOS_FULL_SRC_PATH = $(DOS_PWD)/
DOS_@ = $(shell echo $@ | sed -e "s@//\([A-Za-z]\)@\1:@g")

MAKE_ARLIB = lib -nologo -machine:IX86 -out:$(DOS_@)
CCDRIVER := $(C++)

# default for this should be yes
HAS_SH_LINK := 1
# bill, i switched this back for using bash as build shell instead of dos
# globbing does not work right in dos shell on win95
#RM:= del
RM := rm -f
#/ := \\
/ := /
OUTPUT_OPTION = -Fo$@
LINK_OUT_OPTION = -Fe$(DOS_@)

OBJ_EXT := obj
EXESUFFIX := .exe
# PROGSUFFIX is for .pure
PROGSUFFIX :=

INCDIRS += $(IUEROOT)/win32bin

CPP_MAKEDEPEND = 1
MAKEDEPEND = $(IUE_PERL) -S depend.pl -compiler $(C++) -extra_opt -nologo -relobj $(RELOBJDIR) -E 

lib_m :=
MAKE = make

no_implicit_templates :=
implicit_templates := -DINSTANTIATE_TEMPLATES

### Local Variables: ###
### mode:font-lock ###
### End: ###

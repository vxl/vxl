# 			    C/C++ COMPILER OPTIONS
# 
# 				-OPTIMIZATION-
# 
# /O1 minimize space                       /Op[-] improve floating-pt consistency
# /O2 maximize speed                       /Os favor code space
# /Oa assume no aliasing                   /Ot favor code speed
# /Ob<n> inline expansion (default n=0)    /Ow assume cross-function aliasing
# /Od disable optimizations (default)      /Ox maximum opts. (/Ogityb1 /Gs)
# /Og enable global optimization           /Oy[-] enable frame pointer omission
# /Oi enable intrinsic functions
# 
# 			       -CODE GENERATION-
# 
# /G3 optimize for 80386                   /Gy separate functions for linker
# /G4 optimize for 80486                   /Ge force stack checking for all funcs
# /G5 optimize for Pentium                 /Gs[num] disable stack checking calls
# /G6 optimize for Pentium Pro             /Gh enable hook function call
# /GB optimize for blended model (default) /GR[-] enable C++ RTTI
# /Gd __cdecl calling convention           /GX[-] enable C++ EH (same as /EHsc)
# /Gr __fastcall calling convention        /Gi[-] enable incremental compilation
# /Gz __stdcall calling convention         /Gm[-] enable minimal rebuild
# /GA optimize for Windows Application     /EHs enable synchronous C++ EH
# /GD optimize for Windows DLL             /EHa enable asynchronous C++ EH
# /Gf enable string pooling                /EHc extern "C" defaults to nothrow
# /GF enable read-only string pooling      /QIfdiv[-] enable Pentium FDIV fix 
# /GZ enable runtime debug checks          /QI0f[-] enable Pentium 0x0f fix
# 
# 				-OUTPUT FILES-
# 
# /Fa[file] name assembly listing file     /Fo<file> name object file
# /FA[sc] configure assembly listing       /Fp<file> name precompiled header file
# /Fd[file] name .PDB file                 /Fr[file] name source browser file
# /Fe<file> name executable file           /FR[file] name extended .SBR file
# /Fm[file] name map file
# 
# 				-PREPROCESSOR-
# 
# /C don't strip comments                  /FI<file> name forced include file
# /D<name>{=|#}<text> define macro         /U<name> remove predefined macro
# /E preprocess to stdout                  /u remove all predefined macros
# /EP preprocess to stdout, no #line       /I<dir> add to include search path
# /P preprocess to file                    /X ignore "standard places"
# 
# 				  -LANGUAGE-
# 
# /Zi enable debugging information         /Zl omit default library name in .OBJ
# /ZI enable Edit and Continue debug info  /Zg generate function prototypes
# /Z7 enable old-style debug info          /Zs syntax check only
# /Zd line number debugging info only      /vd{0|1} disable/enable vtordisp
# /Zp[n] pack structs on n-byte boundary   /vm<x> type of pointers to members
# /Za disable extensions (implies /Op)     /noBool disable "bool" keyword
# /Ze enable extensions (default)
# 
# 				-MISCELLANEOUS-
# 
# /?, /help print this help message        /V<string> set version string
# /c compile only, no link                 /w disable all warnings
# /H<num> max external name length         /W<n> set warning level (default n=1)
# /J default char type is unsigned         /WX treat warnings as errors
# /nologo suppress copyright message       /Yc[file] create .PCH file
# /Tc<source file> compile file as .c      /Yd put debug info in every .OBJ
# /Tp<source file> compile file as .cpp    /Yu[file] use .PCH file
# /TC compile all files as .c              /YX[file] automatic .PCH
# /TP compile all files as .cpp            /Zm<n> max memory alloc (% of default)
# 
# 				   -LINKING-
# 
# /MD link with MSVCRT.LIB                 /MDd link with MSVCRTD.LIB debug lib
# /ML link with LIBC.LIB                   /MLd link with LIBCD.LIB debug lib
# /MT link with LIBCMT.LIB                 /MTd link with LIBCMTD.LIB debug lib
# /LD Create .DLL                          /F<num> set stack size
# /LDd Create .DLL debug libary            /link [linker options and libraries]


OBJBASE:=

# awf added multiple-inclusion guard
ifneq ($(config_mk_set_objbase_only),1)
HAS_WING := 1
# removed these; should be defined in site.mk if you need them.
#WINGHOMES := $/Wing
#WING_INC_DIR := $(WINGHOMES)/Include
#WING_LIB_DIR := $(WINGHOMES)/Lib

HAS_WINAPI := 1

HAS_MFC := 1
# dwh - removed these since they are wrong for VC60, and they should be
# defined in either site.mk or etc/user.mk if you need them.
#MFC_INC_DIR := C:/PROGRA~1/DevStudio/VC/mfc/include
#MFC_LIB_DIR := C:/PROGRA~1/DevStudio/VC/mfc/lib

HAS_OLE := 1
# dwh - removed these since they are wrong for VC60, and they should be
# defined in either site.mk or etc/user.mk if you need them.
#OLE_INC_DIR := C:/PROGRA~1/DevStudio/VC/include
#OLE_LIB_DIR := C:/PROGRA~1/DevStudio/VC/lib

HAS_ATL := 1
# dwh - removed these since they are wrong for VC60, and they should be
# defined in either site.mk or etc/user.mk if you need them.
#ATL_INC_DIR =  C:/PROGRA~1/DevStudio/VC/atl/include
#ATL_INC_DIR += C:/PROGRA~1/DevStudio/SharedIDE/Template/atl
#ATLLIBS :=

HAS_SGI_OPENGL := 1
SGI_OPENGLINCS = /olgsdk/include/gl
SGI_OPENGLLIBS := /oglsdk/lib

HAS_FRESCO := 1
ifeq "1" "$(HAS_FRESCO)"
# the user may override FRESCO_DRIVE in site.mk or etc/user.mk
ifndef FRESCO_DRIVE
FRESCO_DRIVE=C
endif
FRESCO_INC_DIR := $/$/$(FRESCO_DRIVE)$/Fresco
FRESCO_LIB_DIR := $/$/$(FRESCO_DRIVE)$/Fresco$/Build$/Win95$/lib
endif

# Remove # from next lines if you have GLUT
#HAS_GLUT := 1
#GLUT_INC_DIR := $/$/C$/glut
#GLUT_LIB_DIR := $/$/C$/glut

HAS_OPENGL := 1
no_x := 1
LIB_EXT := dll
AR_EXT := lib
MKDIR_P := mkdir -p
# debug = -MTd -Zi -DDEBUG -Fd$(OBJDIR)/
debug = -MDd -W3 -Gm -GX -ZI -Od  -FD -GZ  -DDEBUG -Fd$(OBJDIR)/ 
# opts:
# -MDd 	-lMSVCRTdebug
# -W3   Warning level 3
# -Gm   Incremental rebuild
# -GX   No exceptions
# -ZI   Edit-and-continue
# -Od   No optimizations
# -FD   Dump file dependencies
# -GZ   Runtime debugging
# -Fd   PDB dir

optimize :=
optimize_for_speed := -Ox -Ob2 -DNDEBUG
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
LIB_PATH := -LIBPATH:
LDFLAGS += -link -debug $(ENTRY) /INCREMENTAL:NO /STACK:10000000

pic = -DBUILD_DLL -DBUILDING_$(LIBRARY)_DLL
PIC = $(pic)
C++ := CL.exe -TP
CC  := CL.exe -TC -nologo -DWIN32

RESOURCE_COMPILER  := RC.exe
RES_EXT := res

IDL_COMPILER := MIDL.exe
TLB_EXT := tlb

# -Tp is not a define it tells the compiler to assume all files are cplusplus
# putting this flag in with the defines makes it so that it will only be used
# for compiles and not links.  If it is used for a link, then it will think
# the .obj file is a .C file!
DEFINES += -DWIN32 -DNOREPOS -D_WINDOWS -DVC50 -DVC60  -DDYNAMIC_GLOBALS -DWORDS_LITTLEENDIAN=1

ccflags := -nologo
# -W3 is warnings full
wall := -W3
# set up programs to be called
# following line commented out because mkdir fails if path exists already
#MKDIR_P := mkdir 

STRIP := :
RM := rm -f
CP := cp
MAKE_SHLIB = cl.exe -nologo -LDd -MDd -Zi 

MAKE_EXE := CL.exe -nologo

SEARCH := $(IUE_PERL) -S grep.pl 

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

TJWD = $(IUE_PERL) -x $(sys_or_iu_Scripts)/Perl/tjwd

no_implicit_templates :=
implicit_templates := -DINSTANTIATE_TEMPLATES

endif # config_mk_set_objbase_only
### Local Variables: ###
### mode:font-lock ###
### End: ###

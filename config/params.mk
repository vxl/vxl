#############################################################################
## Generic makefile for gnumake
# Author: awf, Feb 97
#
#     PLEASE CONSULT ME ABOUT CHANGES TO THIS FILE.
#     It's not that I'm proprietorial about it, it's just that things
#     do tend to get duplicated or done wrong.  It's best therefore that
#     they go through one channel.
#
#     awf@robots.ox.ac.uk
#
# Notes:
#
# * Most makefiles assume sourcefile can be inferred from object file, but 
#   often this leads to great complexity.  In this, you generally supply the
#   sources that will do something, rather than the objects.
# 
# * This makefile should be included after all variables have been set,
#   i.e. generally at the end of the makefile.  This is to make it somewhat
#   easier to debug, by using := instead of = for the variables.
#   Any extra makefile targets should generally be put after this is included,
#   so that they have access to computed variables such as LDPATH etc.
#
# * It is intended that variables in UPPERCASE are "external" symbols,
#   lowercase names are internal to this file.
#
# * By criminy!  Gmake includes the spaces at the end of variable values
#   dir = fred   # sets dir to "fred   "
#
# * ":=" variables are expanded once
#   "="  variables are more like macros -- any $.. references are evaluated as late as possible.
# 
# * I would strongly recommmend font-lock-mode on this file.
#
# * A good way to debug make is to
#   make |& egrep '(Must remake)|^[a-zA-Z]'
#
# * Be very sparing in the use of $(shell). It's expensive, $(wildcard) is often just as good,
#   and anyway, it often refers to things that should be in configure.
#
#            ####################
#             INCLUDED MAKEFILES
#            ####################
#  * Please keep this section up to date *
#  
#  top.mk  # 2 lines, defines $(configdir)
#    generic.mk
#      identify-os.mk    # No user-servicable parts inside, just sets $(OS)
#      config-$(OS)-$(COMPILER).mk # ./configured from config.mk.in
#      	 $(OS)-$(COMPILER).mk  # Things that can't be ./configured about this OS
#      site.mk		 # *SITE* preferences.
#      user.mk           # Even more local preferences
#      build-$(BUILD).mk # Sets code-generation flags for different build types.
#      makevars.mk	 # defines variables users makefiles may use


########################################################################
#  If users makefiles want to set these variables, the setting must occur
#  before including top-params.mk or top.mk.  Note that assignments
#  must use = or += rather than := if the value depends on a variable
#  defined by params.mk (such as $(IUE_USES)
#	USES
#	IULIBS
#
#	IU_COMPILER
#	IU_COMPILER_cc
#	VXL
#	LIBDEST
#	IULIBHOME

#######################################################################
#  These are set by config, but can be overridden by values set in user's
#  makefiles before including params.mk
#	C++
#	NO_BOOL
#	IVINSTALLED_DIR
#	ACE	
#	OMNIORB
#	XGL
#	TCL
#	MSQL
#	MYSQL
#	VTKHOME

# Option: QUIET
# Set QUIET= to see various bits of make in operation, e.g.
# depend commands
QUIET=@

#############################################################################
## Set generic flags.  All os-dependent configuration should be done here,
## so there should be no "ifeq ($(OS))" anywhere else in the makefiles.
## The first tasks are to set the OS and COMPILER variables, so that
## the appropriate config file can be found.

#ifeq ($(strip $(MAKE_VERSION)),)
#v:=$(shell echo 'error: This makefile requires a recent (3.69+) version of GNU Make.' 1>&2)
#endif


# 0. Set default COMPILER variable.
#    If make is called with COMPILER=xxx on the command line,
#    this will be overridden, as will calling make native.target
#    or make photon.target  make sure this is first so that
#    it can be overriden in identify-os.mk  (see win32 stuff)
#    
# Option: COMPILER
# Set the name of the compiler to use.  This will be the name passed
# to the toplevel "configure" with any spaces stripped, and will be
# added to $(OBJDIR) so that objects produced by different compilers
# go in different directories.  Takes its value from IU_COMPILER if that
# is set in the environment.
ifdef IU_COMPILER
 COMPILER := $(IU_COMPILER)
 CC := $(IU_COMPILER_cc)
else
 ifeq (win32,$(OS))
   COMPILER := VC50
 else
   COMPILER := gcc
 endif
endif

# 1. Set the OS variable
include $(configdir)/identify-os.mk

# MACRO "sys_or_iu_macro": 
# Use this macro as $(foreach file,XXX,$(sys_or_iu_macro)), where XXX is a
# list of top-level package names, to obtain a list of full paths to the
# given packages. The macro selects the one from the IUELOCALROOT if it
# exists and the one from IUEROOT otherwise.
sys_or_iu_macro = $(firstword $(wildcard $(IUELOCALROOT)/$(file) $(IUEROOT)/$(file)))


# 1a. Find IUE Scripts directory
sys_or_iu_Scripts := $(foreach file,Scripts,$(sys_or_iu_macro))


# 2. Set OS/compiler/site-dependent variables.
#    These are determined by configure, creating a config-$(OS)-$(COMPILER).mk
#    file from config.mk.in, and include the defaults just below, plus:
#      programs/command rules:
#      	LEX, RANLIB, LORDER, C++, CC, MAKEDEPEND,
#      	MAKE_SHLIB, MAKE_ARLIB
#      	
#      directories:
#      	CXX_HDRDIR, OBJBASE
#      	
#      flags:
#      	DEFINES, OS_DEFINES, pic, wall, ccflags,
#      	rpath, link-static, link-dynamic, CCOUTFLAG

# VERBOSE
ifeq ($(VERBOSE),1)
SILENT :=
else
SILENT := @
endif

# Initial defaults for most systems, to be overwritten by
# config-$(OS)-$(COMPILER).mk
TOUCH_OLD := touch
MKDIR_P := mkdir -p
IUE_PERL := perl
TJWD := $(IUE_PERL) $(sys_or_iu_Scripts)/Perl/tjwd
SEARCH := grep
RM := rm -f 
STRIP := true #strip
LN := ln -f -s

# -------------------- suffixes and prefixes --------------------

# Config: LIB_PREFIX
# This is preprended to each library name.
LIB_PREFIX := lib

# Config: LIB_EXT
# The file extension for shared libraries, E.g. "so" for unix, "dll" for windows. Is "sl" used?
LIB_EXT := so

# Config: AR_EXT
# The file extension for archive libraries. E.g. "a" for unix, "lib" for windows.
AR_EXT := a

# Config: OBJ_EXT
# The extension used for object files. E.g. "o" for unix and "obj" for windows.
OBJ_EXT := o

# Config: EXESUFFIX
# This is appeded to executables. E.g. "" for unix and ".exe" for windows.
EXESUFFIX :=

# This is appended to the end of each library. It's "" for unix and ".lib" for windows.
LINK_POSTFIX :=



# Handy make vars
null :=
spc := $(null) $(null)
EMPTY_COMMAND := $(IUE_PERL) -e "0;"
/ := /

# -------------------- compiler and linker flags --------------------

# flag to tell the compiler to turn on debugging.
debug = -g
debug_for_optimize := -g

# flag to tell the compiler to turn on optimization.
optimize := -O3

# flag to tell the compiler to turn on profiling.
profile := -pg

# how to tell the linker to link against certain common libraries.
#lib_dl := -ldl
#lib_sockets := -lnsl -lsockets
lib_m := -lm
#lib_c := -lc

#
iue-no-print-directory := --no-print


# This is used to tell the linker to search for libraries in a given directory, e.g.
# $(C++) $(LIB_PATH)/usr/local/lib ...etc...
LIB_PATH := -L

# This is used at the begining of each library. E.g. for unix, libm.{a,so} is given as -lm
LINK_PREFIX := -l
LINK_LIB_FLAG := -l

# This is used to tell the compiler the name of the object file. 
# To do "-o OBJECT" with microsoft is "/FoOBJECT"  file.C
# The flag is /Fo and file.o must touch it!
# Use OUTPUT_OPTION as that has precedent in SunOS make.
#
# To be used in a rule where the object file is the target $@.
OUTPUT_OPTION = -o $@

# Gets even more fun, the output option for executables is different than obj files!
# Even more fun, to build a shared library we use link which uses "-out", but
# to build a program we use cl.exe which uses another option.
#
# To be used in a rule where the executable/share library is the target $@.
LINK_OUT_OPTION = -o $@
LINKER_OUT_OPTION = -o $@

# this is a linker config flag.
USE_LIB_VERSION := 0

# -------------------- commands to make thing --------------------

# Config: MAKE_EXE
# The command used to generate executables. On most platforms this is $(PURIFY) $(C++)
MAKE_EXE = $(PURIFY) $(C++) 

# Config: MAKE_SHLIB
# The command used to generate shared libraries. On most platforms this is $(C++) -shared
MAKE_SHLIB := $(C++) -shared

# Config: MAKE_ARLIB
# Command used to generate archive libraries. To be used in a rule where the target
# is the library to make.
MAKE_ARLIB = rm -f $@ && ar qcv $@ 

######################### including config.mk.in #########################
## Include config.mk.in as modified by the configure script
# Remember, it's really including config.mk.in (actually it
# isn't *really* doing that...).


# For even make -k to fail, if the system has not been configured yet,
# it would be nice if it could print out a message. Instead it will send
# them to this line in this file, hello unconfigured folks!  WAH
ifeq (,$(wildcard $(configdir)/config-$(OS)-$(COMPILER).mk))
xall::
	echo your system has not been configured for $(OS)-$(COMPILER)
	echo run configure from $$IUEROOT before make can be run
endif


## To get the OBJBASE for the IUEROOT and the IUELOCALROOT, we first
# include IUEROOT/config/config.mk to get the OBJBASE for the IUEROOT, and
# then IUELOCALROOT/config/config.mk to get the OBJBASE for the IUELOCALROOT.

# Both the IUEROOT and IUELOCALROOT files are included,
# The former is used to set SYS_OBJBASE, the latter for everything else.
config_mk_set_objbase_only := 1
include $(IUEROOT)/config/config-$(OS)-$(COMPILER).mk
SYS_OBJBASE := $(OBJBASE)
config_mk_set_objbase_only := 0
include $(configdir)/config-$(OS)-$(COMPILER).mk

#############################################################################
## Include makevars.mk which contains variables that may be used
## in defining variables such as USES that params.mk looks at

include $(configdir)/makevars.mk

#########################
## Now all configurable parameters have been set.  No config-specifics
## beyond this point.  It still remains to determine the BUILD and OBJDIR,
## which are more preference variables than config variables.
#########################

# 3. Set BUILD using the same technique as was used for COMPILER.
#    Default build at most sites is shared, so set that here.
#    To override the default, reset BUILD in site.mk

# Option: BUILD
# Define what type of code to produce.  This causes the inclusion of
# config/build-$(BUILD).mk which sets flags such as optimization,
# debugging, profiling and whether to produce shared or static libraries.
# Common values are "shared", "noshared", "profiled", "shared-opt".
BUILD := shared

# 3. A "well known" build type -- i.e. one that is mentioned in the BUILDTYPES
#     variable may be made just using make build.
BUILDTYPES := noshared profiled shared shared-opt

#----------------------------------------------------------------------
# 4. Set OBJDIR variable.

# Variable: OBJDIR
# A slight misnomer, OBJDIR is set to a string that describes the type of
# executables that the build will produce, e.g. "solaris" or
# "irix6-CC-n32.profiled".  The full pathnames are in $(RELOBJDIR) and
# $(ABSOBJDIR).

# The build dir is always obj/$(name_of_architecture), regardless of
# whether or not CPU or IUEOSTYPE is set in the environment.
# Thus, object files produced by different compilers go in different
# directories because this is necessary for multiplatform builds (and
# awf will fight anyone who says otherwise).
# E.g. obj/solaris-gcc
OBJDIR := obj/$(OS)-$(COMPILER)

# Default OBJDIR is OBJDIR.build unless build is shared.
# This is a bit horrid, but good for old TargetJr compatibility.
ifneq "$(BUILD)" "shared"
  OBJDIR := $(OBJDIR).$(BUILD)
endif

# Internal: tj_libdir
# Set to the relative name of the library directory. It is
# obtained from $(OBJDIR) by replacing 'obj' with 'lib'.
tj_libdir := $(OBJDIR:obj/%=lib/%)

# Config: OLD_SUBDIR_STYLE
# An alternative to this, when OLD_SUBDIR_STYLE is set, is to build into
# subdirectory $(IUEOSTYPE)-$(COMPILER) (i.e., without "obj/") and to
# put libraries into lib.$(OBJDIR) :
ifdef OLD_SUBDIR_STYLE
  OBJDIR := $(IUEOSTYPE)
  ifneq "$(COMPILER)" "gcc"
    OBJDIR := $(OBJDIR)-$(COMPILER)
  endif
  ifneq "$(BUILD)" "shared"
    OBJDIR := $(OBJDIR).$(BUILD)
  endif
  tj_libdir := lib.$(OBJDIR)
endif

#----------------------------------------------------------------------
# Config: OBJBASE
# If set, all objects and libraries go to a shadow tree rooted at OBJBASE.
# Elso, objects and libraries go into the source tree.
ifneq ($(strip $(OBJBASE)),)
IUELOCALROOT_libbase := $(OBJBASE)
IUEROOT_libbase := $(SYS_OBJBASE)
else
IUELOCALROOT_libbase := $(IUELOCALROOT)
IUEROOT_libbase := $(IUEROOT)
endif

#--------------------------------------------------------------------------------

# 4a. Set directory for installed executables
#	Use environment values for this path so programs outside of
#	make can find it.
IUE_BINDIR := $(IUELOCALROOT)/bin/$(IUEOSTYPE)


# 5. Set user options

# Option: LINK_INTO_TMP
# The IRIX linker is incredibly slow over NFS, so setting this option causes
# linking to be directed into /tmp, speeding things up by a factor between 1.5
# and 3.  For solaris on a fast local disk, this slows the link down by about
# 20%.  However it is on by default because it also means that remaking a shared
# library while the application is running does not cause it to crash.

# LINK_INTO_TMP :=

# Option: NOTEST
# If defined, causes all tests (and other nonessential stuff) not to be built.
# This saves mucho disk space and is faster, but is an incredibly bad idea :)
# [Insert tales of much saved bacon due to Tests here ].
ifeq ($(BUILD),noshared)
  NOTEST = echo
endif

# Option: USE_TESTLIB
# Set to zero to ignore TestLib
ifndef USE_TESTLIB
ifneq (,$(strip $(wildcard ${IUELOCALROOT}/TestLib)))
USE_TESTLIB := 1
endif
endif

#############################################################################
# Include site-specific definitions.
# The site.mk file overrides any of the previous defaults:
#  BUILD, OBJDIR, IUEOSTYPE, COMPILER
# It can even override config parameters although that would probably be
# silly.  Second, it sets variables such as extra compiler flags, peculiar
# opengl locations, etc.

#############################################################################

### include build definition
include $(configdir)/build-$(BUILD).mk

### include site-specific defines.
include $(configdir)/site.mk

# fsm : linux gmake sometimes says "no rule to make target xxx.mk" for 
# the -includes below. It is usually caused by a double-inclusion of the
# referring makefile.

### include any auto generated defines
-include $(IUELOCALROOT)/config/config-auto.mk

### include user specific defines if they are there 
# The user defines re-override site.mk definitions if supplied.
-include $(IUELOCALROOT)/etc/user.mk


#--------------------------------------------------------------------------------

# Get pwd relative to IUELOCALROOT. (We assume the user knows the best
# filesystem name to use, accounting for automounters etc, and has used
# it in IUELOCALROOT).

# If in $IUEROOT/vxl/vgui/impl, this will be "vxl/vgui/impl"
PKGWD := $(shell $(TJWD))
#(fsm:not used anywhere)PKGWD.. := $(patsubst %$/,%,$(dir $(PKGWD)))

# make absolute paths in IUELOCALROOT :
#(fsm:not used anywhere)PWD := $(IUELOCALROOT)$/$(PKGWD)
#(fsm:not used anywhere)PWD.. := $(IUELOCALROOT)$/$(PKGWD..)

# Rick@aai - PKGDIR is the USES entry (i.e. package name) that forms a prefix of 
# the current tjwd.
# E.g. if in $IUEROOT/vxl/vgui/impl, this will be "vxl"
#
# Use this for location of libs if not using a central libdir. Do not use PKGWD..
# because that won't work for packages with deep library directories and
# multi-directory package names (e.g., 
#   CoordSys-IUE/IUE-CS/CartesianSystem in package CoordSys-IUE and
#   Examples-IUE/GUIExample/IUE-GUIExample in Examples-IUE/GUIExample)
PKGDIR := $(foreach use,$(USES),$(findstring $(use),$(filter $(use)/%,$(PKGWD)/)))
PKGDIR := $(firstword $(PKGDIR))
# If no current USES entry forms a prefix of tjwd, use the first part of $(PKGWD) :
ifeq ($(strip $(PKGDIR)),)
  PKGDIR := $(firstword $(subst /, ,$(PKGWD)))
endif


# If $(PKGWD) is empty we are outside the targetjr build tree. There are 
# three kinds of makefiles :
#   1. makefile in IUEROOT
#   2. makefile in IUELOCALROOT
#   3. makefile in client directory
# and whether or not and which OBJBASE to use depends on where we are.
ifneq "$(PKGWD)" ""
  PWD_INSIDE_IUE:=1
else
  PWD_INSIDE_IUE:=0
endif



# Internal: CENTRAL_LIBDIR
# If defined, all libraries go into $(IUELOCALROOT)/$(tj_libdir), otherwise they
# go into package-specific subdirectories.  This is the default if IUEOSTYPE is not set
# in the environment (recommended), or on windows.

# FIXME: Temporarily broken for IUE as some applications have libraries
# with the same name (e.g. Morse/Windows, Site/Windows).  These zap each other
# when moved into the centre.  The correct fix a mon avis is to make applications
# put their libraries elsewhere.

ifdef CENTRAL_LIBDIR
xall::
	@echo "* params.mk: WARNING, CENTRAL_LIBDIR is set."
	@echo "* If you want to use a central libdir, you should"
	@echo "* specify it at configure time with the option"
	@echo "* --with-single-libdir=yes"
endif

ifeq ($(OS),win32)
  # on windows, we always use a central libdir. or do we?
  CENTRAL_LIBDIR:=1
else
  # on other systems, it depends on the configure value of SINGLE_LIBDIR :
  ifneq ($(strip $(SINGLE_LIBDIR)),)
    CENTRAL_LIBDIR:=1
  endif
endif

ifdef CENTRAL_LIBDIR
xall::
	@echo "using CENTRAL_LIBDIR"
endif

############################################################
# check for configure defined stuff here

# This macro expands package name 'file' to the correct library directory :
sys_or_iu_macro_lib = $(firstword $(wildcard $(IUELOCALROOT_libbase)/$(file) $(IUEROOT_libbase)/$(file)))

# Config: NO_BOOL
# NO_BOOL is set to true by configure if your compiler does not support the bool type.
# In that case, we use 'unsigned' instead.
ifeq (true,$(NO_BOOL))
  DEFINES += -Dbool=unsigned -Dfalse=0u -Dtrue=1u
endif

# Config: HAS_BROKEN_AS
# HAS_BROKEN_AS is set to 1 by configure if your assember cannot handle long lines.
ifeq (1,$(strip $(HAS_BROKEN_AS)))
  DEFINES += -DVCL_BROKEN_AS=1
endif
## HAS_BROKEN_AS is always defined!!!
##ifdef HAS_BROKEN_AS
##  DEFINES += -DVCL_BROKEN_AS=1
##endif


#check to see if the system has IUE-Root files, if
# not assume HAS_IUE:= 
sys_or_iu_IUERoot := $(foreach file,Basics-IUE/IUE-Root/makefile ,$(sys_or_iu_macro))
ifeq (,$(sys_or_iu_IUERoot))
HAS_IUE :=
else
HAS_IUE := 1
endif

# HAS_IUE can be overridden by --with-tjonly configure option
ifeq (1,$(strip $(TJ_ONLY))$(strip $(VXL_ONLY)))
HAS_IUE :=
endif

ifeq (1,$(strip $(HAS_IUE)))
# * IUE
# Add -DIUE only if using IUE
DEFINES += -DIUE
endif

# pcp@robots
# set GNU_LIBSTDCXX_V3
ifeq ($(GNU_LIBSTDCXX_V3),1)
  DEFINES += -DGNU_LIBSTDCXX_V3
endif




################################################################################
################################################################################

# Include 3rd party library parameters that clients might want
# to refer to in their makefiles.
include $(configdir)/tpp-params.mk

################################################################################
################################################################################

# Compute relative and absolute objdirs (RELOBJDIR and ABSOBJDIR).
# In general, faraway object directories are linked into $(SRC)$/..$/$(tj_libdir) so
# that $(IUEROOT)/Package/lib/$(OS)-$(COMPILER) is still the entry in LIBDIRS


# If we are not in the IUE source tree, then objects will be placed under the
# current source directory, regardless of OBJBASE, SYS_OBJBASE.
# If we are in the IUE source tree, object placement depends on the values of
# OBJBASE and SYS_OBJBASE.

ifeq ($(PWD_INSIDE_IUE),1)
  ifdef OBJBASE
    IUE_send_to_objbase := 1
  endif
endif

# FIXME: using only OBJBASE and not SYS_OBJBASE, i.e. assumes we're 
# compiling in the IUELOCALROOT, but not the IUEROOT.
# FIXME: the ABSOBJDIR is never in IUELOCALROOT if the sources aren't.
ifdef IUE_send_to_objbase
  RELOBJDIR := $(OBJBASE)$/$(PKGWD)$/$(OBJDIR)
  ABSOBJDIR := $(OBJBASE)$/$(PKGWD)$/$(OBJDIR)
else
  RELOBJDIR := $(OBJDIR)
  ABSOBJDIR := $(IUELOCALROOT)$/$(PKGWD)$/$(OBJDIR)
endif

#--------------------------------------------------------------------------------

# Internal: default_LIBDEST
# Default final resting place of libraries

# If we are outside a targetjr build tree we want to place
# libraries relative to the currect directory, unless we are 
# using a central libdir.
ifdef CENTRAL_LIBDIR
  #FIXME: what if we have an IUELOCALROOT, but are building in the IUEROOT?
  default_LIBDEST := $(IUELOCALROOT_libbase)$/$(tj_libdir)
else
  ifeq ($(PWD_INSIDE_IUE),1)
    default_LIBDEST := $(IUELOCALROOT_libbase)$/$(PKGDIR)$/$(tj_libdir)
  else
    default_LIBDEST := $(tj_libdir)
  endif
endif

# special case for win32. is this really needed?
ifeq ($(OS),win32)
  default_LIBDEST := $(tj_libdir)
  ifndef LIBDEST
    LIBDEST := $(IUELOCALROOT_libbase)$/$(tj_libdir)
  endif
endif

## If LIBDEST is not defined (e.g. by the client makefile), set it here
ifndef LIBDEST
LIBDEST:=$(default_LIBDEST)
endif

### Quote any colons in LIBDEST (twice)
LIBDEST := $(subst :,\:,$(LIBDEST))
LIBDEST := $(subst :,\:,$(LIBDEST))

#--------------------------------------------------------------------------------

# check to see if the system has NITFImage or not
# is this a 3rd party package thing?
sys_or_iu_NITF := $(foreach file,Image/NITF/NITFImage.h,$(sys_or_iu_macro))
ifneq (,$(sys_or_iu_NITF))
NITF := -lNITF -lImageProcessing
DEFINES += -DHAS_NITF
endif

# Variable: AUX_SOURCE_DIRECTORY
# All source files (extensions .C, .cc or .cxx) in the directories listed
# in this variable are automatically added to SOURCES.
# Directory names are relative to the current makefile's location.
# Its value defaults to Templates.
# This variable can be overridden or appended to in a user makefile.
# See also the variable TEMPLATE_SOURCES
AUX_SOURCE_DIRECTORY := Templates

# Variable: TEMPLATE_SOURCES
# Template instantiations should reside in files with extension
# .C, .cc or .cxx
# in subdirectory Templates.  They are automatically added to SOURCES.
# This variable can be overridden or appended to in a user makefile.
TEMPLATE_SOURCES = $(foreach d,$(AUX_SOURCE_DIRECTORY),$(wildcard $d/*.C $d/*.cc $d/*.cxx))

ifndef PREPROC_FLAG
PREPROC_FLAG := -E
endif

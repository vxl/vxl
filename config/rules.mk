#############################################################################
## Generic makefile for gnumake
# Module: Default targets
# Author: awf, Feb 97
# Notes:
#     PLEASE CONSULT ME ABOUT CHANGES TO THIS FILE.
#     It's not that I'm proprietorial about it, it's just that things
#     do tend to get duplicated or done wrong.  It's best therefore that
#     they go through one channel.
#
#     awf@robots.ox.ac.uk
#
# Modifications:
#  010798 AWF Make PROGSUFFIX apply to TestDriver and MINI_PROG_SOURCES
#             (So that make PURIFY=... causes the purified exes to be built and tested).


# more help:
# I use foreach as a macro expander.
# I want to say stuff like

#  define mymacro(x) $(patsubst %,%.d,$(wildcard $x))
#  use $(mymacro arg)

# I do so using foreach as follows:
#
#  mymacro = $(patsubst %,%.d,$(wildcard $x))
#  use $(foreach x, arg, $(mymacro))


# Place this target first, as a "forward declaration" of the default target.

# Target: all
# The default target, makes any LIBRARY/PROGRAM/etc in the makefile
all: xall

# Target: help
# make help prints some useful info.
help:
	@echo "Available targets are :"
	@echo "   '<blank>'          : Same as make all"
	@echo "   all                : Same as make all.shared"
	@echo "   all.shared         : Recompile as necessary -- dynamic libraries gcc/g++"
	@echo "   all.noshared       : Recompile as necessary -- static libraries gcc/g++"
	@echo "   native.shared      : Recompile as necessary -- dynamic libraries native"
	@echo "   native.noshared    : Recompile as necessary -- static libraries native"
	@echo "   clean              : Remove all binaries and dependencies (not executables)"
	@echo "   dclean             : Remove all object file directories"
	@echo "   clean_obj_only     : Removes all .o files "
	@echo "   clean_prog         : Remove all binaries, not objs and deps (to cause a relink)"
	@echo "   echovar-VAR        : Print value of VAR in the makefile "
	@echo "   cpp-File.C         : Print output of preprocessor on file File.C "
	@echo " "
	@echo "See $(configdir)/README.vars for more info."


# Note that xall is a double-colon target.
# [can someone clearly explain double-colon here?]
# [perhaps via a simple example?]
xall::

# cancel implicit rules:
% : %.o
% : %.c
% : %.cc
% : %.C
% : %.p
% : %.f
% : %.F
% : %.r
% : %.s
% : %.S
% : %.mod
% : %.sh

# passthru targets to set compiler.
native: native.shared
native.%:
ifeq ($(OS),IRIX6)
	$(MAKE) BUILD=$(@:native.%=%) COMPILER=CC-n32
else
	$(MAKE) BUILD=$(@:native.%=%) COMPILER=CC CC=cc
endif

photon: photon.shared
photon.%:
	$(MAKE) BUILD=$(@:photon.%=%) COMPILER=KCC


# 3a. Passthru targets to set build.

# 3b. A "well known" build type -- i.e. one that is mentioned in the buildtypes
#     variable may be made just using make <build> or make all.<build>
$(BUILDTYPES) : % :
	$(MAKE) BUILD=$@

$(BUILDTYPES:%=all.%) : all.% :
	$(MAKE) BUILD=$(@:all.%=%)

# Hmm, had to dump this as it was trying to make all from all.$(OBJ_EXT):
#     Any build may be specified using make all.build
all.%:
	 $(MAKE) BUILD=$(@:all.%=%) $(iue-no-print-directory)

# Targets to ensure that $(RELOBJDIR) is made
dirstamp := $(RELOBJDIR)$/stamp

$(dirstamp):
	[ -d $(RELOBJDIR) ] || $(MKDIR_P) $(RELOBJDIR)
	$(TOUCH_OLD) $@

ifneq ($(RELOBJDIR),$(LIBDEST))
# Target to ensure that $(LIBDEST) is made
$(LIBDEST)$/stamp:
	[ -d $(LIBDEST) ] || $(MKDIR_P) $(LIBDEST)
	$(TOUCH_OLD) $@
endif

#############################################################################
#############################################################################
### Set include and link paths

#############################################################################
# Set derived variables, assuming using target
# Variable:
#
# This section sets the compiler flags from the userdefined variables
#  USES -- A list of package names in specific-to-general order.
#  SOURCES -- List of source files to compile to objects.
#  OBJECTS -- List of additional objects whose compilation is handled in the
#             makefile.  If you find you're using this variable  to do something
#             generic, such as compile e.g. a YACC or pascal file, please consider
#             adding the rules to this rules.mk.  Mail iue-help@aai.com if
#             you wonder how generic your rule will be.
#
# Compiler flags:
#  CCFLAGS -- Special code-generation flags.
#  INCDIRS -- Complete include-file search path, no -I flags.
#  DEFINES -- List of -D, -U flags for C preprocessor.
#
# Linking flags:
#  LIBDIRS -- Complete linker directory path, no -L flags.
#  LDLIBS -- Complete list of libraries to be loaded
#    LOCAL_LIBS -- First, depend on IUE
#    IULIBS     -- Second, portions of IUE selected.
#      IU3DLIBS -- Used within IULIBS to select 3D rendering libraries (based
#                  on USE_RENDERING, OPENGLRENDER variables).
#    STDLIBS    -- Third, based on values of variables USE_X11, USE_SOCKETS etc.
#
#
# To modify behavior from the make command line, define the following variables
# with var='value':
#  CMDCFLAGS	-- adds flags to C compilation commands
#  CMDCCFLAGS	-- adds flags to C++ compilation commands
#  CMDLDFLAGS	-- adds flags to linker commands
#
#  CMDCFLAGS1, CMDCCFLAGS1, CMDLDFLAGS1 -- like above variables, but adds
#		   flags to the beginning of the generated list of flags
#		   rather than to the end.
#

# Add sections in specific-to-general order

#######################################################

# Easy TRY_blah macros, handy for using a package when
# it is available, without having to conditionalize.
# E.g. one can use "TRY_blah := 1" instead of
# ifeq ($(HAS_blah),1)
#   USE_blah := 1
# endif
$(configdir)/try.mk: $(configdir)/rules.mk
	@echo "# fsm@robots.ox.ac.uk" > $@
	@echo "# this file is generated by rules.mk" >> $@
	@for pkg in OPENGL GLUT FLTK GTKGLAREA MOTIFGL JPEG MPEG MPEG2 TIFF PNG SDL HERMES; do \
	  echo "ifeq (\$$(strip \$$(TRY_$$pkg)),1)" >> $@; \
	  echo "  ifeq (\$$(strip \$$(HAS_$$pkg)),1)" >> $@; \
	  echo "    USE_$$pkg := 1" >> $@; \
	  echo "  endif" >> $@; \
	  echo "endif" >> $@; \
	  echo "" >> $@; \
	done

-include $(configdir)/try.mk

#######################################################

# Variable: IUE_PACKAGE_LIBRARIES
# A list of libraries derived from the USES variable.  For each package in
# $(USES), the file $(IUELOCALROOT)/Package/iulibs.mk is included, which
# adds a set of libraries to IUE_PACKAGE_LIBRARIES.  The files are included
# in reverse order, depending on the USES variable being specified in
# specific-to-general order.


# Set VXL to 1 if "vxl" is in USES. Else, add v3p to
# USES, for backwards compatibility -- fsm.
ifneq "" "$(findstring vxl,$(USES))"
VXL := 1
else
USES += v3p
endif


ifneq ($(strip $(USES)),)
iue_mkincs := $(foreach i,$(USES),$(firstword $(wildcard $(IUELOCALROOT)/$i/iulibs.mk $(IUEROOT)/$i/iulibs.mk)))
# reverse mkincs
# hmmm.  no need, multiple includes appear to be done in reverse anyway....
# iue_mkincs := $(foreach i,30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1,$(word $i,$(iue_mkincs)))
ifneq ($(strip $(iue_mkincs)),)
-include $(iue_mkincs)
endif
endif

#############################################################################
## 1. Defaults for LDLIBS, CCFLAGS.

# Variable: LDLIBS
# The list of -llibraries to link.  If not set, defaults to
#  -lTestLib $(LOCAL_LIBS) $(IULIBS) $(STDLIBS)

# This file is included after the makefile, so we override
# with the default only if a variable is not set.
ifeq (1,$(strip $(USE_TESTLIB)))
 ifeq (0,$(USE_DEFAULT_TESTLIB))
  USE_DEFAULT_TESTLIB :=
 else
  ifdef MAKE_SHARED
    LINK_TESTLIB = -lTestLib
    USE_DEFAULT_TESTLIB:=1      # add TestLib to LIBDIRS later
#   USES += TestLib
  endif 
 endif 
endif

# Variable: ald_libs
# The list of direct libraries to link.
# If given, auto-compute transitive closure of libraries.
ifneq ($(strip $(ald_libs)),)
  aldtmp := $(shell $(IUE_PERL) $(sys_or_iu_Scripts)/Perl/fsm-libsort.pl $(ald_libs) $(ald_libdeps))
  IULIBS += $(aldtmp:%=-l%)
endif

# Switch to ifndef so LDLIBS = $(VAR) is seen as NON empty
ifndef LDLIBS
  lib_tmp := $(shell $(IUE_PERL) $(sys_or_iu_Scripts)/Perl/remove_duplicates.pl $(IULIBS))
  LDLIBS = $(LOCAL_LIBS) $(lib_tmp)
endif

#############################################################################
## 2. Build STDLIBS and LIBDIRS.

# Select: STDLIBS
# The list of libraries generated by the USES variable and USE_* flags.

## 2.1 Add IUE packages, from the USES variable.
# Use INCDIRS and LIBDIRS without -I or -L in case some systems need them specified
# differently.  This also has the advantage that dirs can be added below, such as
# X11_INC_DIR, and systems which have it in /usr/include don't need to worry about
# the empty -I on the command line that a simple -I$(X11_INC_DIR) would leave.

# REMEMBER:  Order is significant in this section -- add USES checks in the order
# that they would finally be linked: specific to general.


# * Target packages from the USES variable.
# For each package in USES, pathsearch for it on $(IUELOCALROOT):$(IUEROOT)
# Warning don't use $/ in this line, it will not work for win32, on the bright side 
# -I paths for the compiler on win32 seem to work with mixed / and \ !

#################################################################################

#
# first, the incdirs :
#

# construct include path from the USES list :
SYS_OR_IUUSES := $(foreach file,$(USES),$(sys_or_iu_macro))

# test for and warn about missing packages :
uses_count := $(words $(USES))
comp_uses_count := $(words $(SYS_OR_IUUSES))
ifneq ($(uses_count), $(comp_uses_count))
# fsm : made some changes here to make the warning more readable and useful.
tmp := $(SYS_OR_IUUSES)
tmp := $(tmp:$(IUELOCALROOT)/%=%) $(tmp:$(IUEROOT)/%=%)
xall::
	@echo "rules.mk WARNING: some packages in USES were not found"
	@echo "*"
	@echo "*   Used packages :"
	@echo "* \""$(sort $(USES))"\""
	@echo "*   Found packages :"
	@echo "* \""$(sort $(tmp))"\""
	@echo "*   Missing packages :"
	@echo "* \""$(filter-out $(sort $(tmp)),$(sort $(USES)))"\""
	@echo "*"
endif

# Add the include paths computed from the USES variable to the INCDIRS variable now :
INCDIRS += $(SYS_OR_IUUSES)


#
# next, the libdirs :
#

# allow the client to link against libraries with different optimization
ifneq ($(strip $(LINK_WITH_SECOND_BUILD)),)
  # substitute the build type in $(tj_libdir) and call the result $(tj_libdir_other)
  tj_libdir_other := $(tj_libdir)
  ifneq "$(BUILD)" "shared"
    tj_libdir_other := $(subst .$(BUILD),,$(tj_libdir_other))
  endif
  ifneq "$(LINK_WITH_SECOND_BUILD)" "shared"
    tj_libdir_other := $(tj_libdir_other).$(LINK_WITH_SECOND_BUILD)
  endif
endif

# add library paths to LIBDIRS now :
ifdef CENTRAL_LIBDIR
  # add the central lib directory to the LIBDIRS variable.
  LIBDIRS += $(IUELOCALROOT_libbase)$/$(tj_libdir) $(IUEROOT_libbase)/$(tj_libdir)
  # if LINK_WITH_SECOND_BUILD is nonempty, add library paths for the second build.
  ifneq ($(strip $(tj_libdir_other)),)
    LIBDIRS += $(IUELOCALROOT_libbase)$/$(tj_libdir_other) $(IUEROOT_libbase)/$(tj_libdir_other)
  endif
else
  # add paths to non-central libraries.
  tmp_libs := $(foreach file,$(USES),$(sys_or_iu_macro_lib))
  LIBDIRS += $(tmp_libs:%=%$/$(tj_libdir))
  # again, allow client to link against second build.
  ifneq ($(strip $(tj_libdir_other)),)
    LIBDIRS += $(tmp_libs:%=%$/$(tj_libdir_other))
  endif
endif


# Variable: IU3DLIBS
# Set to the appropriate IUE libraries when USE_RENDERING is
# on.  Use it in your LDLIBS if you're linking the 3D stuff.
ifeq ($(strip $(USE_RENDERING)),1)
# * If use_rendering is on, then 
# we always want GenericView3D 
  IU3DLIBS += -lGenericView3D
  ifeq ($(strip $(HAS_OPENGL)),1)
     IU3DLIBS += -lOpenGLView3D -lGenericView3D
     USE_OPENGL := 1
  endif
  ifeq ($(strip $(HAS_XGL)),1)
     IU3DLIBS += -lXglView3D -lGenericView3D
     USE_XGL := 1
  endif
  ifeq ($(strip $(HAS_X11)),1)
     USE_X11 := 1
  endif
endif

# include site-uses.mk, if it exists.
-include $(configdir)/site-uses.mk

#############################################################
## 2.2 Add to LIBDIRS and IULIBS, based on USE_* variables.

# * COOL
ifneq "" "$(findstring COOL,$(USES))"
  IULIBS  += -lTJCOOL
endif

# * Numerics
ifneq "" "$(findstring Numerics,$(USES))"
  IULIBS  += -liue-math
  USE_NETLIB := 1
endif

ifneq "" "$(findstring COOL,$(USES))$(findstring Numerics,$(USES))"
ifeq "" "$(findstring -DNOREPOS,$(OS_DEFINES))"
  DEFINES += -DNOREPOS 
endif
endif

# IUE on Win32 with BUILD=noshared needs -ladvapi32 -luser32
ifeq "1" "$(strip $(HAS_IUE))"
ifeq (win32,$(strip $(OS)))
ifeq ($(strip $(BUILD)),noshared) 
USE_WINAPI := 1
endif
endif
endif


# sigh.....  Continuing from earlier logic.
ifneq (,$(USE_DEFAULT_TESTLIB))
  ifndef CENTRAL_LIBDIR
    LIBDIRS += $(foreach file,TestLib,$(sys_or_iu_macro_lib))/$(tj_libdir)
    ifneq ($(strip $(tj_libdir_other)),)
      LIBDIRS += $(foreach file,TestLib,$(sys_or_iu_macro_lib))/$(tj_libdir_other)
    endif
  endif
endif


# Select: VXL
# Set this to 1 to use the VXL package.
ifeq ($(strip $(VXL)),1)
  INCDIRS += $(foreach file,vxl,$(sys_or_iu_macro))
  INCDIRS += $(firstword $(wildcard $(IUELOCALROOT) $(IUEROOT)))# temporarily, for <vcl/vcl_*.h>
  # add stdc++ for those who need it.
  STDLIBS += $(AC_STDCXX_LIB)
endif

# this is the location of the autoconfed header included by vcl_unistd.* :
VXL_CONFIG_DIR := $(IUEROOT)/vcl/config.$(OS)-$(COMPILER)
#OS_DEFINES += -DVXL_CONFIG_H=\"$(VXL_CONFIG_HDR)\"
INCDIRS += $(VXL_CONFIG_DIR)

SYS_OR_IU_CONFIG_IUE_LIBDIRS := $(foreach file,vcl,$(sys_or_iu_macro_lib))
SYS_OR_IU_CONFIG_IUE_INCDIRS := $(firstword $(wildcard $(IUELOCALROOT) $(IUEROOT)))

ifndef CENTRAL_LIBDIR
  LIBDIRS += $(SYS_OR_IU_CONFIG_IUE_LIBDIRS)/$(tj_libdir)
  ifneq ($(strip $(tj_libdir_other)),)
    LIBDIRS += $(SYS_OR_IU_CONFIG_IUE_LIBDIRS)/$(tj_libdir_other)
  endif
endif

# Select: NO_STDLIBS
# Set this to 1 to not link with vcl

ifndef NO_STDLIBS
STDLIBS += -lvcl
endif

ifeq (1,0)# THIS IS NOW IN vcl/config.$(IUEOSTYPE)/vcl_config_manual.h
# Use the compiler's built-in STL rather than the one supplied in vcl/emulation
 ifeq ($(strip $(USE_NATIVE_STL)),1)
   OS_DEFINES += -DVCL_USE_NATIVE_STL=1
 endif
 ifeq ($(strip $(USE_NATIVE_STL)),0)
   OS_DEFINES += -DVCL_USE_NATIVE_STL=0
 endif
endif
CONFIG_IUE_INCDIR += $(SYS_OR_IU_CONFIG_IUE_INCDIRS)/vcl

# Add stdc++ -- only needed for iostream (cout,cerr,operator<<()) -- should disappear
STDLIBS += $(AC_STDCXX_LIB)


#----------------------------------------
# 3rd party libraries
#

# For each USE_$thing, add the relevant include and library paths.

# If USE_$thing is set but the build is not configured for $thing, add
# USE_$thing to the variable err_CANNOT_BUILD_HERE and the error will
# be dealt with at the end.
ifndef err_CANNOT_BUILD_HERE
err_CANNOT_BUILD_HERE :=
endif
# Set this variable to 1 to ensure that the library will not be built on windows.
err_CANNOT_BUILD_ON_WINDOWS := 0
# Set this variable to 1 to ensure that the library is only built on windows.
err_MUST_BUILD_ON_WINDOWS := 0


ifdef USE_B_STATIC
STDLIBS += $(link-static)
endif

# Include "rules" to respond to USE_blah := 1 statement in the client makefile :
include $(configdir)/tpp-rules.mk





#
# Switch off "-Bdynamic" or "-Bstatic" for the remaining libs:
#
STDLIBS += $(no-link-dynamic)


# Set to 1 to use X11 libraries
# Select: USE_X11
ifeq ($(strip $(USE_X11)),1)
 ifeq ($(strip $(HAS_X11)),1)
  INCDIRS += $(X11_INC_DIR)
  C_INCDIRS += $(X11_INC_DIR)
  LIBDIRS += $(X11_LIB_DIR)
  STDLIBS += -lXmu
  STDLIBS += -lXext -lX11
  USE_SOCKETS := 1
 endif
endif

# Select: USE_SOCKETS
ifeq ($(strip $(USE_SOCKETS)),1)
  STDLIBS += $(lib_sockets)
endif

# Dynamic linker library.
STDLIBS += $(lib_dl)

# C math and run-time libraries
STDLIBS += $(lib_m) $(lib_c)



#
# catch errors
#

# this is where we catch any USE_*s that have been put into err_CANNOT_BUILD_HERE
ifneq ($(strip $(err_CANNOT_BUILD_HERE)),)
MINI_SO_TESTS :=
PROGRAM :=
SOURCES :=
MINI_SO_SOURCES :=
MINI_PROG_SOURCES :=
LIBRARY :=
xall::
	@echo "* rules.mk: ERROR"
	@echo "* Cannot build here because your system is not configured for :"
	@echo "* $(err_CANNOT_BUILD_HERE)"
	@echo "* To build here you must re-run configure from the IUEROOT"
	@echo "* (or IUELOCALROOT) with the appropriate arguments, e.g. "
	@echo "* for GLUT, use --with-glut=/path/to/GLUT".
endif

# clear out all the build if non-windows stuff is built on a windows enviroment
ifeq ($(OS),win32)
  ifeq ($(err_CANNOT_BUILD_ON_WINDOWS),1)
    SOURCES =
    PROGRAM =
    LIBRARY =
xall::
	@echo "This code cannot be built on windows"
  endif
endif

# clear out all the build if windows stuff is built on a non windows enviroment
ifneq ($(OS),win32)
  ifeq ($(err_MUST_BUILD_ON_WINDOWS),1)
    SOURCES =
    PROGRAM =
    LIBRARY =
xall::
	@echo "Microsoft dependent code may only be build in windows"
  endif
endif

# if the user tries to link in -lImageClasses, then add -ljpeg
ifndef VXL
ifndef HAS_SH_LINK # because otherwise the transitive linking does the job

# make sure they do not have it already
ifeq "" "$(findstring -ljpeg,$(IULIBS))"
# only do this if they have -lImageClasses
  ifneq "" "$(findstring -lImageClasses,$(IULIBS))"
# make sure the user is not getting -ljpeg from USE_FRESCO
    ifeq "" "$(findstring -ljpeg,$(STDLIBS))"
      IULIBS += -ljpeg
    endif
  endif
endif

endif # HAS_SH_LINK
endif # VXL


ifneq (,$(strip $(filter gcc,$(COMPILER))$(filter gcc-3.0,$(COMPILER))))
# gcc should never be given an include path to /usr/include as that
# might cause the inclusion of certain system headers which gcc
# provides too. This is done if the system headers are broken, so
# we must avoid /usr/include when compiling with gcc.
ifneq (,$(strip $(filter /usr/include,$(INCDIRS))))
INCDIRS := $(filter-out /usr/include,$(INCDIRS))
endif
endif

#############################################################################
#############################################################################

### It becomes more generic from here....

## Assemble the final flags.

# The tj_* flags are those specific to the IUE as a library,
# Imagine tj_CPPFLAGS to be equivalent to -I/usr/X11/include, although
# it's much longer.
#
# Then the non-prefixed versions are made from the prefixed ones and any others
# that are needed, just as a standard compilation which uses many libraries does.

## CPPFLAGS included after defines and before includes so both can be overridden:
## defines by -U, includes by prefixing the correct one.

# Config: CVFLAGS
# Compiler Verbosity flags, may be overridden by specifying in site.mk
ifndef CVFLAGS
 CVFLAGS := $(wall)
endif

# Config: CGFLAGS
# Code Generation Flags; passed to cc1,cc1plus,ld.
# Set in build-$(BUILD).mk

# make sure that vcl or Config-IUE is the last include
tj_INCDIRS = $(INCDIRS) $(CONFIG_IUE_INCDIR)
include_flags = $(tj_INCDIRS:%=-I%)

ifeq ($(USING_GCC_COMPILER),1)
  x_include_flags = $(tj_INCDIRS:%=-I%)
  include_flags = $(x_include_flags:-I$(X11_INC_DIR)=-isystem $(X11_INC_DIR))
endif

ifeq ($(OS),win32)
include_flags := $(shell echo $(include_flags) |sed -e "s@//\([A-Za-z]\)@\1:@g")
endif
tj_CPPFLAGS = $(include_flags)
tj_CGFLAGS = $(CMDCFLAGS1) $(CGFLAGS) $(CVFLAGS) $(ccflags) $(CMDCFLAGS)
tj_CCFLAGS = $(CMDCCFLAGS1) $(tj_CGFLAGS) $(APP_CCFLAGS) $(CPPFLAGS) $(CMDCCFLAGS)

# jb@aai -- removed CGFLAGS from LDFLAGS because the $(pic) was
# getting into the link, which was causing problems in certain cases
# (alphatech).  Also, code isn't really being generated at link time
# anyway.  Note that the CGFLAGS still appears when creating shared
# libraries (see rule further below for creating shared libs, which
# puts tj_CGFLAGS on the command line)

# tj_LDFLAGS = $(CGFLAGS) $(LDFLAGS)
LD_UNDEF_LDFLAGS = $(foreach symbol,$(LD_UNDEFS:%=%), $(IUE_EXPAND_LD_UNDEF) )
LDFLAGS += $(LD_UNDEF_LDFLAGS)
tj_LDFLAGS = $(CMDLDFLAGS1) $(LDFLAGS) $(CMDLDFLAGS)

####
# Assemble the final versions
CPPFLAGS += $(OS_DEFINES) $(DEFINES) $(tj_CPPFLAGS)
CFLAGS   = $(C_INCDIRS:%=-I%) $(CGFLAGS) $(CVFLAGS) $(APP_CFLAGS) $(CPPFLAGS)
CCFLAGS  = $(CGFLAGS) $(APP_CCFLAGS) $(CPPFLAGS)

# Internal: LDPATH
# Made from LIBDIRS, holds appropriate -L and -R or -rpath flags.
LDPATH := $(LIBDIRS:%=$(LIB_PATH)%)
ifneq ($(RPATHLIBDIRS),)
LDPATH += $(rpath)
endif

# convert unix slash to win32 slash
ifeq ($(OS),win32)
win32_LDPATH = $(shell echo $(LDPATH) |sed -e "s@//\([A-Za-z]\)@\1:@g")
LDPATH := $(subst /,\\,$(win32_LDPATH))
endif

# Internal: LOADLIBES
# LOADLIBES is used by the default GNU make rules for linking executables.
LOADLIBES := $(LDPATH)


# "Harden" LDLIBS.  This means that any $(...) references in LDLIBS are
# expanded at this point, and future changes to the referred-to variables
# have no effect.  Specifically, any changes to IULIBS, STDLIBS, etc after 
# the inclusion of rules.mk will be ignored.
LDLIBS := $(LDLIBS)
STDLIBS := $(STDLIBS)

# Backward-compatibilty hacks on LDLIBS
ifneq (,$(findstring -lCOOL,$(LDLIBS)))
xall::
	@echo "rules.mk WARNING: -lCOOL should be changed to -lTJCOOL"
endif
LDLIBS := $(subst -lCOOL,-lTJCOOL,$(LDLIBS))

# Variable: IMMEDIATE_LIBS
# Users can set this in a makefile which creates a (shared) library.
# It should be the list of libraries (with -l but without .lib) containing
# definitions of unresolved symbols in the shared library being built,
# i.e., the "immediate" libraries on which the current one depends.
# If additionally HAS_SH_LINK was set (by default on win32),
# the names and rpaths to these libraries are written into the shared library
# being built.  This allows for transitive linking: when linking an executable,
# it is no longer necessary to give all its link libraries, only the immediate
# ones, which in turn point to their dependencies by the transitive linking
# mechanism.
# If not set, IMMEDIATE_LIBS defaults to IULIBS (or actually to LDLIBS).
# Note that STDLIBS are not added to IMMEDIATE_LIBS (except on win32),
# to avoid specifying a static library, like libstdc++.a .
# Note also that transitive linking will not work in the case of circular
# library dependencies; a bootstrapped build should be done in that case:
# create one library without HAS_SH_LINK, create the other one(s), then
# re-create the first one with HAS_SH_LINK.  This has to be done manually.
ifndef IMMEDIATE_LIBS
   IMMEDIATE_LIBS := $(LDLIBS)
endif
ifneq "" "$(findstring COOL,$(USES))"
  IMMEDIATE_LIBS += -lTJCOOL
endif
ifneq "" "$(findstring Numerics,$(USES))"
  IMMEDIATE_LIBS += -liue-math
endif
IMMEDIATE_LIBS += $(STDLIBS)

# Convert LDLIBS to -l or .lib form.
LDLIBS := $(patsubst -l%,$(LINK_PREFIX)%$(LINK_POSTFIX),$(LDLIBS))
STDLIBS := $(patsubst -l%,$(LINK_PREFIX)%$(LINK_POSTFIX),$(STDLIBS))
IMMEDIATE_LIBS := $(patsubst -l%,$(LINK_PREFIX)%$(LINK_POSTFIX),$(IMMEDIATE_LIBS))
ifdef HAS_SH_LINK
LDLIBS := $(IMMEDIATE_LIBS)
endif

#############################################################################

# basenames etc.
srcbase := $(basename $(SOURCES))

## Some hairy macros to get the current source file from the
# dependency list.  The problem is that when we have an object file,
# we can't infer its suffix, and we don't want to do it by wildcarding
# as it's not uncommon to have a file hanging around with the same
# basename but a different suffix.  (Two ways that happens are (a)
# changing the suffix of a file and not removing the old one
# e.g. "promoting" a file from C to C++ and (b) autogenerated .c
# files).  Therefore we want to get back to the original source file
# that the makefile mentions.

# Really we want the %.C on the right hand side of the rule, but there appears
# no way to get at it cleanly.  Instead, we assume that the %.C appears first in
# the dependency list *before* any headers or suchlike, and use GNU make's
# special variable $^, which contains all the dependencies, in the order they
# were specified.

source_for_object = $(firstword $^)

# Note that "source_for_object = $(filter $(baseat).%,$(allsources))" fails for
# files with dots because base.instances.C and base.C will both be extracted for
# file base.C Oh for regexp matching...

#############################################################################
# Targets
#
# Defines the standard targets based on whether variables are set:
#  SUBDIRS: List of subdirs to recurse into after other making.
#  LIBRARY, VERSION: Make library with name LIBRARY.
#                    Shared library is version VERSION.

# First combine sources by type.  For example, MINI_SO_TESTS are MINI_SO_SOURCES

ifdef MINI_SO_TESTS
MINI_SO_SOURCES += $(MINI_SO_TESTS)
MINI_PROG_SOURCES += TestDriver.C
pre_deps += TestDriver.C 
# Note: TestDriver.C is an autogenerated file, its rule is below.

# Insert $(FORCE_LOAD) before every library of LDLIBS, when building TestDriver
# (because it loads libraries using dlopen, so delay load won't work).
LDLIBS := $(patsubst %,$(FORCE_LOAD) %,$(LDLIBS))
endif


#############################################################################
################ Build objects from $(SOURCES) ##############################

# Combine all sources into one variable so that all objects are made from it,
# even though the objects might be used in various nefarious ways
allsources := $(SOURCES) $(MINI_PROG_SOURCES) $(MINI_SO_SOURCES)

## Separate allsources based on suffix
allsources_C := $(filter %.C,$(allsources))
allsources_cxx := $(filter %.cxx,$(allsources))
allsources_cpp := $(filter %.cpp,$(allsources))
allsources_c++ := $(filter %.c++,$(allsources))
allsources_cc := $(filter %.cc,$(allsources))
allsources_yy := $(filter %.yy,$(allsources))
allsources_ll := $(filter %.ll,$(allsources))
allsources_c := $(filter %.c,$(allsources))
allsources_y := $(filter %.y,$(allsources))
allsources_l := $(filter %.l,$(allsources))
allsources_S := $(filter %.S,$(allsources))
allsources_s := $(filter %.s,$(allsources))

allsources_rc := $(filter %.rc,$(RESOURCE_SOURCES))

# Add to all .cc files those generated from yacc, lex
#allsources_cc += $(allsources_yy:%.yy=%.cc)
#allsources_cc += $(allsources_ll:%.ll=%.cc)
#allsources_c += $(allsources_y:%.y=%.c)
#allsources_c += $(allsources_l:%.l=%.c)

## Build object file names from suffixed sources
allsources_obj_C := $(allsources_C:%.C=$(RELOBJDIR)$/%.$(OBJ_EXT))
allsources_obj_cxx := $(allsources_cxx:%.cxx=$(RELOBJDIR)$/%.$(OBJ_EXT))
allsources_obj_cpp := $(allsources_cpp:%.cpp=$(RELOBJDIR)$/%.$(OBJ_EXT))
allsources_obj_c++ := $(allsources_c++:%.c++=$(RELOBJDIR)$/%.$(OBJ_EXT))
allsources_obj_cc := $(allsources_cc:%.cc=$(RELOBJDIR)$/%.$(OBJ_EXT))
allsources_obj_c := $(allsources_c:%.c=$(RELOBJDIR)$/%.$(OBJ_EXT))
allsources_obj_S := $(allsources_S:%.S=$(RELOBJDIR)$/%.$(OBJ_EXT))
allsources_obj_s := $(allsources_s:%.s=$(RELOBJDIR)$/%.$(OBJ_EXT))
allsources_cc_y := $(allsources_y:%.y=%.c++)
allsources_obj_y := $(allsources_y:%.y=$(RELOBJDIR)$/%.$(OBJ_EXT))
allsources_cc_l := $(allsources_l:%.l=%.c++)
allsources_obj_l := $(allsources_l:%.l=$(RELOBJDIR)$/%.$(OBJ_EXT))

## Collect all the object files into one list.
allsources_obj := $(allsources_obj_C) $(allsources_obj_cxx) $(allsources_obj_cpp) $(allsources_obj_c++)\
 $(allsources_obj_cc) $(allsources_obj_c) $(allsources_obj_S) $(allsources_obj_s) \
 $(allsources_obj_y) $(allsources_obj_l)

allsources_res_rc := $(allsources_rc:%.rc=$(RELOBJDIR)$/%.$(RES_EXT))

## template sources
allsources_tmpl_C := $(filter %.C,$(TEMPLATE_SOURCES))
allsources_obj_tmpl_C := $(allsources_tmpl_C:%.C=$(RELOBJDIR)$/%.$(OBJ_EXT))
allsources_tmpl_cc := $(filter %.cc,$(TEMPLATE_SOURCES))
allsources_obj_tmpl_cc := $(allsources_tmpl_cc:%.cc=$(RELOBJDIR)$/%.$(OBJ_EXT))
allsources_tmpl_cxx := $(filter %.cxx,$(TEMPLATE_SOURCES))
allsources_obj_tmpl_cxx := $(allsources_tmpl_cxx:%.cxx=$(RELOBJDIR)$/%.$(OBJ_EXT))
SOURCES_obj_tmpl := $(allsources_obj_tmpl_C) $(allsources_obj_tmpl_cc) $(allsources_obj_tmpl_cxx)
allsources_obj_tmpl := $(SOURCES_obj_tmpl) # $(RELOBJDIR)$/Templates$/*.$(OBJ_EXT)

## Recombine just the objects from SOURCES -- these will be used to go into
## the "main" target in a directory.
SOURCES_res += $(patsubst %,$(RELOBJDIR)$/%.$(RES_EXT),$(basename $(RESOURCE_SOURCES)))
SOURCES_obj += $(patsubst %,$(RELOBJDIR)$/%.$(OBJ_EXT),$(basename $(SOURCES)))
SOURCES_obj += $(SOURCES_obj_tmpl)

# $(baseat)  Expands to the basename of the current target, with .$(OBJ_EXT) stripped
baseat = $(@:$(RELOBJDIR)$/%.$(OBJ_EXT)=%)

extraflags_for_object = $(FLAGS_$(source_for_object))

# Variable: USE_IMPLICIT_TEMPLATES
#
# Set to 1 if any source files requires its templates to be instantiated here.
# [E.g. The IUE makefile use this to turn on implicit instantiation.]
# This may also be defined by config.mk.in now. -- fsm
ifeq (1,$(strip $(USE_IMPLICIT_TEMPLATES)))
no_implicit_templates := ${implicit_templates}
endif

# Config: PRAGMA_INSTANTIATE_TEMPLATES
# Set by configure, or in $(OS)-$(COMPILER).mk to indicate that the compiler
# uses #pragma instantiate Class<T> rather than template class Class<T> to
# instantiate templates.


ifdef PRAGMA_INSTANTIATE_TEMPLATES
TMASG := $(IUE_PERL) -p $(sys_or_iu_Scripts)/Perl/sgi-massage-templates

# *** who still needs this? ***

#### Compile to .i so that second compile is faster (does not need to spawn cpp)
define COMPILE.c++
	$(C++) $(source_for_object) $(extraflags_for_object) \
	$(no_implicit_templates) $(tj_CCFLAGS) -E | $(TMASG) > $@.i
	$(C++) $(extraflags_for_object) $(no_implicit_templates) $(tj_CGFLAGS) $@.i -c $(OUTPUT_OPTION)
	$(RM) $@.i
endef
define COMPILE.tmpl
	$(C++) $(force_c++) $(source_for_object) $(extraflags_for_object) \
	$(implicit_templates)    $(tj_CCFLAGS) -E | $(TMASG) > $@.i
	$(C++) $(force_c++) $(extraflags_for_object) $(no_implicit_templates) $(tj_CGFLAGS) $@.i -c $(OUTPUT_OPTION)
	$(RM) $@.i
endef

else
define COMPILE.c++
	$(strip $(C++)              $(DOS_FULL_SRC_PATH)$(source_for_object) $(no_implicit_templates) $(tj_CCFLAGS) $(extraflags_for_object) -c $(OUTPUT_OPTION))
endef
define COMPILE.tmpl
	$(strip $(C++) $(force_c++) $(DOS_FULL_SRC_PATH)$(source_for_object) $(implicit_templates)    $(tj_CCFLAGS) $(extraflags_for_object) -c $(OUTPUT_OPTION))
endef
endif


# Sunpro CC does not understand the .S suffix
# COMPILE.S
ifndef OS_COMPILE.S
COMPILE.S = $(CC) $(source_for_object) $(extraflags_for_object) $(CFLAGS) -c $(OUTPUT_OPTION)
else
COMPILE.S = $(OS_COMPILE.S)
endif

# COMPILE.s
ifndef OS_COMPILE.s
COMPILE.s = $(COMPILE.S)
else
COMPILE.s = $(OS_COMPILE.s)
endif

## Note: extraflags_for_object should be after defaults

define COMPILE.c
	$(CC) $(DOS_FULL_SRC_PATH)$(source_for_object) $(CFLAGS) $(extraflags_for_object) -c $(OUTPUT_OPTION)
endef

COMPILE.y = bison -d -t -o $@ $(YACCFLAGS) $(source_for_object) && mv $@.h $(subst .c++,.h,$@)
COMPILE.l = $(LEX) -t $(LEXFLAGS) $(source_for_object) > $@

COMPILE.rc = $(RESOURCE_COMPILER) $(subst /,\\,$(include_flags)) $(APP_RCFLAGS) $(OUTPUT_OPTION) $(source_for_object)

nullobj := $(RELOBJDIR)$/null.$(OBJ_EXT)

ifdef noisy_make
DASH_LINE = echo "------------------------------------------------------------"
VERBOSE_COMPILE = @echo "" ; echo "-- Compiling $<"
CDLINE = echo ""; $(DASH_LINE) ; echo "-- Compiling directory $(@D)" ; echo ""
else
VERBOSE_COMPILE = @true
CDLINE = @true
endif

## Now specify the rules to make objects from source files :
$(allsources_obj_C) : $(RELOBJDIR)$/%.$(OBJ_EXT) : %.C
	$(VERBOSE_COMPILE)
	@$(MKDIR_P) $(@D)
	$(COMPILE.c++)
$(allsources_obj_cc) : $(RELOBJDIR)$/%.$(OBJ_EXT) : %.cc
	$(VERBOSE_COMPILE)
	@$(MKDIR_P) $(@D)
	$(COMPILE.c++)
$(allsources_obj_cxx) : $(RELOBJDIR)$/%.$(OBJ_EXT) : %.cxx
	$(VERBOSE_COMPILE)
	@$(MKDIR_P) $(@D)
	$(COMPILE.c++)
$(allsources_obj_cpp) : $(RELOBJDIR)$/%.$(OBJ_EXT) : %.cpp
	$(VERBOSE_COMPILE)
	@$(MKDIR_P) $(@D)
	$(COMPILE.c++)
$(allsources_obj_c++) : $(RELOBJDIR)$/%.$(OBJ_EXT) : %.c++
	$(VERBOSE_COMPILE)
	@$(MKDIR_P) $(@D)
	$(COMPILE.c++)
$(allsources_obj_c) : $(RELOBJDIR)$/%.$(OBJ_EXT) : %.c
	$(VERBOSE_COMPILE)
	@$(MKDIR_P) $(@D)
	$(COMPILE.c)
$(allsources_obj_tmpl_C) : $(RELOBJDIR)$/%.$(OBJ_EXT) : %.C
	$(VERBOSE_COMPILE)
	@$(MKDIR_P) $(@D)
	$(COMPILE.tmpl)
$(allsources_obj_tmpl_cc) : $(RELOBJDIR)$/%.$(OBJ_EXT) : %.cc
	$(VERBOSE_COMPILE)
	@$(MKDIR_P) $(@D)
	$(COMPILE.tmpl)
$(allsources_obj_tmpl_cxx) : $(RELOBJDIR)$/%.$(OBJ_EXT) : %.cxx
	$(VERBOSE_COMPILE)
	@$(MKDIR_P) $(@D)
	$(COMPILE.tmpl)
$(allsources_obj_y) : $(RELOBJDIR)$/%.$(OBJ_EXT) : %.c++
	$(VERBOSE_COMPILE)
	@$(MKDIR_P) $(@D)
	$(COMPILE.c++)
$(allsources_cc_y) : %.c++ : %.y
	$(VERBOSE_COMPILE)
	@$(MKDIR_P) $(@D)
	$(COMPILE.y)
$(allsources_res_rc) : $(RELOBJDIR)$/%.$(RES_EXT) : %.rc
	$(VERBOSE_COMPILE)
	@$(MKDIR_P) $(@D)
	$(COMPILE.rc)

# yacc (bison) and lex (flex) added by Peter Vanroose - 24 Oct 1997
$(allsources_cc_l) : %.c++ : %.l
	$(VERBOSE_COMPILE)
	@$(MKDIR_P) $(@D)
	$(COMPILE.l)
$(allsources_obj_l) : $(RELOBJDIR)$/%.$(OBJ_EXT) : %.c++
	@$(MKDIR_P) $(@D)
	$(COMPILE.c++)

$(allsources_obj_S) : $(RELOBJDIR)$/%.$(OBJ_EXT) : %.S
	$(VERBOSE_COMPILE)
	@$(MKDIR_P) $(@D)
	$(COMPILE.S)
$(allsources_obj_s) : $(RELOBJDIR)$/%.$(OBJ_EXT) : %.s
	$(VERBOSE_COMPILE)
	@$(MKDIR_P) $(@D)
	$(COMPILE.s)


#$(allsources_yy:%.yy=%.cc) : %.cc : %.yy

#  $(allsources_yy:%.yy=%.cc) $(allsources_yy:%.yy=%.h) : $(allsources_yy)
# 
# $(allsources_yy:%.yy=%.cc) $(allsources_yy:%.yy=%.h) : %.cc %.h : %.yy
# 	  bison $(firstword $^) --defines --debug -o $@
# 	  mv $@.h $(@:.cc=.h)
# 
$(allsources_l:%.l=%.cc) : %.cc : %.l
	  flex -p -o$@ $(firstword $^)

$(allsources_ll:%.ll=%.cc) : %.cc : %.ll
	  flex -p -o$@ $(firstword $^)

$(allsources_ll:%.ll=$(RELOBJDIR)$/%.$(OBJ_EXT)) : $(RELOBJDIR)$/%.$(OBJ_EXT) : %.cc
	@$(MKDIR_P) $(@D)
	$(COMPILE.c++)

# An error on this line of the form
#     target `fex_parser.yy' doesn't match the target pattern
# means it's really on the line above



################ Hartley _dcl.h files ##############################
# These are the rules for generating _dcl.h files from .c and .cc files
# currently those are the two extensions used in Richards Carmen code.
# I don't expect, or want this to become used in the rest of IUE,
# so it should not be that limiting.

ifeq ($(strip $(GEN_DCL)),1)
# keep _dcl.h files upto date
dot_dcl_from_c_files := $(patsubst %.c,.%_dcl.h,$(allsources_c))
dot_dcl_from_cc_files := $(patsubst %.cc,.%_dcl.h,$(allsources_cc))
xall:: $(dot_dcl_from_c_files) $(dot_dcl_from_cc_files)

# make .file_dcl.h depend on .c files  
# each time a .c file is changed the .file_dcl.h is created
# if it is different than file_dcl.h, .file_dcl.h is copied to 
# file_dcl.h, this is done by the gen_decl.pl script
ifneq ($(strip $(dot_dcl_from_c_files)),)
$(dot_dcl_from_c_files) : .%_dcl.h: %.c
	$(IUE_PERL) $(sys_or_iu_Scripts)/Perl/gen_decl.pl -c -N $(source_for_object) -o $@ $(source_for_object) 
endif

ifneq ($(strip $(dot_dcl_from_cc_files)),)
$(dot_dcl_from_cc_files) : .%_dcl.h: %.cc
	$(IUE_PERL) $(sys_or_iu_Scripts)/Perl/gen_decl.pl -N $(source_for_object) -o $@ $(source_for_object) 
endif

# generate _decl.h files if they are not there
# if gen_decl is given -create_only option it will first check
# to see if the file is there and exit if it exits, otherwise it will create
# the file.
pre_deps_c  = $(patsubst %.c,%_dcl.h,$(allsources_c))
pre_deps_cc = $(patsubst %.cc,%_dcl.h,$(allsources_cc))

ifneq ($(strip $(pre_deps_c)),)
$(pre_deps_c) : %_dcl.h : %.c
	@$(IUE_PERL) $(sys_or_iu_Scripts)/Perl/gen_decl.pl -create_only -c -N $(source_for_object) -o $@ $(source_for_object)
endif

ifneq ($(strip $(pre_deps_cc)),)
$(pre_deps_cc) : %_dcl.h : %.cc
	@$(IUE_PERL) $(sys_or_iu_Scripts)/Perl/gen_decl.pl -create_only -N $(source_for_object) -o $@ $(source_for_object)
endif

pre_deps = $(pre_deps_c) $(pre_deps_cc)

endif # end of Hartley _dcl.h files


#############################################################################
################ DEPENDENCIES ###############################################

# Dependencies are automatically made as follows:
# GNU make 

# Internal: allsources_deps
# The list of .d files in which dependencies live.
# The .d files for x.cc are $(RELOBJDIR)$/x.cc.d
# This allows us to easily determine the src file for a .d file
# They should be *included*, just not rebuilt.

allsources_deps := $(allsources:%=$(RELOBJDIR)$/%.d) $(TEMPLATE_SOURCES:%=$(RELOBJDIR)$/%.d)
ifneq "" "$(strip $(allsources_deps))"
# Note: including $(dirstamp) ensures the dir is made.
-include $(dirstamp) $(allsources_deps)
endif

# Macro to infer source file name from .d name. It's easy because we just strip off the .d
source_for_@d = $(@:$(RELOBJDIR)$/%.d=%)

# Option: NODEPENDS
# Set to 1 to inhibit dependency checking.  If you have never been bitten by
# a non up-to-date dependency before, do not use this flag.
ifndef NODEPENDS

# Variable: pre_deps
# Additional dependencies for the deps files.  This varible would
# normally be empty causing no trouble, however if you generate _decl.h files
# from .c files this needs to be done before the depends are run, so the var is set

# Must be careful with .d files, as we never want to leave an empty one behind.  If we do, it
# won't get remade.  So make deps to a temp file and move it only if the command has succeeded.
# Notes:
#- Piping through perl would hide the exit code of MAKEDEPEND, so do that afterwards.
#- Depends can legitimately fail when files are removed.
#- Added dependency on .cc so depends are made when .d is not there, and .cc
#  is autogenerated.

MAKEDEPEND_VERBOSE := $(subst depend.pl,depend.pl -v,$(MAKEDEPEND))

# Config: CPP_MAKEDEPEND
# Set to 1 if using the win32bin/depends.pl script

# due_to = $?
due_to = \\t ........ due to $(firstword $(filter-out $(RELOBJDIR)/stamp, $?))

ifdef SHOW_DEPENDCMD
fsm_at:=
else
fsm_at:=@
endif # ifdef SHOW_DEPENDCMD

ifdef CPP_MAKEDEPEND

# fsm@robots.ox.ac.uk
# CC -n32 fails to exec /usr/lib32/cmplrs/cpp when used to 
# make dependences for an assembly (.s) file. So we don't
# do that. FIXME - is this really the place to make that
# distinction?
$(filter-out %.s.d,$(allsources_deps)) : $(RELOBJDIR)/%.d : $(dirstamp) $(pre_deps) %
	@test -f $(source_for_@d) || echo "Source file $(source_for_@d) not found"
	@$(MKDIR_P) $(@D)
	@echo "-- Making dependencies for $(source_for_@d)  $(due_to)"
	$(fsm_at)$(MAKEDEPEND) $(force_language_macro) $(source_for_@d) $(CPPFLAGS)  -out $@

else # ifdef CPP_MAKEDEPEND

obj_for_@d = $(@:$(suffix $(@:.d=)).d=.$(OBJ_EXT))

$(allsources_deps) : $(RELOBJDIR)/%.d : $(dirstamp) $(pre_deps) %
	@test -f $(source_for_@d) || echo "Source file $(source_for_@d) not found"
	@$(MKDIR_P) $(@D)
	@echo "-- Making dependencies for $(source_for_@d) $(due_to)"
	@$(RM) $@
ifdef IUE_PLUSPLUS
	$(fsm_at)$(C++) -V $(source_for_@d) $(CPPFLAGS) -M $@ -o $(obj_for_@d) # -c
else
	$(fsm_at)$(MAKEDEPEND) $(source_for_@d) $(CPPFLAGS) > $@~
	$(QUIET) $(IUE_PERL) $(sys_or_iu_Scripts)/Perl/tj-massage-depends.pl -OBJFILE=$(obj_for_@d) -DEPFILE=$@  < $@~ > $@
	$(QUIET) $(RM) $@~
endif

endif # ifdef CPP_MAKEDEPEND

endif # NODEPENDS


ifeq ($(OS),win32)
 defined_undefined = @$(EMPTY_COMMAND)
else
  # allow user to set in environment (e.g. to turn it off).
 ifndef defined_undefined
  defined_undefined = $(sys_or_iu_Scripts)/Perl/defined_undefined.pl
 endif
endif

# Option: CLEAN
# Set in site.mk, or on make line, to indicate that object files should be
# removed after successfully building the library or executables.
CLEAN_FILES :=	$(RELOBJDIR)$/*.$(OBJ_EXT).i $(RELOBJDIR)$/Templates$/*.$(OBJ_EXT).i\
		$(RELOBJDIR)$/ii_files$/* $(RELOBJDIR)$/Templates$/ii_files$/*
ifdef CLEAN
CLEAN_FILES +=	$(SOURCES_obj) $(allsources_deps) $(RELOBJDIR)$/vc50.pdb \
		$(RELOBJDIR)$/stamp $(RELOBJDIR)$/mandeps \
		$(RELOBJDIR)$/*.def $(LIBDEST)$/$(LIBRARY).pdb \
		$(LIBDEST)$/$(LIBRARY).ilk $(LIBDEST)$/$(LIBRARY).exp
endif

#############################################################################
################ LIBRARY ####################################################

# Variable: LIBRARY
# The name of a library to be made from SOURCES.
ifneq ($(strip $(LIBRARY)),)

# Config: MAKE_SHARED
# Should be defined in build-$(BUILD).mk if BUILD implies
# shared libraries and dynamically linked executables.

ifndef MAKE_SHARED
###############################
# Archive/Static library target
libfile := $(LIBDEST)$/$(LIB_PREFIX)$(LIBRARY).$(AR_EXT)

xall:: $(libfile)

# Config: MAKE_ARLIB MAKE_ARLIB_CMD
# Set MAKE_ARLIB if what? FIXME
# Otherwise use MAKE_ARLIB_CMD
ifeq (,$(MAKE_ARLIB))
$(libfile): $(LIBDEST)$/stamp $(SOURCES_obj)
	@echo ""
	@echo "-- Creating Static Library $@"
	@$(RM) $@
	$(MAKE_ARLIB_CMD) $@ $(SOURCES_obj)
	-$(RM) $(CLEAN_FILES)
	$(defined_undefined) $(libfile) $(libfile).defined $(libfile).used

else
$(libfile): $(LIBDEST)$/stamp $(SOURCES_obj)
	@echo ""
	@echo "-- Creating Static Library $@"
	$(MAKE_ARLIB) $(SOURCES_obj)
	-$(RM) $(CLEAN_FILES)
	$(defined_undefined) $(libfile) $(libfile).defined $(libfile).used

endif # MAKE_ARLIB
else # MAKE_SHARED
###########################
# Shared library/DLL target
# some os's (freebsd ) require a version, so default to 1.0
ifeq (,$(VERSION))
VERSION = 1.0
endif
shlibnameversion := lib$(LIBRARY).$(LIB_EXT).$(VERSION)
shlibname := $(LIB_PREFIX)$(LIBRARY).$(LIB_EXT)
shlibfile := $(LIBDEST)$/$(shlibname)
shlibversionfile := $(LIBDEST)$/$(shlibnameversion)
set_soname := $(soname_opt)

ifeq ($(strip $(USE_LIB_VERSION)),1)
xall::  $(shlibversionfile)
endif # USE_LIB_VERSION

xall:: $(shlibfile)

## Win32 needs to make the library.{def,exp} files.
ifeq ($(OS),win32)
DEFINES += -DBUILDING_$(LIBRARY)_DLL
deffile := $(RELOBJDIR)$/$(LIBRARY).def
xall:: $(deffile)
$(deffile): $(LIBDEST)$/stamp $(SOURCES_obj)
	@echo ""
	@echo "-- [Win32] Creating Module Definition File for Shared Library $(LIBRARY)"
	@$(IUE_PERL) $(IUELOCALROOT)/win32bin/makedef.pl $@ $(shlibname) $(RELOBJDIR) $(SOURCES_obj)

opt_deffile = -def:$(deffile)
# This is to handle the dlls can't mutually depend on each other
# you have to use lib for a first pass, then use link to build the lib
expfile := $(LIBDEST)$/$(LIBRARY).exp
dos_expfile := $(shell echo $(expfile) |sed -e "s@//\([A-Za-z]\)@\1:@g")
dos_libfile := $(dos_expfile:.exp=.lib)
xall:: $(expfile)
$(expfile): $(deffile)
	@echo ""
	@echo "-- [Win32] Creating Export File and Import Library for $(LIBRARY)"
	lib -nologo -machine:IX86 -def:$(deffile) -out:$(dos_libfile) $(SOURCES_obj)
endif # WIN32

safelink:
	$(MAKE) LINK_INTO_TMP=1


# See params.mk, link shared lib into /tmp and then mv to final
# destination.
ifeq "" "$(strip $(LINK_INTO_TMP))"
  shlib_op = $(LINK_OUT_OPTION)
  shlib_mv = @$(EMPTY_COMMAND)
else
  ifeq ($(OS),win32)
    shlib_tmpname = tmplink.$(subst /,_,$(OBJDIR)).$(@F)
  else
    ifeq "" "$(strip $(TMPDIR))"
      TMPDIR := /tmp
    endif
    shlib_tmpname = $(TMPDIR)/tmplink.$(subst /,_,$(OBJDIR)).$(@F)
  endif
  shlib_op = $(foreach @,$(shlib_tmpname),$(OUTPUT_OPTION))
  # sometimes this business gets funny dates onto the libfile
  # (SGI->/tmp, mv /tmp to Solaris NFS2 partition, date is 1hr ahead)
  # The touch is required to fix it.
  shlib_mv = mv $(shlib_tmpname) $@ && touch $@
endif # LINK_INTO_TMP

# Config: HAS_SH_LINK
# Set to 1 if the operating system needs to (or can) add -l flags when building a shared
# library, to resolve immediate external dependencies.
# Also set LDLIBS to IMMEDIATE_LIBS which is normally much shorter.
# This is only important when creating an executable.
ifeq ($(strip $(HAS_SH_LINK)),1)
# this removes the library that is being made from the list of libraries 
# that are linked to the library for obvious reasons, if not obvious methods...
sh_extra_libs := $(LDPATH) $(subst $(LINK_PREFIX)$(LIBRARY)$(LINK_POSTFIX),,$(IMMEDIATE_LIBS))
sh_extra_libs := $(LDPATH) $(subst $(LINK_PREFIX)$(LIBRARY)$(LINK_POSTFIX),,$(sh_extra_libs))
LDLIBS := $(IMMEDIATE_LIBS)
endif # HAS_SH_LINK

# This is to handle the dlls can't mutually depend on each other
# you have to use lib for a first pass, then use link to build the lib
ifeq ($(OS),win32)
#shlib_op = -out:$@
endif # WIN32

ifdef PURIFYHOME
PURIFIED_SHLIB := $(PURIFYHOME)/cache$(shlibfile)_pure_p3_c0_410_56
endif

$(shlibfile): $(LIBDEST)$/stamp $(SOURCES_obj) $(expfile) $(deffile) 
	@echo ""
	@echo "-- Linking Shared Library $(LIBRARY)"
	$(RM) so_locations
	$(MAKE_SHLIB) \
	$(SOURCES_obj) \
	$(shlib_op) $(set_soname) $(tj_CGFLAGS) $(LDFLAGS) $(sh_extra_libs) $(opt_deffile)
	-$(RM) $(CLEAN_FILES)
	$(shlib_mv)
	$(defined_undefined) $(shlibfile) $(shlibfile).defined $(shlibfile).used

# Config: USE_LIB_VERSION
# This is for OS's like sunos that need a version number on the 
# shared library for things to work.  The USE_LIB_VERSION flag
# is set in the os-compiler file in the config directory.
# This is wrapped with an ifdef MAKE_SHARED so that it is only done
# with the shared version
ifeq ($(strip $(USE_LIB_VERSION)),1)
$(shlibversionfile):  $(shlibfile)
	$(RM) $(shlibversionfile)
	$(LN) $(shlibfile) $(shlibversionfile)
endif

endif # MAKE_SHARED

endif # LIBRARY
################ END LIBRARY ################################################
#############################################################################

#############################################################################
################ STATIC_LIBRARY #############################################

# Variable: STATIC_LIBRARY
# The name of a static library to be made from SOURCES.
ifneq ($(strip $(STATIC_LIBRARY)),)

libfile := $(LIBDEST)$/$(LIB_PREFIX)$(STATIC_LIBRARY).$(AR_EXT)

xall:: $(libfile)

# Config: MAKE_ARLIB MAKE_ARLIB_CMD
# Set MAKE_ARLIB if what? FIXME
# Otherwise use MAKE_ARLIB_CMD
ifeq (,$(MAKE_ARLIB))
$(libfile): $(LIBDEST)$/stamp $(SOURCES_obj)
	@echo ""
	@echo "-- Creating Static Library $@"
	@$(RM) $@
	$(MAKE_ARLIB_CMD) $@ $(SOURCES_obj)
	-$(RM) $(CLEAN_FILES)
	$(defined_undefined) $(libfile) $(libfile).defined $(libfile).used
#	$(STRIP) $@

else
$(libfile): $(LIBDEST)$/stamp $(SOURCES_obj)
	@echo ""
	@echo "-- Creating Static Library $@"
	$(MAKE_ARLIB) $(SOURCES_obj)
	-$(RM) $(CLEAN_FILES)
	$(defined_undefined) $(libfile) $(libfile).defined $(libfile).used
#	$(STRIP) $@

endif # MAKE_ARLIB
endif # STATIC_LIBRARY
################ END STATIC_LIBRARY #########################################
#############################################################################


#############################################################################
################################# PROGRAM  ##################################

# Internal: LIB_DEPENDENCIES
# The full pathnames of all static libraries being linked, generated from
# LIBDIRS and LDLIBS.  For example, an LDLIBS of
#  -lmylib -lTJCOOL -lm
# might produce:
#  /homes/me/lib/libmylib.a /pkg/iue/COOL/lib/$(IUEOSTYPE)/libCOOL.a /usr/lib/libm.a
# This list can then be used as a dependency of any executable.
ifneq ($(OS),win32)
LIB_DEPENDENCIES = $(foreach lib,$(LDLIBS:-l%=%) $(STDLIBS:-l%=%),$(firstword $(wildcard $(LIBDIRS:%=%$/lib$(lib).$(AR_EXT)))))
else
LIB_DEPENDENCIES = $(foreach lib,$(LDLIBS:-l%=%) $(STDLIBS:-l%=%), $(wildcard $(LIBDIRS:%=%$/$(lib))))
endif


# Variable: PROGRAM
# A single executable program made from $(SOURCES).
ifneq ($(strip $(PROGRAM)),)

ifndef IUE_PROGRAM_DEST
IUE_PROGRAM_DEST := $(RELOBJDIR)
endif

progfile := $(IUE_PROGRAM_DEST)$/$(PROGRAM)$(EXESUFFIX)$(PROGSUFFIX)

xall:: $(progfile)

ifdef MAKE_SHARED
 ## Assume no lib deps for shared builds
 statlibs :=
 strip_cmd = @$(EMPTY_COMMAND)
else
 statlibs := $(LIB_DEPENDENCIES)
 strip_cmd = $(STRIP) $(link_tmpname)
endif

ifneq "" "$(strip $(LINK_INTO_TMP))"
 ifeq ($(OS),win32)
  link_tmpname = tmplink.$(@F).$(subst /,_,$(OBJDIR))
 else
  ifeq "" "$(strip $(TMPDIR))"
   TMPDIR := /tmp
  endif
  link_tmpname = $(TMPDIR)/xxx.tmplink.$(@F).$(subst /,_,$(OBJDIR))
 endif
 link_op = $(foreach @,$(link_tmpname),$(LINK_OUT_OPTION))
 link_mv = mv $(link_tmpname) $@
else
 link_tmpname = $@
 link_op = $(LINK_OUT_OPTION)
 link_mv = @$(EMPTY_COMMAND)
endif # LINK_INTO_TMP

ifndef SUNPRO_REPO_HACK
SUNPRO_REPO_HACK := @true
endif

##  To use purify, see the definition of MAKE_EXE
$(progfile): $(SOURCES_obj) $(SOURCES_res) $(OBJECTS) $(statlibs)
	@echo ""
	@echo "-- Linking Program $(PROGRAM)"
	$(MAKE_EXE) $(link_op) \
	$(SOURCES_obj) \
	$(SOURCES_res) \
	$(OBJECTS)  \
	$(BUGFIX_OBJECTS) \
	$(tj_LDFLAGS) \
	$(LDPATH) \
	$(LINK_TESTLIB) \
	$(LDLIBS) \
	$(STDLIBS) \
	$(LDOPTS)
	$(SUNPRO_REPO_HACK)
	$(strip_cmd)
	-$(RM) $(CLEAN_FILES)
	$(link_mv)

endif # PROGRAM
############################### END PROGRAM  ################################
#############################################################################

################# 
# Variable: MINI_PROG_SOURCES
# Single source files to be compiled and linked against LDLIBS.
ifneq ($(strip $(MINI_PROG_SOURCES)),)

# Internal: MINI_PROG_BASENAMES
# 1-1 correspondence between basenames of MINI_PROG_EXES
MINI_PROG_BASENAMES = $(basename $(MINI_PROG_SOURCES))

# Internal: MINI_PROG_EXES
# The list of executables generated from MINI_PROG_SOURCES
MINI_PROG_EXES += $(patsubst %,$(RELOBJDIR)$/%$(EXESUFFIX)$(PROGSUFFIX),$(MINI_PROG_BASENAMES))

xall:: $(MINI_PROG_EXES)

# Additional dependencies for each mini_prog_exe. No "for" loop, so have to do this by hand
$(word 1, $(MINI_PROG_EXES)): $(OBJECTS_$(word 1, $(MINI_PROG_BASENAMES)))
$(word 2, $(MINI_PROG_EXES)): $(OBJECTS_$(word 2, $(MINI_PROG_BASENAMES)))
$(word 3, $(MINI_PROG_EXES)): $(OBJECTS_$(word 3, $(MINI_PROG_BASENAMES)))
$(word 4, $(MINI_PROG_EXES)): $(OBJECTS_$(word 4, $(MINI_PROG_BASENAMES)))
$(word 5, $(MINI_PROG_EXES)): $(OBJECTS_$(word 5, $(MINI_PROG_BASENAMES)))
$(word 6, $(MINI_PROG_EXES)): $(OBJECTS_$(word 6, $(MINI_PROG_BASENAMES)))
$(word 7, $(MINI_PROG_EXES)): $(OBJECTS_$(word 7, $(MINI_PROG_BASENAMES)))
$(word 8, $(MINI_PROG_EXES)): $(OBJECTS_$(word 8, $(MINI_PROG_BASENAMES)))
$(word 9, $(MINI_PROG_EXES)): $(OBJECTS_$(word 9, $(MINI_PROG_BASENAMES)))
$(word 10, $(MINI_PROG_EXES)): $(OBJECTS_$(word 10, $(MINI_PROG_BASENAMES)))
$(word 11, $(MINI_PROG_EXES)): $(OBJECTS_$(word 11, $(MINI_PROG_BASENAMES)))
$(word 12, $(MINI_PROG_EXES)): $(OBJECTS_$(word 12, $(MINI_PROG_BASENAMES)))
$(word 13, $(MINI_PROG_EXES)): $(OBJECTS_$(word 13, $(MINI_PROG_BASENAMES)))
$(word 14, $(MINI_PROG_EXES)): $(OBJECTS_$(word 14, $(MINI_PROG_BASENAMES)))
$(word 15, $(MINI_PROG_EXES)): $(OBJECTS_$(word 15, $(MINI_PROG_BASENAMES)))
$(word 16, $(MINI_PROG_EXES)): $(OBJECTS_$(word 16, $(MINI_PROG_BASENAMES)))
$(word 17, $(MINI_PROG_EXES)): $(OBJECTS_$(word 17, $(MINI_PROG_BASENAMES)))
$(word 18, $(MINI_PROG_EXES)): $(OBJECTS_$(word 18, $(MINI_PROG_BASENAMES)))
$(word 19, $(MINI_PROG_EXES)): $(OBJECTS_$(word 19, $(MINI_PROG_BASENAMES)))
$(word 20, $(MINI_PROG_EXES)): $(OBJECTS_$(word 20, $(MINI_PROG_BASENAMES)))
$(word 21, $(MINI_PROG_EXES)): $(OBJECTS_$(word 21, $(MINI_PROG_BASENAMES)))
$(word 22, $(MINI_PROG_EXES)): $(OBJECTS_$(word 22, $(MINI_PROG_BASENAMES)))
$(word 23, $(MINI_PROG_EXES)): $(OBJECTS_$(word 23, $(MINI_PROG_BASENAMES)))
$(word 24, $(MINI_PROG_EXES)): $(OBJECTS_$(word 24, $(MINI_PROG_BASENAMES)))
$(word 25, $(MINI_PROG_EXES)): $(OBJECTS_$(word 25, $(MINI_PROG_BASENAMES)))
$(word 26, $(MINI_PROG_EXES)): $(OBJECTS_$(word 26, $(MINI_PROG_BASENAMES)))
$(word 27, $(MINI_PROG_EXES)): $(OBJECTS_$(word 27, $(MINI_PROG_BASENAMES)))
$(word 28, $(MINI_PROG_EXES)): $(OBJECTS_$(word 28, $(MINI_PROG_BASENAMES)))
$(word 29, $(MINI_PROG_EXES)): $(OBJECTS_$(word 29, $(MINI_PROG_BASENAMES)))
$(word 30, $(MINI_PROG_EXES)): $(OBJECTS_$(word 30, $(MINI_PROG_BASENAMES)))
$(word 31, $(MINI_PROG_EXES)): $(OBJECTS_$(word 31, $(MINI_PROG_BASENAMES)))
$(word 32, $(MINI_PROG_EXES)): $(OBJECTS_$(word 32, $(MINI_PROG_BASENAMES)))
$(word 33, $(MINI_PROG_EXES)): $(OBJECTS_$(word 33, $(MINI_PROG_BASENAMES)))
$(word 34, $(MINI_PROG_EXES)): $(OBJECTS_$(word 34, $(MINI_PROG_BASENAMES)))
$(word 35, $(MINI_PROG_EXES)): $(OBJECTS_$(word 35, $(MINI_PROG_BASENAMES)))
$(word 36, $(MINI_PROG_EXES)): $(OBJECTS_$(word 36, $(MINI_PROG_BASENAMES)))
$(word 37, $(MINI_PROG_EXES)): $(OBJECTS_$(word 37, $(MINI_PROG_BASENAMES)))
$(word 38, $(MINI_PROG_EXES)): $(OBJECTS_$(word 38, $(MINI_PROG_BASENAMES)))
$(word 39, $(MINI_PROG_EXES)): $(OBJECTS_$(word 39, $(MINI_PROG_BASENAMES)))
$(word 40, $(MINI_PROG_EXES)): $(OBJECTS_$(word 40, $(MINI_PROG_BASENAMES)))
$(word 41, $(MINI_PROG_EXES)): $(OBJECTS_$(word 41, $(MINI_PROG_BASENAMES)))
$(word 42, $(MINI_PROG_EXES)): $(OBJECTS_$(word 42, $(MINI_PROG_BASENAMES)))
$(word 43, $(MINI_PROG_EXES)): $(OBJECTS_$(word 43, $(MINI_PROG_BASENAMES)))
$(word 44, $(MINI_PROG_EXES)): $(OBJECTS_$(word 44, $(MINI_PROG_BASENAMES)))
$(word 45, $(MINI_PROG_EXES)): $(OBJECTS_$(word 45, $(MINI_PROG_BASENAMES)))
$(word 46, $(MINI_PROG_EXES)): $(OBJECTS_$(word 46, $(MINI_PROG_BASENAMES)))
$(word 47, $(MINI_PROG_EXES)): $(OBJECTS_$(word 47, $(MINI_PROG_BASENAMES)))
$(word 48, $(MINI_PROG_EXES)): $(OBJECTS_$(word 48, $(MINI_PROG_BASENAMES)))
$(word 49, $(MINI_PROG_EXES)): $(OBJECTS_$(word 49, $(MINI_PROG_BASENAMES)))
$(word 50, $(MINI_PROG_EXES)): $(OBJECTS_$(word 50, $(MINI_PROG_BASENAMES)))
$(word 51, $(MINI_PROG_EXES)): $(OBJECTS_$(word 51, $(MINI_PROG_BASENAMES)))
$(word 52, $(MINI_PROG_EXES)): $(OBJECTS_$(word 52, $(MINI_PROG_BASENAMES)))
$(word 53, $(MINI_PROG_EXES)): $(OBJECTS_$(word 53, $(MINI_PROG_BASENAMES)))
$(word 54, $(MINI_PROG_EXES)): $(OBJECTS_$(word 54, $(MINI_PROG_BASENAMES)))
$(word 55, $(MINI_PROG_EXES)): $(OBJECTS_$(word 55, $(MINI_PROG_BASENAMES)))
$(word 56, $(MINI_PROG_EXES)): $(OBJECTS_$(word 56, $(MINI_PROG_BASENAMES)))
$(word 57, $(MINI_PROG_EXES)): $(OBJECTS_$(word 57, $(MINI_PROG_BASENAMES)))
$(word 58, $(MINI_PROG_EXES)): $(OBJECTS_$(word 58, $(MINI_PROG_BASENAMES)))
$(word 59, $(MINI_PROG_EXES)): $(OBJECTS_$(word 59, $(MINI_PROG_BASENAMES)))
$(word 60, $(MINI_PROG_EXES)): $(OBJECTS_$(word 60, $(MINI_PROG_BASENAMES)))
$(word 61, $(MINI_PROG_EXES)): $(OBJECTS_$(word 61, $(MINI_PROG_BASENAMES)))
$(word 62, $(MINI_PROG_EXES)): $(OBJECTS_$(word 62, $(MINI_PROG_BASENAMES)))
$(word 63, $(MINI_PROG_EXES)): $(OBJECTS_$(word 63, $(MINI_PROG_BASENAMES)))
$(word 64, $(MINI_PROG_EXES)): $(OBJECTS_$(word 64, $(MINI_PROG_BASENAMES)))
$(word 65, $(MINI_PROG_EXES)): $(OBJECTS_$(word 65, $(MINI_PROG_BASENAMES)))
$(word 66, $(MINI_PROG_EXES)): $(OBJECTS_$(word 66, $(MINI_PROG_BASENAMES)))
$(word 67, $(MINI_PROG_EXES)): $(OBJECTS_$(word 67, $(MINI_PROG_BASENAMES)))
$(word 68, $(MINI_PROG_EXES)): $(OBJECTS_$(word 68, $(MINI_PROG_BASENAMES)))
$(word 69, $(MINI_PROG_EXES)): $(OBJECTS_$(word 69, $(MINI_PROG_BASENAMES)))
$(word 70, $(MINI_PROG_EXES)): $(OBJECTS_$(word 70, $(MINI_PROG_BASENAMES)))
$(word 71, $(MINI_PROG_EXES)): $(OBJECTS_$(word 71, $(MINI_PROG_BASENAMES)))
$(word 72, $(MINI_PROG_EXES)): $(OBJECTS_$(word 72, $(MINI_PROG_BASENAMES)))
$(word 73, $(MINI_PROG_EXES)): $(OBJECTS_$(word 73, $(MINI_PROG_BASENAMES)))
$(word 74, $(MINI_PROG_EXES)): $(OBJECTS_$(word 74, $(MINI_PROG_BASENAMES)))
$(word 75, $(MINI_PROG_EXES)): $(OBJECTS_$(word 75, $(MINI_PROG_BASENAMES)))
$(word 76, $(MINI_PROG_EXES)): $(OBJECTS_$(word 76, $(MINI_PROG_BASENAMES)))
$(word 77, $(MINI_PROG_EXES)): $(OBJECTS_$(word 77, $(MINI_PROG_BASENAMES)))
$(word 78, $(MINI_PROG_EXES)): $(OBJECTS_$(word 78, $(MINI_PROG_BASENAMES)))
$(word 79, $(MINI_PROG_EXES)): $(OBJECTS_$(word 79, $(MINI_PROG_BASENAMES)))
$(word 80, $(MINI_PROG_EXES)): $(OBJECTS_$(word 80, $(MINI_PROG_BASENAMES)))
$(word 81, $(MINI_PROG_EXES)): $(OBJECTS_$(word 81, $(MINI_PROG_BASENAMES)))
$(word 82, $(MINI_PROG_EXES)): $(OBJECTS_$(word 82, $(MINI_PROG_BASENAMES)))
$(word 83, $(MINI_PROG_EXES)): $(OBJECTS_$(word 83, $(MINI_PROG_BASENAMES)))
$(word 84, $(MINI_PROG_EXES)): $(OBJECTS_$(word 84, $(MINI_PROG_BASENAMES)))
$(word 85, $(MINI_PROG_EXES)): $(OBJECTS_$(word 85, $(MINI_PROG_BASENAMES)))
$(word 86, $(MINI_PROG_EXES)): $(OBJECTS_$(word 86, $(MINI_PROG_BASENAMES)))
$(word 87, $(MINI_PROG_EXES)): $(OBJECTS_$(word 87, $(MINI_PROG_BASENAMES)))
$(word 88, $(MINI_PROG_EXES)): $(OBJECTS_$(word 88, $(MINI_PROG_BASENAMES)))
$(word 89, $(MINI_PROG_EXES)): $(OBJECTS_$(word 89, $(MINI_PROG_BASENAMES)))
$(word 90, $(MINI_PROG_EXES)): $(OBJECTS_$(word 90, $(MINI_PROG_BASENAMES)))
$(word 91, $(MINI_PROG_EXES)): $(OBJECTS_$(word 91, $(MINI_PROG_BASENAMES)))
$(word 92, $(MINI_PROG_EXES)): $(OBJECTS_$(word 92, $(MINI_PROG_BASENAMES)))
$(word 93, $(MINI_PROG_EXES)): $(OBJECTS_$(word 93, $(MINI_PROG_BASENAMES)))
$(word 94, $(MINI_PROG_EXES)): $(OBJECTS_$(word 94, $(MINI_PROG_BASENAMES)))
$(word 95, $(MINI_PROG_EXES)): $(OBJECTS_$(word 95, $(MINI_PROG_BASENAMES)))
$(word 96, $(MINI_PROG_EXES)): $(OBJECTS_$(word 96, $(MINI_PROG_BASENAMES)))
$(word 97, $(MINI_PROG_EXES)): $(OBJECTS_$(word 97, $(MINI_PROG_BASENAMES)))
$(word 98, $(MINI_PROG_EXES)): $(OBJECTS_$(word 98, $(MINI_PROG_BASENAMES)))
$(word 99, $(MINI_PROG_EXES)): $(OBJECTS_$(word 99, $(MINI_PROG_BASENAMES)))
$(word 100, $(MINI_PROG_EXES)): $(OBJECTS_$(word 100, $(MINI_PROG_BASENAMES)))
$(word 101, $(MINI_PROG_EXES)): $(OBJECTS_$(word 101, $(MINI_PROG_BASENAMES)))
$(word 102, $(MINI_PROG_EXES)): $(OBJECTS_$(word 102, $(MINI_PROG_BASENAMES)))
$(word 103, $(MINI_PROG_EXES)): $(OBJECTS_$(word 103, $(MINI_PROG_BASENAMES)))
$(word 104, $(MINI_PROG_EXES)): $(OBJECTS_$(word 104, $(MINI_PROG_BASENAMES)))
$(word 105, $(MINI_PROG_EXES)): $(OBJECTS_$(word 105, $(MINI_PROG_BASENAMES)))
$(word 106, $(MINI_PROG_EXES)): $(OBJECTS_$(word 106, $(MINI_PROG_BASENAMES)))
$(word 107, $(MINI_PROG_EXES)): $(OBJECTS_$(word 107, $(MINI_PROG_BASENAMES)))
$(word 108, $(MINI_PROG_EXES)): $(OBJECTS_$(word 108, $(MINI_PROG_BASENAMES)))
$(word 109, $(MINI_PROG_EXES)): $(OBJECTS_$(word 109, $(MINI_PROG_BASENAMES)))
$(word 110, $(MINI_PROG_EXES)): $(OBJECTS_$(word 110, $(MINI_PROG_BASENAMES)))
$(word 111, $(MINI_PROG_EXES)): $(OBJECTS_$(word 111, $(MINI_PROG_BASENAMES)))
$(word 112, $(MINI_PROG_EXES)): $(OBJECTS_$(word 112, $(MINI_PROG_BASENAMES)))
$(word 113, $(MINI_PROG_EXES)): $(OBJECTS_$(word 113, $(MINI_PROG_BASENAMES)))
$(word 114, $(MINI_PROG_EXES)): $(OBJECTS_$(word 114, $(MINI_PROG_BASENAMES)))
$(word 115, $(MINI_PROG_EXES)): $(OBJECTS_$(word 115, $(MINI_PROG_BASENAMES)))
$(word 116, $(MINI_PROG_EXES)): $(OBJECTS_$(word 116, $(MINI_PROG_BASENAMES)))
$(word 117, $(MINI_PROG_EXES)): $(OBJECTS_$(word 117, $(MINI_PROG_BASENAMES)))
$(word 118, $(MINI_PROG_EXES)): $(OBJECTS_$(word 118, $(MINI_PROG_BASENAMES)))
$(word 119, $(MINI_PROG_EXES)): $(OBJECTS_$(word 119, $(MINI_PROG_BASENAMES)))
$(word 120, $(MINI_PROG_EXES)): $(OBJECTS_$(word 120, $(MINI_PROG_BASENAMES)))
$(word 121, $(MINI_PROG_EXES)): $(OBJECTS_$(word 121, $(MINI_PROG_BASENAMES)))
$(word 122, $(MINI_PROG_EXES)): $(OBJECTS_$(word 122, $(MINI_PROG_BASENAMES)))
$(word 123, $(MINI_PROG_EXES)): $(OBJECTS_$(word 123, $(MINI_PROG_BASENAMES)))
$(word 124, $(MINI_PROG_EXES)): $(OBJECTS_$(word 124, $(MINI_PROG_BASENAMES)))
$(word 125, $(MINI_PROG_EXES)): $(OBJECTS_$(word 125, $(MINI_PROG_BASENAMES)))
$(word 126, $(MINI_PROG_EXES)): $(OBJECTS_$(word 126, $(MINI_PROG_BASENAMES)))
$(word 127, $(MINI_PROG_EXES)): $(OBJECTS_$(word 127, $(MINI_PROG_BASENAMES)))
$(word 128, $(MINI_PROG_EXES)): $(OBJECTS_$(word 128, $(MINI_PROG_BASENAMES)))
$(word 129, $(MINI_PROG_EXES)): $(OBJECTS_$(word 129, $(MINI_PROG_BASENAMES)))
$(word 130, $(MINI_PROG_EXES)): $(OBJECTS_$(word 130, $(MINI_PROG_BASENAMES)))
$(word 131, $(MINI_PROG_EXES)): $(OBJECTS_$(word 131, $(MINI_PROG_BASENAMES)))
$(word 132, $(MINI_PROG_EXES)): $(OBJECTS_$(word 132, $(MINI_PROG_BASENAMES)))
$(word 133, $(MINI_PROG_EXES)): $(OBJECTS_$(word 133, $(MINI_PROG_BASENAMES)))
$(word 134, $(MINI_PROG_EXES)): $(OBJECTS_$(word 134, $(MINI_PROG_BASENAMES)))
$(word 135, $(MINI_PROG_EXES)): $(OBJECTS_$(word 135, $(MINI_PROG_BASENAMES)))
$(word 136, $(MINI_PROG_EXES)): $(OBJECTS_$(word 136, $(MINI_PROG_BASENAMES)))
$(word 137, $(MINI_PROG_EXES)): $(OBJECTS_$(word 137, $(MINI_PROG_BASENAMES)))
$(word 138, $(MINI_PROG_EXES)): $(OBJECTS_$(word 138, $(MINI_PROG_BASENAMES)))
$(word 139, $(MINI_PROG_EXES)): $(OBJECTS_$(word 139, $(MINI_PROG_BASENAMES)))
$(word 140, $(MINI_PROG_EXES)): $(OBJECTS_$(word 140, $(MINI_PROG_BASENAMES)))
$(word 141, $(MINI_PROG_EXES)): $(OBJECTS_$(word 141, $(MINI_PROG_BASENAMES)))
$(word 142, $(MINI_PROG_EXES)): $(OBJECTS_$(word 142, $(MINI_PROG_BASENAMES)))
$(word 143, $(MINI_PROG_EXES)): $(OBJECTS_$(word 143, $(MINI_PROG_BASENAMES)))
$(word 144, $(MINI_PROG_EXES)): $(OBJECTS_$(word 144, $(MINI_PROG_BASENAMES)))
$(word 145, $(MINI_PROG_EXES)): $(OBJECTS_$(word 145, $(MINI_PROG_BASENAMES)))
$(word 146, $(MINI_PROG_EXES)): $(OBJECTS_$(word 146, $(MINI_PROG_BASENAMES)))
$(word 147, $(MINI_PROG_EXES)): $(OBJECTS_$(word 147, $(MINI_PROG_BASENAMES)))
$(word 148, $(MINI_PROG_EXES)): $(OBJECTS_$(word 148, $(MINI_PROG_BASENAMES)))
$(word 149, $(MINI_PROG_EXES)): $(OBJECTS_$(word 149, $(MINI_PROG_BASENAMES)))
$(word 150, $(MINI_PROG_EXES)): $(OBJECTS_$(word 150, $(MINI_PROG_BASENAMES)))
$(word 151, $(MINI_PROG_EXES)): $(OBJECTS_$(word 151, $(MINI_PROG_BASENAMES)))
$(word 152, $(MINI_PROG_EXES)): $(OBJECTS_$(word 152, $(MINI_PROG_BASENAMES)))
$(word 153, $(MINI_PROG_EXES)): $(OBJECTS_$(word 153, $(MINI_PROG_BASENAMES)))
$(word 154, $(MINI_PROG_EXES)): $(OBJECTS_$(word 154, $(MINI_PROG_BASENAMES)))
$(word 155, $(MINI_PROG_EXES)): $(OBJECTS_$(word 155, $(MINI_PROG_BASENAMES)))
$(word 156, $(MINI_PROG_EXES)): $(OBJECTS_$(word 156, $(MINI_PROG_BASENAMES)))
$(word 157, $(MINI_PROG_EXES)): $(OBJECTS_$(word 157, $(MINI_PROG_BASENAMES)))
$(word 158, $(MINI_PROG_EXES)): $(OBJECTS_$(word 158, $(MINI_PROG_BASENAMES)))
$(word 159, $(MINI_PROG_EXES)): $(OBJECTS_$(word 159, $(MINI_PROG_BASENAMES)))
$(word 160, $(MINI_PROG_EXES)): $(OBJECTS_$(word 160, $(MINI_PROG_BASENAMES)))
$(word 161, $(MINI_PROG_EXES)): $(OBJECTS_$(word 161, $(MINI_PROG_BASENAMES)))
$(word 162, $(MINI_PROG_EXES)): $(OBJECTS_$(word 162, $(MINI_PROG_BASENAMES)))
$(word 163, $(MINI_PROG_EXES)): $(OBJECTS_$(word 163, $(MINI_PROG_BASENAMES)))
$(word 164, $(MINI_PROG_EXES)): $(OBJECTS_$(word 164, $(MINI_PROG_BASENAMES)))
$(word 165, $(MINI_PROG_EXES)): $(OBJECTS_$(word 165, $(MINI_PROG_BASENAMES)))
$(word 166, $(MINI_PROG_EXES)): $(OBJECTS_$(word 166, $(MINI_PROG_BASENAMES)))
$(word 167, $(MINI_PROG_EXES)): $(OBJECTS_$(word 167, $(MINI_PROG_BASENAMES)))
$(word 168, $(MINI_PROG_EXES)): $(OBJECTS_$(word 168, $(MINI_PROG_BASENAMES)))
$(word 169, $(MINI_PROG_EXES)): $(OBJECTS_$(word 169, $(MINI_PROG_BASENAMES)))
$(word 170, $(MINI_PROG_EXES)): $(OBJECTS_$(word 170, $(MINI_PROG_BASENAMES)))
$(word 171, $(MINI_PROG_EXES)): $(OBJECTS_$(word 171, $(MINI_PROG_BASENAMES)))
$(word 172, $(MINI_PROG_EXES)): $(OBJECTS_$(word 172, $(MINI_PROG_BASENAMES)))
$(word 173, $(MINI_PROG_EXES)): $(OBJECTS_$(word 173, $(MINI_PROG_BASENAMES)))
$(word 174, $(MINI_PROG_EXES)): $(OBJECTS_$(word 174, $(MINI_PROG_BASENAMES)))
$(word 175, $(MINI_PROG_EXES)): $(OBJECTS_$(word 175, $(MINI_PROG_BASENAMES)))
$(word 176, $(MINI_PROG_EXES)): $(OBJECTS_$(word 176, $(MINI_PROG_BASENAMES)))
$(word 177, $(MINI_PROG_EXES)): $(OBJECTS_$(word 177, $(MINI_PROG_BASENAMES)))
$(word 178, $(MINI_PROG_EXES)): $(OBJECTS_$(word 178, $(MINI_PROG_BASENAMES)))
$(word 179, $(MINI_PROG_EXES)): $(OBJECTS_$(word 179, $(MINI_PROG_BASENAMES)))
$(word 180, $(MINI_PROG_EXES)): $(OBJECTS_$(word 180, $(MINI_PROG_BASENAMES)))
$(word 181, $(MINI_PROG_EXES)): $(OBJECTS_$(word 181, $(MINI_PROG_BASENAMES)))
$(word 182, $(MINI_PROG_EXES)): $(OBJECTS_$(word 182, $(MINI_PROG_BASENAMES)))
$(word 183, $(MINI_PROG_EXES)): $(OBJECTS_$(word 183, $(MINI_PROG_BASENAMES)))
$(word 184, $(MINI_PROG_EXES)): $(OBJECTS_$(word 184, $(MINI_PROG_BASENAMES)))
$(word 185, $(MINI_PROG_EXES)): $(OBJECTS_$(word 185, $(MINI_PROG_BASENAMES)))
$(word 186, $(MINI_PROG_EXES)): $(OBJECTS_$(word 186, $(MINI_PROG_BASENAMES)))
$(word 187, $(MINI_PROG_EXES)): $(OBJECTS_$(word 187, $(MINI_PROG_BASENAMES)))
$(word 188, $(MINI_PROG_EXES)): $(OBJECTS_$(word 188, $(MINI_PROG_BASENAMES)))
$(word 189, $(MINI_PROG_EXES)): $(OBJECTS_$(word 189, $(MINI_PROG_BASENAMES)))
$(word 190, $(MINI_PROG_EXES)): $(OBJECTS_$(word 190, $(MINI_PROG_BASENAMES)))
$(word 191, $(MINI_PROG_EXES)): $(OBJECTS_$(word 191, $(MINI_PROG_BASENAMES)))
$(word 192, $(MINI_PROG_EXES)): $(OBJECTS_$(word 192, $(MINI_PROG_BASENAMES)))
$(word 193, $(MINI_PROG_EXES)): $(OBJECTS_$(word 193, $(MINI_PROG_BASENAMES)))
$(word 194, $(MINI_PROG_EXES)): $(OBJECTS_$(word 194, $(MINI_PROG_BASENAMES)))
$(word 195, $(MINI_PROG_EXES)): $(OBJECTS_$(word 195, $(MINI_PROG_BASENAMES)))
$(word 196, $(MINI_PROG_EXES)): $(OBJECTS_$(word 196, $(MINI_PROG_BASENAMES)))
$(word 197, $(MINI_PROG_EXES)): $(OBJECTS_$(word 197, $(MINI_PROG_BASENAMES)))
$(word 198, $(MINI_PROG_EXES)): $(OBJECTS_$(word 198, $(MINI_PROG_BASENAMES)))
$(word 199, $(MINI_PROG_EXES)): $(OBJECTS_$(word 199, $(MINI_PROG_BASENAMES)))
$(word 200, $(MINI_PROG_EXES)): $(OBJECTS_$(word 200, $(MINI_PROG_BASENAMES)))

## Check that MINI_PROG_BASENAMES shorter than 200 elements
# ( SpatialObject-IUE/IUE-SO/Tests/makefile has 192 MINI_PROG_SOURCES )
ifneq (,$(word 200, $(MINI_PROG_EXES)))
xall::
	@echo rules.mk:$.: FIXME, MINI_PROG_EXES longer than 200 elements.
	false
endif

ifdef USE_TMPLCLOSURE

IUESCANLINKFORINSTANCESCMD = $(IUE_PERL) -x $(sys_or_iu_Scripts)/Perl/_iuescanlinkforinstances
# Add the parameters  -v -T=progs for debugging output for iueinstantiateclosure
IUEINSTANTIATECLOSURECMD = IUE_OBJDIR=${OBJDIR} $(IUE_PERL) -w -x $(sys_or_iu_Scripts)/Perl/iueinstantiateclosure 

define compile-tmpl-insts
	if [ -d Templates -a \! -z "$(COMPILING_TEMPLATES)" ]; then \
	  if [ \! -z "`ls Templates`" ]; then \
	    echo ""; \
	    echo "-- Compiling Template Instances"; \
	    $(MAKE) COMPILING_TEMPLATES=1; \
	  fi; \
	fi
endef

# add IUE_DEBUG_TEMPLATES=1 to make command line if you want to save
# intermediate instantiation files and generate verbose output
ifdef IUE_DEBUG_TEMPLATES
  override IUE_DEBUG_TEMPLATES=-v
endif

# jb@aai: removed $(tj_CGFLAGS) from the command line below because
# the $(pic) was getting into the link, which was causing problems
# in certain cases (alphatech).  Also, code isn't really being
# generated at link time anyway (or is it??)
# Dependencies:
# 1. Make sure that exes depend on the template libraries
# 2. Link in individual Templates object files (because this is for USE_TMPLCLOSURE).
# 3. MINI_PROG_EXES depend on $(allsources_obj), although they only use $(OBJECTS_progname),
#    because we can't get "make" to evaluate $(OBJECTS_progname) late enough.  This is ok,
#    it just means that all objects are made before any programs are linked.
# 

rm_cond = true
ifndef IUE_DEBUG_TEMPLATES
rm_cond = rm -f
endif

$(MINI_PROG_EXES): %$(EXESUFFIX)$(PROGSUFFIX): %.$(OBJ_EXT) $(SOURCES_obj_tmpl) $(LIB_DEPENDENCIES) $(libfile) makefile
	@if [ -z "$(COMPILING_TEMPLATES)" ]; then $(compile-tmpl-insts); fi
	@echo ""
	@echo "-- Linking Mini Program $@ $(due_to)"
	@linkout=$(TMPDIR)/linkout$$$$; \
	instances=$(TMPDIR)/instances$$$$; \
	lstatfile=$(TMPDIR)/linkstatus$$$$; \
	if [ -f $$lstatfile ]; then echo rm $$lstatfile; rm $$lstatfile; fi; \
	echo "--   Undefined symbols may indicate missing template instances."; \
	echo "--   Template instantiator will try to resolve them."; \
	echo $(MAKE_EXE) $(LINK_OUT_OPTION) $(firstword $^) \
	$(OBJECTS_$(basename $(@F))) \
	$(allsources_obj_tmpl) \
	$(BUGFIX_OBJECTS) \
	$(tj_LDFLAGS) \
	$(LDPATH) \
	$(LINK_TESTLIB) \
	$(LDLIBS) \
	$(STDLIBS) \
	$(LDOPTS) \
	\| tee $$linkout; \
	($(MAKE_EXE) $(LINK_OUT_OPTION) $(firstword $^) \
	$(OBJECTS_$(basename $(@F))) \
	$(allsources_obj_tmpl) \
	$(BUGFIX_OBJECTS) \
	$(tj_LDFLAGS) \
	$(LDPATH) \
	$(LINK_TESTLIB) \
	$(LDLIBS) \
	$(STDLIBS) \
	$(LDOPTS); \
	lstat=$$?; if [ $$lstat != 0 ]; then echo $$lstat > $$lstatfile; fi) 2>&1 | tee $$linkout; \
	estat=0; \
	if [ -f $$lstatfile ]; then \
	    echo " "; \
	    echo "-- UNDEFINED SYMBOLS MAY INDICATE MISSING TEMPLATE INSTANCES."; \
	    echo "   TRYING TO RESOLVE THEM..."; \
	    echo "-- SCANNING LINKER OUTPUT FOR TEMPLATE INSTANCES"; \
	    echo " "; \
	    echo $(IUESCANLINKFORINSTANCESCMD) $(IUE_DEBUG_TEMPLATES) -c $(firstword $(C++)) \< $$linkout \> $$instances; \
	    $(IUESCANLINKFORINSTANCESCMD) $(IUE_DEBUG_TEMPLATES) -c $(firstword $(C++)) < $$linkout > $$instances; \
	    estat=$$?; \
	    if [ $$estat != 0 ]; then \
	      echo "$(IUEINSTANTIATECLOSURECMD)" $(IUE_DEBUG_TEMPLATES) -f $$instances -L Templates; \
	      $(IUEINSTANTIATECLOSURECMD) $(IUE_DEBUG_TEMPLATES) -f $$instances -L Templates; \
	      estat=$$?; \
	    fi; \
	    if [ $$estat = 2 ]; then \
		echo " "; \
		echo "-- RERUNNING MAKE TO BRING IN NEW TEMPLATE INSTANCES"; \
		echo " "; \
		$(compile-tmpl-insts); \
		estat=$$?; \
	    else \
		echo " "; \
		echo "-- NO TEMPLATE INSTANCES CREATED.  LINK PROBABLY FAILED FOR OTHER REASONS."; \
		echo " "; \
		exit 1; \
	    fi; \
	fi; \
	$(rm_cond) $$linkout $$instances $$lstatfile; \
	exit $$estat;

else
# Issues:
# [a] should the exe depend on the template objects?
# [b] should the exe be linked against the library or with the template objects?
#
# Currently, the exe depends only on its own object file, which is fine for shared
# builds, and the template objects are not put into the exe.
#
# fsm@robots: it's annoying to depend on the template object files. Eg if I have
# a directory with a *shared* library and a miniprog linked against it, I don't want
# to relink the miniprog every time I change a template. I only want to link the
# miniprog when its source file has been recompiled.
#
# If you change things back the way they were, don't forget to change fsmSOURCES_obj_tmpl
# below too (just delete the 3-letter prefix).
#$(MINI_PROG_EXES): %$(EXESUFFIX)$(PROGSUFFIX): %.$(OBJ_EXT) $(SOURCES_obj_tmpl) $(LIB_DEPENDENCIES) $(libfile) makefile
$(MINI_PROG_EXES): %$(EXESUFFIX)$(PROGSUFFIX): %.$(OBJ_EXT) $(LIB_DEPENDENCIES) $(libfile) makefile
#	@echo ""
	@echo "rules.mk:$.: Making $@, $(due_to)"
	$(MAKE_EXE) $(LINK_OUT_OPTION) \
	$(tj_CGFLAGS) $(firstword $^) \
	$(OBJECTS_$(basename $(@F))) \
	$(fsmSOURCES_obj_tmpl) \
	$(BUGFIX_OBJECTS) \
	$(tj_LDFLAGS) \
	$(LDPATH) \
	$(LINK_TESTLIB) \
	$(LDLIBS) \
	$(STDLIBS) \
	$(LDOPTS) \
	|| ( $(RM) $@ && false )
	$(SUNPRO_REPO_HACK)
	$(strip_cmd)
	-$(RM) $(CLEAN_FILES)

endif # USE_TMPLCLOSURE

endif # MINI_PROG_SOURCES

#################
# Variable: MINI_SO_TESTS
# Mini shared libraries for TestDrivering.
# Setting this also causes TestDriver.C to be copied from
# the config directory and built.

# The shared libs will be built by MINI_SO_SOURCES, this bit adds the rules for
# running them.

ifneq ($(strip $(MINI_SO_TESTS)),)
xall:: tests

ifdef NOTEST
tests:
	@echo "Not running MINI_SO_TESTS"
else

TESTDRIVER_BASENAME := TestDriver$(EXESUFFIX)$(PROGSUFFIX)
TESTDRIVER := $(RELOBJDIR)/$(TESTDRIVER_BASENAME)

TESTDRIVER_SRC := $(configdir)/TestDriver.C

TestDriver.C: $(TESTDRIVER_SRC)
	$(RM) $@ && cp $^ $@

test_targets := $(MINI_SO_TESTS:%.C=$(RELOBJDIR)/%.out)

tests: $(test_targets)

#fsm@robots: we don't want to run the MINI_SO_TESTS every time as it drives me scatty.
$(RELOBJDIR)/%.out: $(TESTDRIVER) $(RELOBJDIR)/$(LIB_PREFIX)%.$(LIB_EXT) #fsmFORCE
	cd $(ABSOBJDIR) && ./$(TESTDRIVER_BASENAME) $(@F:%.out=%)
	@-$(IUE_PERL) $(sys_or_iu_Scripts)/Perl/unify_pointer_names $@
	$(RM) $@.bak
	@-tail -2 $@

runtest-%: $(RELOBJDIR)/$(@:runtest-%=%).out

endif # NOTEST
endif # MINI_SO_TESTS

#################
# Variable: MINI_SO_SOURCES
# Mini shared libraries for tjrunning, made only when MAKE_SHARED is set
ifneq ($(strip $(MINI_SO_SOURCES)),)
ifdef MAKE_SHARED

MINI_SO_EXES := $(patsubst %,$(RELOBJDIR)$/$(LIB_PREFIX)%.$(LIB_EXT),$(basename $(MINI_SO_SOURCES)))

xall:: $(MINI_SO_EXES)

ifeq ($(OS),win32)
ifdef EXPORT_MAIN
WINSTUFF2 = -link -export:main $(LDLIBS) $(STDLIBS) $(LDPATH)
else
WINSTUFF = -link -export:$(basename $@) $(LDLIBS) $(STDLIBS) $(LDPATH)
WINSTUFF2 = $(subst $(RELOBJDIR)$/,,$(WINSTUFF))
endif
endif

ifeq (x$(NO_TEMPLATES_IN_MINI_SOS),x)
# link the template objects into the .so :
$(MINI_SO_EXES): $(RELOBJDIR)$/$(LIB_PREFIX)%$(LIB_EXT): $(RELOBJDIR)$/%$(OBJ_EXT) $(SOURCES_obj_tmpl)
	$(MAKE_SHLIB) $(LINKER_OUT_OPTION) $(tj_CGFLAGS) $(firstword $^) \
	$(allsources_obj_tmpl) \
	$(OBJECTS_$(basename $@)) $(WINSTUFF2)
else
# don't link the template objects into the .so :
$(MINI_SO_EXES): $(RELOBJDIR)$/$(LIB_PREFIX)%$(LIB_EXT): $(RELOBJDIR)$/%$(OBJ_EXT)
	$(MAKE_SHLIB) $(LINKER_OUT_OPTION) $(tj_CGFLAGS) $(firstword $^) $(OBJECTS_$(basename $@)) $(WINSTUFF2)
endif

endif # NOSHARED
endif # MINI_SO_SOURCES

#############################################################################
############################# SUBDIRECTORIES  ###############################

#################
# Variable: SUBDIRS
# Set to a list of subdirectories into which make should recurse.
# Note that missing directories are silently skipped (this is a feature).
actual_subdirs := $(strip $(wildcard $(SUBDIRS)))

ifneq (,$(actual_subdirs))
xall:: subdirs

# Target: subdirs
# Driver to send make into subdirectories.
subdirs:
	@ $(MAKE) BUILD=$(BUILD) $(actual_subdirs:%=%/recurse-subdirs) subdir_target=

subdirs-%:
	@ $(MAKE) BUILD=$(BUILD) $(actual_subdirs:%=%/recurse-subdirs) subdir_target=$(@:subdirs-%=%)

# NOTE: tj-buildreport.pl uses the string "Running make in " to make its report,
# so do not remove this output, or if you change the string, also make the change
# in Scripts/Perl/tj-buildreport.pl

$(actual_subdirs:%=%/recurse-subdirs): FORCE
	@if test -f $(@D)/makefile ; then ( \
	    cd $(@D) && \
	    echo ""  && \
	    echo "--------------------------------------------------------" && \
	    echo "-- Running make in `pwd`" && \
	    $(MAKE) BUILD=$(BUILD) $(subdir_target) || ( echo "*** failed in `pwd`" && false ) \
	) ; else ( \
	    echo "** No makefile in `pwd`/$(@D)" \
	) ;  fi
# Subdir failure should imply complete failure unless "-k" --awf

else
subdirs:
	@echo No SUBDIRS set in makefile actually exist

subdirs-%:
	@$(EMPTY_COMMAND)
endif



#############################################################################
##############################  DOCUMENTATION  ##############################

################# MANPAGES
# Internal: MANDIR
# Destination for autogenerated manual pages.
MANDIR := $(IUELOCALROOT)$/man$/man3
MANSUF := 3
GENMAN := gendoc -t
gendocloc = $(foreach file,Scripts/Perl/gendoc,$(sys_or_iu_macro))

massage_windex := 'if (/^Cool(\w+)\s+\w+\s+\((\w+)\)/) { print "rm -f Cool$$1 && $(LN) $$1.$$2 Cool$$1.$$2 \n"; }'

# Variable: MANPAGE_SOURCES
# If set, the definitive list of source files from which manual pages are to
# be generated.  If not, it is built from SOURCES by adding .h to the basenames.

# use wildcard function to filter out nonexistent .h files
MANPAGE_SOURCES += $(wildcard $(addsuffix .h,$(srcbase)))
# "harden" MANPAGE_SOURCES, meanwhile substituting .txx (from makefiles) for .h
MANPAGE_SOURCES := $(subst .txx,.h,$(MANPAGE_SOURCES))
# sort to make unique
MANPAGE_SOURCES := $(sort $(MANPAGE_SOURCES))

make-windex:
	catman-do $(MANDIR)/..

# subdir_target defined means we are being called by a higher make.
# This means that windex is made just once, at the top level.
ifndef subdir_target
manpage_need_windex := make-windex
endif

# Target: manpages
# Make manual pages in $(MANDIR)

ifeq "" "$(strip $(MANPAGE_SOURCES))"
# Just go into subdirs.
manpages : subdirs-manpages  $(manpage_need_windex)
else
# Make manpages here, then go into subdirs

# compute actual manpage filenames
manpage_sources_base = $(basename $(MANPAGE_SOURCES))

MANPAGES := $(manpage_sources_base:%=$(MANDIR)$/%.$(MANSUF))

mandepend = $(IUE_PERL) $(sys_or_iu_Scripts)/Perl/tj-mandepend -I.. \
	"$(MANDIR)" "$(MANPREFIX)" "$(MANSUF)" "$(MANPAGE_SOURCES)" > $(RELOBJDIR)/mandeps

$(RELOBJDIR)/mandeps:
	[ -d $(RELOBJDIR) ] || $(MKDIR_P) $(RELOBJDIR)
	$(mandepend)

-include $(RELOBJDIR)/mandeps

manpages: $(MANPAGES) subdirs-manpages $(manpage_need_windex)

manpage_source_from_dest = \
	$(wildcard $(@:$(MANDIR)$/%.$(MANSUF)=%.h)  $(@:$(MANDIR)$/%.$(MANSUF)=%)  )

$(MANPAGES) : $(MANDIR)$/%.$(MANSUF) : $(gendocloc)
	@$(MKDIR_P) $(@D)
	$(GENMAN) -I.. $(manpage_source_from_dest) > $@

# We don't want to generate these dependencies every time as it will
# drive people scatty

mandepend: FORCE
	$(mandepend)

endif # MANPAGE_SOURCES


################# HTML
# Internal: HTMLDIR
# Subdirectory into which to place HTML indices.
HTMLDIR := HTML
GENHTML := $(sys_or_iu_Scripts)$/Perl$/gendoc -h

# use wildcard function to filter out nonexistent .h files
HTMLHEADERS := $(wildcard $(MANPAGE_SOURCES))
HTMLPAGES := $(HTMLHEADERS:%.h=$(HTMLDIR)$/%.html)


# Target: htmlpages
# Make HTML manual pages in $(HTMLDIR)

htmlpages: htmlfiles

# Variable: HTMLINDEX
# Define this variable if you have a handwritten html index
# page.  If unset, one will be generated automatically
# from MANPAGE_SOURCES.
ifndef HTMLINDEX
HTMLINDEX := $(HTMLDIR)$/classes.html
htmlindex: FORCE
	@$(MKDIR_P) $(HTMLDIR)
	tj-make-htmlindex "$(MANPAGE_SOURCES)" "$(SUBDIRS)" > $(HTMLINDEX)
else
htmlindex:
endif # HTMLINDEX

htmlfiles: $(HTMLPAGES) subdirs-htmlfiles htmlindex

# Add this rule so peeps can do make HTML/Fred.html
$(HTMLDIR)$/%.html: %.h
	@$(MKDIR_P) $(@D)
	gendoc -I.. -h -colors 'BGCOLOR="#FFFFFF" TEXT="#000000" LINK="#FF0000" ALINK="#0000FF"' $(@:$(HTMLDIR)$/%.html=%.h) > $@

#############################################################################
###########################  PUBLISHED_PROGRAMS  ############################

# Variable: PUBLISHED_PROGRAMS
# Add the basenames of MINI_PROG_SOURCES to this variable to have
# them "published" in $IUELOCALROOT/bin.  For example:
#  PUBLISHED_PROGRAMS = useful_example_1 useful_example_7
#

# Make a central bindir entry for any published programs
iue_all_program_basenames := $(PUBLISHED_PROGRAMS)
ifneq "" "$(iue_all_program_basenames)"
# Check that we are in a source tree
ifneq "" "$(PKGWD)"

program_central_bindir := ${IUE_BINDIR}
iue_mk_binentry = $(sys_or_iu_Scripts)/Perl/iue-make-bindir-entry.pl

xall:: $(foreach i, $(iue_all_program_basenames), program_central_bindir-$(strip $i))

program_central_bindir-%:
	$(IUE_PERL) -x $(iue_mk_binentry) $(program_central_bindir) $(PKGWD) $(@:program_central_bindir-%=%)
endif
endif

#############################################################################
##############################  MISC TARGETS  ###############################

# Handy FORCE target.
FORCE:

.PHONY: FORCE all xall depend


# Target: depend
# Brutal depend target -- removes all .d files and remakes
depend:
	-@$(RM) $(allsources_deps)
	-@$(RM) $(RELOBJDIR)$/*.d $(RELOBJDIR)$/Templates$/*.d
	$(MAKE) FORCE

depend.%:
	$(MAKE) BUILD=$(@:depend.%=%) depend

# Target: clean
# The old reliable.  Removes .d, .o, .obj files, but not executables.
# Operates only in the object directory corresponding to the current value of BUILD.
clean::
	$(MAKE) NODEPENDS=1 do_clean

do_clean::
	-$(RM) $(allsources_deps)
	-$(RM) $(RELOBJDIR)$/*.d $(CLEAN_FILES) $(RELOBJDIR)$/*.$(OBJ_EXT)
	-$(RM) $(RELOBJDIR)$/Templates/*.d $(RELOBJDIR)$/Templates/*.$(OBJ_EXT)
ifdef RES_EXT
	-$(RM) $(RELOBJDIR)$/*.$(RES_EXT)
endif
ifdef SUBDIRS
	$(MAKE) NODEPENDS=1 subdirs-$@
endif

$(BUILDTYPES:%=clean.%) : clean.% :
	$(MAKE) BUILD=$(@:clean.%=%) clean

# Target: clean_obj_only
# Remove only object files.
clean_obj_only::
	-$(RM) $(RELOBJDIR)$/*.$(OBJ_EXT)
	-$(RM) $(RELOBJDIR)$/Templates/*.$(OBJ_EXT)
ifdef RES_EXT
	-$(RM) $(RELOBJDIR)$/*.$(RES_EXT)
endif
ifdef SUBDIRS
	$(MAKE) NODEPENDS=1 subdirs-$@
endif

# Target: clean_dep_only
# Remove only .d files.
clean_dep_only::  subdirs-clean_dep_only
	-$(RM) $(allsources_deps)
	-$(RM) $(RELOBJDIR)$/*.d $(RELOBJDIR)$/Templates/*.d

# Target: clean_prog
# Remove executables, but not objects or dependencies
clean_prog:: subdirs-clean_prog

ifneq ($(strip $(PROGRAM)),)
clean_prog::
	$(RM) $(progfile)
endif
ifneq ($(strip $(MINI_SO_EXES)),)
clean_prog::
	$(RM) $(MINI_SO_EXES)
endif

ifneq ($(strip $(MINI_PROG_EXES)),)
clean_prog::
	$(RM) $(MINI_PROG_EXES)
endif

# Target: dclean
# Remove object and lib directories
RMRF=$(RM) -r
dclean::
	@echo RM in `pwd`
	-$(RMRF) $(RELOBJDIR) $(LIBDEST)
ifdef SUBDIRS
	$(MAKE) $(iue-no-print-directory) NODEPENDS=1 subdirs-dclean
endif


#======================================================================
#  Find-base clean targets (faster but allows no control by submakes)
#======================================================================
ifeq ($(strip $(OS)),win32)
# on the PC, rm is under cygwin, not /bin
RM_CMD = rm
else
RM_CMD = /bin/rm
endif
#----------------------------------------------------------------------
# get rid of everything generated for this architecture.
#----------------------------------------------------------------------
fastfullclean:
	find . -name $(notdir $(OBJDIR)) -type d -prune -print | xargs $(RM_CMD) -rf
	find . -name $(notdir $(LIBDEST)) -type d -prune -print | xargs $(RM_CMD) -rf


#----------------------------------------------------------------------
# get rid of everything generated for this architecture and template instances
#----------------------------------------------------------------------
fastfullcleant:
	find . -name $(notdir $(OBJDIR)) -type d -prune -print | xargs $(RM_CMD) -rf
	find . -name $(notdir $(LIBDEST)) -type d -prune -print | xargs $(RM_CMD) -rf
	find . -name Templates -prune -print | xargs $(RM_CMD) -rf

#----------------------------------------------------------------------
# keep libraries and executables
#----------------------------------------------------------------------
fastclean:
	@dirs=`find . \( -name $(notdir $(OBJDIR)) -o -name $(notdir $(LIBDEST)) \) -prune -print`; \
	for d in $$dirs; do \
	  echo cleaning $$d; \
	  rm -rf $$d/*.o $$d/core $$d/*.d; \
	done

#----------------------------------------------------------------------
# keep libraries, executables, and depends
#----------------------------------------------------------------------
fastclean-keepdepends:
	@dirs=`find . \( -name $(notdir $(OBJDIR)) -o -name $(tj_libdir) \) -prune -print`; \
	for d in $$dirs; do \
	  echo cleaning $$d; \
	  rm -rf $$d/*.o $$d/core; \
	done


# Target: cpp-%
# make cpp-File.C will print the output of running the C preprocessor on File.C,
# using all appropriate IUE command-line switches.

iue_cppfree = $(@:cpp-%=%)

cpp-% cpp-Templates/%: FORCE
	$(C++) $(iue_cppfree) $(tj_CCFLAGS) $(FLAGS_$(iue_cppfree)) $(PREPROC_FLAG)


# Target: echovar-%
# Handy target to print a "make" variable's value e.g.
# make echovar-SOURCES
# ECHOVAR MUST BE USABLE IN SCRIPTS.  FOR PRETTY PRINTING USE SHOWVAR.
echovar-%:
	@echo "$($(@:echovar-%=%))"

# Target: showvar-%
# Pretty-printing version of echovar.  We need a seperate showvar because
# echovar must be usable in scripts.
showvar-%:
	@echo $(@:showvar-%=%) = \"$($(@:showvar-%=%))\"

# Target: originvar-%
# Show origin of a make variable
originvar-%:
	@echo $($*)
	@echo origin: $(origin $*)

# include this 
-include $(configdir)/rules-$(OS)-$(COMPILER).mk

#--------------------------------------------------------------
#  Some stuff for getting linking information - not guaranteed on all platforms.
#  Written by R. Hartley (hartley@crd.ge.com).
#--------------------------------------------------------------
RHLINKINFO = $(IUELOCALROOT)/RHUtilities/interpreter/linkinfo
link.out :
	@echo ""
	@echo Link report ...
	$(RHLINKINFO)/linksummary $(link_op) $(SOURCES_obj) $(SOURCES_res) $(OBJECTS)  $(BUGFIX_OBJECTS)  $(tj_LDFLAGS)  $(LDPATH)  $(LDLIBS) $(STDLIBS) $(LDOPTS) > link.out

rawlink.out :
	@echo ""
	@echo Link report ...
	$(RHLINKINFO)/rawlinksummary $(link_op) $(SOURCES_obj) $(SOURCES_res) $(OBJECTS)  $(BUGFIX_OBJECTS)  $(tj_LDFLAGS)  $(LDPATH)  $(LDLIBS) $(STDLIBS) $(LDOPTS) > rawlink.out

rawestlink.out :
	@echo ""
	@echo Link report ...
	$(RHLINKINFO)/rawestlinksummary $(link_op) $(SOURCES_obj) $(SOURCES_res) $(OBJECTS)  $(BUGFIX_OBJECTS)  $(tj_LDFLAGS)  $(LDPATH)  $(LDLIBS) $(STDLIBS) $(LDOPTS) > rawestlink.out

# Automatic build of load libraries -- works for Dynamic build, but not static at present
# because of ordering of the libraries -- Experimental at this stage
RHLIBDEPENDENCIES = $(IUELOCALROOT)/RHUtilities/interpreter/libdependencies/libdependencies$(EXESUFIX)

ifndef IMMEDIATE_LIBS
   IMMEDIATE_LIBS = $(LDLIBS)
endif

ldlibs :
	$(MAKE) NODEPENDS:=1 IU_LibDependencies
	@echo ""
	@echo Making LDLIBS.mk library dependency file
	@if test -f LDLIBS.mk ; then ( \
	   mv LDLIBS.mk LDLIBS.mk.bak$$$$ \
	) ; fi
	$(RHLIBDEPENDENCIES) -i < IU_LibDependencies > LDLIBS.mk

# This keeps the file IU_LibDependencies up to date.  This file records the
# present state of LDLIBS in a separate file.  
IU_LibDependencies : makefile 
	@echo "Updating IU_LibDependencies"
	@echo " " $(IMMEDIATE_LIBS) " " | sed -e 's/[ 	][ 	]*/ /g' -e 's/ -[^l][^ ]*//g' -e 's/ -l/ /g' -e 's/\.lib / /g' -e 's/^ //' > IU_LibDependencies

resolve-libs:
	@echo $(foreach l, $(LDLIBS:-l%=lib%.*) $(STDLIBS:-l%=lib%.*), $(firstword $(wildcard $(LIBDIRS:%=%/$l))))

# Create Microsoft Visual Studio Project (.dsp) file from makefile
dspfile = $(LIBRARY)_$(BUILD).dsp
dswfile = $(LIBRARY).dsw
dspsources = $(RESOURCE_SOURCES) $(SOURCES) $(wildcard $(addsuffix .h,$(srcbase)))
MAKE_DSP = \
$(IUE_PERL) $(IUEROOT)$/win32bin$/makedsp.pl \
 "$(MINI_PROG_SOURCES)" \
 "$(MINI_PROG_EXES)" \
 "$(MINI_SO_SOURCES)" \
 "$(MINI_SO_EXES)" \
 "$(PROGRAM)" \
 "$(LIBRARY)" \
 "$(USES)" \
 $(BUILD) \
 $(OBJDIR) \
 "$(shlibname)" \
 "$(LDFLAGS)" \
 "$(LDPATH)" \
 "$(LDLIBS)" \
 $(dspfile) \
 $(LIBRARY)_$(BUILD) \
 "$(LIBRARY)" \
 "$(DEFINES) $(CVFLAGS) $(APP_CFLAGS)" \
 "$(DEFINES) $(CVFLAGS) $(tj_CPPFLAGS) $(APP_CCFLAGS)" \
 "$(CGFLAGS)" \
 "$(ide_optimize)" \
 "$(ide_debug)" \
 $(OBJDIR) \
 $(LIBDEST) \
 $(IUELOCALROOT) \
 $(IUEROOT) \
 $(dspsources)
MAKE_DSW = \
$(IUE_PERL) -S makedsw.pl \
 "$(MINI_PROG_SOURCES)" \
 "$(MINI_PROG_EXES)" \
 "$(MINI_SO_SOURCES)" \
 "$(MINI_SO_EXES)" \
 "$(PROGRAM)" \
 "$(LIBRARY)" \
 "$(USES)" \
 $(BUILD) \
 $(OBJDIR) \
 "$(LDLIBS)" \
 $(dspfile) \
 $(LIBRARY)_$(BUILD) \
 "$(LIBRARY)" \
 "$(DEFINES) $(CVFLAGS) $(APP_CFLAGS)" \
 "$(DEFINES) $(CVFLAGS) $(tj_CPPFLAGS) $(APP_CCFLAGS)" \
 "$(CGFLAGS)" \
 "$(ide_optimize)" \
 "$(ide_debug)" \
 $(OBJDIR) \
 $(LIBDEST) \
 $(IUELOCALROOT) \
 $(IUEROOT) \
 $(dspsources)

workspacefiles:
	@$(MAKE_DSW)

studiofiles: subdirs-studiofiles
	@$(MAKE_DSP)

### Local Variables: ###
### mode:font-lock ###
### End: ###

#       Copyright (c) 1993-1997 Amerinex Applied Imaging, Inc.    
#      409 Main St  Amherst, MA  01002     All Rights Reserved                 
#  Reproduction rights limited as described in the COPYRIGHT file.
#
#    Rules for performing a template closure.
#?	Must follow definition of IUEBUILDROOTS, and LOCALTDATADIRS.
#?	If IUELIBTARGETROOT is undefined, computes closure for an application,
#?	placing instance files in a tmpl-inst subdirectory.
#

****** THIS FILE IS UNDER CONSTRUCTION.   IT IS NOT CURRENTLY IN USE  **********


#----------------------------------------------------------------------
#? Template instances for applications go into a tmpl-inst subdirectory
# of the current directory.  Instances of working build trees go into
# a src/tmpl-lib/tmpl-inst directory at the root of the build tree.
# Instances of root build trees get distributed according to the source
# location of the first class argument of the tmplate instance.
#----------------------------------------------------------------------

# Template closure proceeds by scanning the linker output for undefined
# symbols that look like tempate instance symbols, creating source
# files that explicitly instantiate the templates, and rerunning make.
# A major issue with template instantiation is in what library template
# instances should be placed.  We see three choices:
#  1) perform closure only when linking executables and link them into
#     the executable being created.
#  2) add them to the library being made.
#  3) add them to the library containing the code for the type for which
#     the template is instantiated.
# The first choice is easy to implement, but requires relinking to perform
# template closure and whenever the template instance files need to be
# recompiled.  Also, the same template code may appear in many executables
# and every executable linking against a library missing an instance
# will need to compile it.
# The last choice minimizes the code in libraries, since instances exist
# only once.  However, after template closure, users need to remake the
# other libraries.
# This file implements the second choice.  Each library will contain the
# code for templates it uses, but that do not appear in other libraries.
# We also perform template closure on executable linkage in case it
# references templates in code that is not in a library for which
# template closure has been performed.

# Template closure relies on information contained in .tdata and .taliases
# files created for each class involved in a template instantiation (i.e.,
# the templated class and classes used as template arguments.  The mechanism
# uses information contained in classlist.txt files in order to find the
# necessary .tdata files (see iuecreateclasslist.pl).  Suggestions for
# alternative solutions are welcome.


#???
#----------------------------------------------------------------------
# Applications will find tdata files in any of the parent snapshots and
# its local tmpl-data directories.  Build trees (both local and
# central) will find tdata files in their composite tmpl-data directory
# and in any of the parent snapshots.
#----------------------------------------------------------------------
ifeq ($(origin LOCALTDATADIRS),undefined)
  ifneq ($(origin IUELIBTARGETROOT),undefined)
    LOCALTDATADIRS :=
  else
    LOCALTDATADIRS :=	$(SRC)
  endif
endif
IUETDATADIRS =		$(LOCALTDATADIRS) \
			$(IUEROOTS:%=%/include/tmpl-data) \
			$(IUEROOTS:%=%/src/lib/TargetJr)

#???
#----------------------------------------------------------------------
# The target make-appl-tmpl-inst ensures that all existing template
# instances are up-do-date.  The makefile 'makes' this target recursively
# whenever instances are created.
#----------------------------------------------------------------------

APPL-TMPL-INSTANCES = $(wildcard tmpl-inst/*.cc)
APPL-TMPL-OBJECTS = $(APPL-TMPL-INSTANCES:%.cc=%.o)

make-appl-tmpl-inst:	$(APPL-TMPL-OBJECTS)


#======================================================================
# library closure uses tuse files. (applications can use
# template-closure too).
#======================================================================
# template-closure instantiates and compiles, in a single directory,
# the templates used by the current package.  This target is
# non-null only in build trees that have at least one parent.  Trees
# w/o parents have their instances distributed into packages,
# according to the package containing the first user-defined class
# argument to the template.  As a result, these trees require that
# templates are instantiated before the libraries are created.  The
# target full-template-closure, performs the closure and defers
# compilation until the libraries are compiled.
#
# The template-closure target should be 'made' after the library
# target so you find errors in library code before recompiling
# templates that may depend on the library code.
#
# Applications should use local-template-closure, which always puts the
# template instances in a local subdirectory.
#
# If NOCLOSURE= is present on the make command line, no template
# closure/compilation is performed.
#----------------------------------------------------------------------

IUESCANLINKFORINSTANCESCMD = perl -x $(IUELOCALROOT)/Scripts/Perl/_iuescanlinkforinstances
IUEINSTANTIATECLOSURECMD = export IUE_OBJDIR=${OBJDIR}; perl -x $(IUELOCALROOT)/Scripts/Perl/iueinstantiateclosure

ifeq ($(origin NOCLOSURE),undefined)

ifndef TMPDIR
 TMPDIR := /tmp
endif

#----------------------------------------------------------------------
# define rule to compute closure by performing a link and inspecting the
# output.  This command sequence is used both to compute closure on a
# library by performing a dummy link, and as part of the commands to
# link an application.  This sequence performs the following steps:
# 1) ensure existing template instances are up-to-date using recursive make.
# 2) link application or library with all instances.
# 3) scan the link output with _iuetemplateclosurefromlink to detect undefined
#    template symbols and instantiate instances.
# 4) if instances were instantiated, repeat these steps.
#----------------------------------------------------------------------
define template-closure-link
	linkout=$(TMPDIR)/linkout$$$$; \
	instances=$(TMPDIR)/instances$$$$; \
	lstatfile=$(TMPDIR)/linkstatus$$$$; \
	objs="$(filter %.o,$(filter-out tmpl-inst/%,$^))"; \
	estat=2; \
	while [ $$estat = 2 ]; do \
	  echo Ensure tmpl-inst directory is up-to-date; \
	  echo $(MAKE) make-appl-tmpl-inst; \
	  $(MAKE) make-appl-tmpl-inst; \
	  if [ $$? != 0 ]; then exit 1; fi; \
	  tmplobjs=`ls tmpl-inst/*.o 2> /dev/null`; \
	  rm -f $$lstatfile; \
	  if [ "$$IUElinktype" = "library" ]; then \
	    echo ""; \
	    echo '--LINKING LIBRARY TO FIND UNDEFINED TEMPLATE INSTANCES--'; \
	    echo '--UNDEFINED SYMBOLS ARE EXPECTED--'; \
	    echo ""; \
	  fi; \
	  echo $$IUElinkwrapper $(LINK.o) $$objs $$tmplobjs $(LOADLIBES) $(LDLIBS) -o $@ \| tee $$linkout; \
	  ($$IUElinkwrapper $(LINK.o) $$objs $$tmplobjs $(LOADLIBES) $(LDLIBS) -o $@; \
	   lstat=$$?; if [ $$lstat != 0 ]; then echo $$lstat > $$lstatfile; fi) 2>&1 | tee $$linkout; \
	  if [ -f $$lstatfile ]; then \
	    if [ "$$IUElinktype" = "library" ]; then \
	      echo ""; \
	      echo '--THE UNDEFINED SYMBOL ERRORS SHOWN ABOVE ARE EXPECTED DURING TEMPLATE LINK--'; \
	    fi; \
	    echo '--SCANNING LINKER OUTPUT FOR UNDEFINED TEMPLATE SYMBOLS--'; \
	    echo ""; \
	    echo "$(IUESCANLINKFORINSTANCESCMD)" \< $$linkout \> $$instances; \
	    $(IUESCANLINKFORINSTANCESCMD) < $$linkout > $$instances; \
	    echo rm -f $$linkout; \
	    rm -f $$linkout; \
	    echo "$(IUEINSTANTIATECLOSURECMD)" -f $$instances -L tmpl-inst $(IUETDATADIRS); \
	    $(IUEINSTANTIATECLOSURECMD) -f $$instances -L tmpl-inst $(IUETDATADIRS); \
	    estat=$$?; \
	    echo rm -f $$instances; \
	    rm -f $$instances; \
	    echo ""; \
	    echo '--RERUNNING MAKE TO BRING IN NEW TEMPLATE INSTANCES--'; \
	    echo ""; \
	  else \
	    estat=0; \
	  fi; \
	done; \
	if [ "$$IUElinktype" = "library" ]; then \
	  echo ""; \
	  echo '--TEMPLATE CLOSURE IS COMPLETE FOR THIS LIBRARY--'; \
	  echo ""; \
	fi
endef

#----------------------------------------------------------------------
# Link command intended to follow compute-closure-link for shared libraries
#----------------------------------------------------------------------
define sharedlibclosure
endef


#----------------------------------------------------------------------
# define rules to compute closure using explicit .tuse files.
#----------------------------------------------------------------------

ifneq ($(origin IUELIBTARGETROOT),undefined)
TMPLINSTPKGDIR := $(IUEBUILDROOT)/src/tmpl-lib
else
TMPLINSTPKGDIR := .
endif
TMPLINSTLOCFLAG := -L $(TMPLINSTPKGDIR)/tmpl-inst

local-template-closure:
	@instances=$(TMPDIR)/instances.$$$$; \
	 echo "$(IUESCANLINKFORINSTANCESCMD)" -t $(SRC) \| sort  -u \> $$instances; \
	 $(IUESCANLINKFORINSTANCESCMD) -t $(SRC) \| sort  -u  > $$instances; \
	 echo "$(IUEINSTANTIATECLOSURECMD)" -f $$instances $(TMPLINSTLOCFLAG) $(IUETDATADIRS); \
	 $(IUEINSTANTIATECLOSURECMD) -f $$instances $(TMPLINSTLOCFLAG) $(IUETDATADIRS); \
	 estat=$$?; \
	 rm -f $$instances; \
	 if [ $$estat = 2 ]; then \
	   cd $(TMPLINSTPKGDIR); \
	   $(MAKE) make-appl-tmpl-inst; \
	   if [ $$? != 0 ]; then exit 1; fi; \
	 elif [ $$estat != 0 ]; then \
	   exit 1; \
	 fi;

# for backward compatibility with old applications.  Use local-template-closure instead.
appl-template-closure:	local-template-closure

ifeq ($(origin IUE_NOPARENTS),undefined)

template-closure:	local-template-closure

full-template-closure:
	@instances=$(TMPDIR)/instances.$$$$; \
	 echo "$(IUESCANLINKFORINSTANCESCMD)" -t $(SRC) \| sort  -u  \> $$instances; \
	 $(IUESCANLINKFORINSTANCESCMD) -t $(SRC) \| sort  -u  > $$instances; \
	 echo "$(IUEINSTANTIATECLOSURECMD)" -f $$instances $(TMPLINSTLOCFLAG) $(IUETDATADIRS); \
	 $(IUEINSTANTIATECLOSURECMD) -f $$instances $(TMPLINSTLOCFLAG) $(IUETDATADIRS); \
	 if [ $$? != 0 -a $$? != 2 ]; then exit 1; fi; \
	 rm -f $$instances;

else

template-closure:

full-template-closure:
	@instances=$(TMPDIR)/instances.$$$$; \
	 echo "$(IUESCANLINKFORINSTANCESCMD)" -t $(SRC) \| sort  -u  \> $$instances; \
	 $(IUESCANLINKFORINSTANCESCMD) -t $(SRC) \| sort  -u  > $$instances; \
	 echo "$(IUEINSTANTIATECLOSURECMD)" -f $$instances -D $(IUETDATADIRS); \
	 $(IUEINSTANTIATECLOSURECMD) -f $$instances -D $(IUETDATADIRS); \
	 if [ $$? != 0 -a $$? != 2 ]; then exit 1; fi; \
	 rm -f $$instances;

endif


# working area makefiles may need this until they are updated.
# remove this after a while.
LIBTMPLCLOSURE=tryit


ifeq ($(origin IUELIBTARGETROOT),undefined)

#======================================================================
# application closure uses linker output.
#======================================================================
# Future addition: allow user to specify that executable should use
# a different instance directory, possibly based on the executable name.
# Note: must also modify pkgvars to define TXXSOURCES appropriately.
#----------------------------------------------------------------------


#----------------------------------------------------------------------
# override link rule to compute template closure as part of the link commands
#----------------------------------------------------------------------

%: %.o
	@$(template-closure-link); \
	if [ $$estat != 0 ]; then exit $$estat; fi; \
	if [ -f $$lstatfile ]; then \
	  estat=`cat $$lstatfile`; \
	  rm -f $$lstatfile; \
	  exit $$estat; \
	fi

#----------------------------------------------------------------------
# Create a template instantiating linker rule for purify and quantify too.
#----------------------------------------------------------------------
%.pure: %.o
	@IUElinkwrapper="$(PURIFYCMD)"; \
	$(template-closure-link); \
	if [ $$estat != 0 ]; then exit $$estat; fi; \
	if [ -f $$lstatfile ]; then \
	  estat=`cat $$lstatfile`; \
	  rm -f $$lstatfile; \
	  exit $$estat; \
	fi

%.quantify: %.o
	@IUElinkwrapper="$(QUANTIFYCMD)"; \
	$(template-closure-link); \
	if [ $$estat != 0 ]; then exit $$estat; fi; \
	if [ -f $$lstatfile ]; then \
	  estat=`cat $$lstatfile`; \
	  rm -f $$lstatfile; \
	  exit $$estat; \
	fi

# ----------------------------------------------------------------------
# include two rules in a separate file in order to overcome a bug in
# gmake 3.76.1
# ----------------------------------------------------------------------
include $(IUEMAKEINCLUDEDIR)/tmplclosure-extra.mk

endif


else
# NOCLOSURE is defined
template-closure:
full-template-closure:
endif

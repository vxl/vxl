#######################################
include $(IUEROOT)/config/top-params.mk
#######################################
#
# Purpose: Makefile for non-TestDriver tests, a la cool and Numerics
#
# Package: target
# File: config/static-tests.mk
# Author: Andrew W. Fitzgibbon, Oxford RRG
# Modifications:
#    120298 AWF Initial version
#    070402 Peter Vanroose - added TESTMAIN target

# if using separate test executables:
ifndef TESTMAIN
  MINI_PROG_SOURCES += $(TESTS)
endif

# if using single test driver:
ifdef TESTMAIN
  SOURCES += $(TESTMAIN) $(filter %.cxx,$(TESTS)) $(filter %.cc,$(TESTS))
  PROGRAM = $(subst .cxx,,$(TESTMAIN))
endif

ECHO = echo
SUMMARIZE = $(SEARCH) "Summary" 
ERROR    = 2>&1

# need this target here to override the targets in the included makefile
all: tests # run-all

EXESUFFIX := .exe

######################################
include $(IUEROOT)/config/top-rules.mk
######################################
LDLIBS := $(filter-out $(LINK_TESTLIB),$(LDLIBS))

# Now add our derived variables and targets

# TESTS specify the separate invocations of TESTMAIN:

RESULTS := $(patsubst %,$(RELOBJDIR)$/%.out,$(basename $(TESTS)))
$(RESULTS): FORCE

tests: $(RESULTS)

from = $(firstword $^)

ifndef TESTMAIN
$(RESULTS) : %.out : %.exe
 ifndef CLEAN
	$(STRIP) $(from);
 endif
	-$(from) $(PARAMS_$(basename $(@F))) > $@~
	mv $@~ $@
	-$(SUMMARIZE) $@ || true
else
$(RESULTS) : %.out : $(patsubst %,$(RELOBJDIR)$/%.exe,$(basename $(TESTMAIN)))
 ifndef CLEAN
	$(STRIP) $(from);
 endif
	-$(from) $(basename $(@F)) $(PARAMS_$(basename $(@F))) > $@~
	mv $@~ $@
	-$(SUMMARIZE) $@ || true
endif
ifdef CLEAN
	$(RM) $(from) $(patsubst %.out,%.$(OBJ_EXT),$@) \
	$(patsubst %.out,%.pdb,$@) $(patsubst %.out,%.ilk,$@)
endif

clean::
	$(RM) $(RESULTS)
	$(RM) $(allsources_obj_C)
	$(RM) $(MINI_PROG_EXES)

auxrun_tags = $(patsubst %,auxrun-%,$(basename $(TESTS)))

run-all: $(auxrun_tags)

# FIXME: this assumes objects are placed in a subdirectory of
# the current directory, rather than in the build tree.
auxrun-%: $(RELOBJDIR)$/%.exe
	@echo ""
	@./$(@:auxrun-%=$(RELOBJDIR)$/%.exe)
	@echo "."

ifndef usr_rules_mk
usr_rules_mk := 1

#
include $(IUEROOT)/config/top-params.mk


# temporary hack for making matlab mex files.
ifneq (,$(strip $(MEX_MINI_SOURCES)))
USE_MATLAB := 1

MINI_SO_SOURCES += $(MEX_MINI_SOURCES)

xall::$(MEX_MINI_SOURCES:%.cxx=%.mexsol)

$(MEX_MINI_SOURCES:%.cxx=%.mexsol) : %.mexsol : $(RELOBJDIR)/lib%.so
	cp $< $@
endif


# auto-compute transitive closure of libraries, if ald_libs given :
ifdef ald_libs
  ifneq ($(strip $(ald_libs)),)
    aldtmp := $(shell perl $(IUEROOT)/Scripts/Perl/fsm-libsort.pl $(ald_libs) $(ald_libdeps))
    IULIBS += $(aldtmp:%=-l%)
  endif
endif

# sort to make unique
#INCDIRS += $(sort ald_INCDIRS)
#LIBDIRS += $(sort ald_LIBDIRS)

# include old word makefile :
include $(IUEROOT)/config/top-rules.mk

endif

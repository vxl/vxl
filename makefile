ifdef VXLROOT

##########################################
include ${VXLROOT}/config/top-params.mk
##########################################

# Echo date into logfile at the beginning
# Echo pwd, so emacsing a log puts you in the correct dir
xall::
	@echo '# -*- default-directory: "'`pwd`'/" -*-'
	@echo Beginning vxl make: `date`


# Load packages.mk, containing the list of subdirs to make
include packages.mk
xall::
	@echo "*"
	@echo "* Found packages   : $(FOUND_PACKAGES)"
	@echo "* Missing packages : $(MISSING_PACKAGES)"
	@echo "*"

#--------------------------------------------------------------------------------

# fsm: it's bad to reconfigure automatically here. configure is dumb -- if it runs on
# two machines at the same time (e.g. cronbuild) it will fail horribly. if you're old
# enough to use cvs you're also old enough to run configure.
configure: configure.in aclocal.m4
	@echo "*** configure.in and/or aclocal.m4 is newer than configure."
	@echo "*** you should probably run autoconf and then reconfigure."
	@echo "*** not building."
	@false
	autoconf
xall:: configure


ifeq (yes, no) # see above
# if the user has config, then set up the auto run of configure
LOCAL_CONFIG = $(wildcard $(VXLROOT)/config)
ifneq (win32,$(OS))
ifneq "" "$(LOCAL_CONFIG)"

#if the user has autoconf then run it when configure.in changes
ifndef NOAUTOCONFIG
ifeq ($(AUTOCONF),autoconf)

xall:: configure

endif

# only wild card on the IUEOSTYPE of the current machine
CONFIG_STATUS_FILES =  $(wildcard $(CONFIG_STATUS) ./config.status.$(CXX).$(IUEOSTYPE) ./config.status.$(IUEOSTYPE)-$(COMPILER) ./config.status.$(IUEOSTYPE) ./config.status )

# all config status files should be targets in the main build xall
xall:: reconfigure

reconfigure: $(CONFIG_STATUS_FILES)

# awf: why $@F ?
# Note that this should work when . is not in the path, hence the explicit call to sh
$(CONFIG_STATUS_FILES): configure
	-env CFLAGS= CONFIG_STATUS=$@ sh $@ --recheck
	-sh $@
	-/bin/mv config.log $(subst status,log,$(@F))

endif # LOCAL_CONFIG
endif # win32
endif # NOAUTOCONFIG
endif

#--------------------------------------------------------------------------------

PACKAGES := $(IU_PACKAGES)  $(GUI_PACKAGES) $(SITE_PACKAGES) $(PROGRAMS) $(SITE_PROGRAMS)

SUBDIRS := config Scripts $(PACKAGES)


ifdef SUB
# Can't use command-line override of SUBDIRS, as it propagates all the way down,
# So to make a subset of dirs, use
#  make SUB='Foo Bar'
SUBDIRS := $(SUB)
endif

ifdef FROM
SUBDIRS := $(shell echo $(SUBDIRS) | perl -pe 's/^.* $(FROM) /$(FROM) /')
endif


##########################################
include ${VXLROOT}/config/top-rules.mk
##########################################


# Echo date into logfile at the end
xall::
	@echo Done vxl make: `date`

TAGS: FORCE
	env FRESCO=$(FRESCOINCS) VXLROOT=`pwd` PACKAGES='$(PACKAGES)' tj-make-etags

#
just_print_subdirs:
	@echo $(SUBDIRS)

cvs-update:
	cvs up -l
	cvs up -d $(SUBDIRS)

else
all:
	@echo You have to set the environment variable VXLROOT before running make
	@exit 1
endif

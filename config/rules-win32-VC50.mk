#############################################################################
## Generic makefile for gnumake
# Module: Default targets
# Notes: this file is used for adding any rules that are specific to a platform
#

#############################################################################
################################# IDL_INTERFACE #############################

# Variable: IDL_INTERFACE
# A single proxy/stub made from $(IDL_SOURCES).

allsources_idl := $(filter %.idl,$(IDL_SOURCES))
allsources_tlb_idl := $(allsources_idl:%.idl=%.$(TLB_EXT))
SOURCES_tlb += $(patsubst %,%.$(TLB_EXT),$(basename $(IDL_SOURCES)))
COMPILE.idl = $(IDL_COMPILER) $(APP_IDLFLAGS) $(source_for_object)

$(allsources_tlb_idl) : %.$(TLB_EXT) : %.idl
	@echo ""
	@echo "-- Compiling $<"
	$(COMPILE.idl)

ifneq ($(strip $(IDL_INTERFACE)),)

IDL_INTERFACESUFFIX := .dll

idl_interface := $(RELOBJDIR)$/$(IDL_INTERFACE)ps$(IDL_INTERFACESUFFIX)

xall:: $(idl_interface)

##
IDL_SOURCES_c += dlldata.c
IDL_SOURCES_c += $(IDL_INTERFACE)_p.c
IDL_SOURCES_c += $(IDL_INTERFACE)_i.c
IDL_SOURCES_h += $(IDL_INTERFACE).h

$(IDL_SOURCES_c) $(IDL_SOURCES_h) : $(IDL_INTERFACE).idl
	@echo ""
	@echo "-- Compiling $<"
	$(COMPILE.idl)

IDL_SOURCES_obj += $(patsubst %,$(RELOBJDIR)$/%.$(OBJ_EXT),$(basename $(IDL_SOURCES_c)))

$(IDL_SOURCES_obj) : $(RELOBJDIR)$/%.$(OBJ_EXT) : %.c
	@echo ""
	@echo "-- Compiling Idl Interface $<"
	$(CC) $(OUTPUT_OPTION) -c -DWIN32 -D_WIN32_WINNT=0x0400 -DREGISTER_PROXY_DLL $(source_for_object)

idl_interface_libfile := $(LIBDEST)$/$(LIB_PREFIX)$(IDL_INTERFACE).$(AR_EXT)

$(idl_interface_libfile): $(LIBDEST)$/stamp $(IDL_SOURCES_obj)
	@echo ""
	@echo "-- Creating Library for Idl Interface $(IDL_INTERFACE)"
	$(MAKE_ARLIB) $(RELOBJDIR)$/$(IDL_INTERFACE)_i.$(OBJ_EXT)
	$(RM) -r $(OBJDIR)/*.o.C $(OBJDIR)/ii_files

$(idl_interface): $(idl_interface_libfile)
	@echo ""
	@echo "-- Linking Idl Interface $(IDL_INTERFACE)"
	cl.exe -nologo -LD $(IDL_SOURCES_obj) kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib -Fe$(idl_interface) -link -machine:IX86 -def:$(IDL_INTERFACE)ps.def -LIBPATH:$(subst /,\\,$(LIBDEST))
#
endif # IDL_INTERFACE

#############################################################################
################################# ATL_EXE_COMPONENT #########################

ifneq ($(strip $(ATL_EXE_COMPONENT)),)

progfile := $(RELOBJDIR)$/$(ATL_EXE_COMPONENT)$(PROGSUFFIX)

xall:: $(progfile)

ifdef MAKE_SHARED
## Assume no lib deps for shared builds
statlibs :=
else
statlibs := $(LIB_DEPENDENCIES)
endif

ifneq "" "$(strip $(LINK_INTO_TMP))"
 link_tmpname = tmplink.$(@F).$(subst /,_,$(OBJDIR))
 link_op = $(foreach @,$(link_tmpname),$(LINK_OUT_OPTION))
 link_mv = mv $(link_tmpname) $@
else
 link_op = $(LINK_OUT_OPTION)
 link_mv = @$(EMPTY_COMMAND)
endif # LINK_INTO_TMP

$(progfile): $(idl_interface) $(SOURCES_obj) $(SOURCES_res) $(OBJECTS) $(statlibs)
	@echo ""
	@echo "-- [Win32] Linking ATL Component $(ATL_EXE_COMPONENT)"
	$(MAKE_EXE) $(link_op) $(SOURCES_obj) $(SOURCES_res) $(OBJECTS) $(BUGFIX_OBJECTS) $(tj_LDFLAGS) $(LDPATH) $(LDLIBS) $(LDOPTS)
	$(link_mv)
	$(RM) -r $(OBJDIR)/*.o.C $(OBJDIR)/ii_files

endif # ATL_EXE_COMPONENT

#############################################################################
############################## ATL_INPROC_COMPONENT #########################

ifneq ($(strip $(ATL_INPROC_COMPONENT)),)

progfile := $(RELOBJDIR)$/$(ATL_INPROC_COMPONENT).dll
atl_ldopts := -def:$(ATL_INPROC_COMPONENT).def

xall:: $(progfile)

ifdef MAKE_SHARED
## Assume no lib deps for shared builds
statlibs :=
else
statlibs := $(LIB_DEPENDENCIES)
endif

ifneq "" "$(strip $(LINK_INTO_TMP))"
 link_tmpname = /tmp/tmplink.$(@F).$(subst /,_,$(OBJDIR))
 link_op = $(foreach @,$(link_tmpname),$(LINK_OUT_OPTION))
 link_mv = mv $(link_tmpname) $@
else
 link_op = $(LINK_OUT_OPTION)
 link_mv = @$(EMPTY_COMMAND)
endif # LINK_INTO_TMP

$(progfile): $(idl_interface) $(SOURCES_obj) $(SOURCES_res) $(OBJECTS) $(statlibs)
	@echo ""
	@echo "-- [Win32] Linking ATL Component $(ATL_INPROC_COMPONENT)"
	$(MAKE_EXE) -LD $(link_op) $(SOURCES_obj) $(SOURCES_res) $(OBJECTS) $(BUGFIX_OBJECTS) $(tj_LDFLAGS) $(LDPATH) $(LDLIBS) $(LDOPTS) $(atl_ldopts)
	$(link_mv)
	$(RM) -r $(OBJDIR)/*.o.C $(OBJDIR)/ii_files

endif # ATL_INPROC_COMPONENT


library.$(BUILD): $(libfile)

ifeq ($(LIBRARY),)
endif

dspfile = $(LIBRARY)_$(BUILD).dsp
dswfile = $(LIBRARY).dsw
dspsources = $(RESOURCE_SOURCES) $(SOURCES) $(wildcard $(addsuffix .h,$(srcbase)))
MAKE_DSP = \
$(IUE_PERL) -S makedsp.pl \
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
#MAKE_DSW = $(IUE_PERL) -S makedsw.pl $(dswfile) $(LIBDEST) $(OBJDIR) $(LIBDEST) $(IUELOCALROOT) $(IUEROOT) "$(BUILDTYPES)"

xall::

workspacefiles:
	@$(MAKE_DSW)

studiofiles: subdirs-studiofiles
	@$(MAKE_DSP)
#	@$(MAKE_DSW)


### Local Variables: ###
### mode:font-lock ###
### End: ###

#  I guess this seems about as good a place as any to put the information about
#  building with the sun compiler.  I am working with sun CC 4.2 with patch
#  104631-07. 
#  explicit instantiation of inline functions does not work right.
#  If you have a templated friend function inside a template class, 
#  you have to declare it as a template function outside the scope of the class
#  as well.   IF not, the compiler does not know that it is a template function.
#
#  static members of templates get auto-instantiated (global!) even with the  -instances=explicit
#  This causes duplicate symbol problems

OS_DEFINES := -DSVR4 -DSYSV
MAKE_ARLIB = rm -f $@ && ar qcv $@
RPATHLIBDIRS = $(strip $(LIBDIRS))
ifeq ($(BUILD),noshared)
RPATHLIBDIRS = $(strip $(filter-out %.noshared,$(LIBDIRS)))
endif
rpath = $(RPATHLIBDIRS:%=-R%)
lib_dl := -ldl

MAKEDEPEND := $(C++) -c -xM
MAKE_SHLIB := $(C++) -G
pic := -Kpic
PIC := -KPIC
wall :=  -verbose=no%template
debug = -g

# Sunpro4.2  needs the following library:
STDLIBS += -lsunmath
ifdef TJ_SUNPRO50
  # Sunpro5.0 also needs this :
  STDLIBS += -lCstd
endif

C++ := $(C++)

# This option tells the compiler not to place string literals
# in read-only memory, to quell the copious warnings produced
# by code like this :
#   char *ptr = "foo";
# fsm
ccflags += -features=no%conststrings

LINK_INTO_TMP := 1

ifdef USE_IMPLICIT_TEMPLATES
# This is for SunPro 5.0, but at the moment it stalls mysteriously in vnl/algo.
implicit_templates := -instances=extern -verbose=template
no_implicit_templates := -instances=extern -verbose=template
else
# 5.0 works with this. So should 4.2 after installing the patch described in INSTALL-IUE.
# in this mode the compiler will instantiate templates with global linkage and
# only implicitly instantiate inline functions, so we turn on the flag
# -DINLINE_EXPLICIT_FLAG 
# awf: We now need semiexplicit to get copy(..,..) instantiated in SCLInstances.
# awf: No we don't, semiexplicit produces multiple iostream instantiations, as
#      basic_ios<T> is often required as a result of template instantiation, and
#      semiexplicit doesn't do weak symbols.
no_implicit_templates := -instances=explicit -template=wholeclass -DINLINE_EXPLICIT_FLAG

# in this mode the compiler will implicitly instantiate templates with static linkage
# NOTE: it will also make explicit templates static, all explicit instantiations have to
# be in the Template directories
#fsm implicit_templates := -instances=static -DINSTANTIATE_TEMPLATES
implicit_templates := -instances=explicit -DINSTANTIATE_TEMPLATES

# various old flag settings
#implicit_templates := -instances=explicit -template=wholeclass  -DINLINE_EXPLICIT_FLAG
#implicit_templates := -instances=static -template=wholeclass 
#implicit_templates := -pta
#no_implicit_templates := -pta
endif

SolarisArchitecture := 1


#ccflags := -verbose=template
#optimize := -O
# optimization is broken in CC
optimize := 

# Sunpro CC does not understand the .S suffix
# The following rule allows .S files to be preprocessed and assembled
# with SunPro CC-4.2
# Doesn't work with sunpro 5, but CC file.S -c does...

ifndef TJ_SUNPRO50
OS_COMPILE.S = \
tmp_name=$(source_for_object).$$$$ ; \
cp  $(source_for_object) $(TMPDIR)/$$tmp_name.c ; \
cpp -DSVR4 -D__SUNPRO_CC $(TMPDIR)/$$tmp_name.c  > $(TMPDIR)/$$tmp_name.s ; \
/bin/rm -f $(TMPDIR)/$$tmp_name.c; \
as $(TMPDIR)/$$tmp_name.s -K pic $(OUTPUT_OPTION); \
estat=$$?; \
/bin/rm -f $(TMPDIR)/$$tmp_name.s; \
if [ $$estat != 0 ]; then exit $$estat ; fi; 

OS_COMPILE.s = $(CC) $(source_for_object) $(extraflags_for_object) $(CFLAGS) -c $(OUTPUT_OPTION)
else
OS_COMPILE.S = $(C++) $(source_for_object) $(extraflags_for_object) $(CFLAGS) -c $(OUTPUT_OPTION)
OS_COMPILE.s = $(CC) $(source_for_object) $(extraflags_for_object) $(CFLAGS) -c $(OUTPUT_OPTION)
endif

ifeq "$(HAS_GLUT)" "1"
GLUT_XILIB	:= -lXi
endif

## this hack deletes the template repository after each link in
## order to overcome a sunpro quirk: if two programs are linked in
## from the same directory, template instances shared between the
## two may refer to symbols which are defined in only one of the 
## program object files. the symptom is that the other link fails
## due to undefined symbols (they are defined by the other program).
SUNPRO_REPO_HACK = /bin/test -d $(@D)/SunWS_cache && /bin/rm -rf $(@D)/SunWS_cache ; true

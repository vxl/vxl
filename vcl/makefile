#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

# This rule will not actually get updated
# until after dependencies have been made
# but that will probably be OK as the .h
# files in ./iso are now under cvs.
# The || true is to make sure the build
# doesn't fail for systems where perl is
# not available.
xall::
	( cd ./iso && $(MAKE) ) || true

USES := vxl

LIBRARY := vcl
VERSION := 1.0

ifndef NOTEST
SUBDIRS = tests
endif

SOURCES += vcl_cmath.cxx
SOURCES += vcl_cstdlib.cxx
SOURCES += vcl_cassert.cxx
SOURCES += vcl_unistd.cxx
SOURCES += $(wildcard vcl_rtti.cxx)
SOURCES += $(wildcard vcl_debug.cxx)
SOURCES += $(wildcard vcl_chkr.cxx)

ifndef USE_NATIVE_STL
SOURCES += emulation/vcl_straits.cxx
SOURCES += emulation/vcl_hashtable.cxx
SOURCES += emulation/vcl_string_instances.cxx
SOURCES += emulation/vcl_alloc.cxx
SOURCES += emulation/vcl_rbtree_instances.cxx
endif

DEFINES += -DBUILDING_VCL_DLL

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

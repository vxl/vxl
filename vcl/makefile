#######################################
include ${IUEROOT}/config/usr_params.mk
#######################################

USES := vxl

LIBRARY := vcl
VERSION := 1.0

ifndef NOTEST
SUBDIRS = tests
endif

SOURCES += vcl_cmath.cxx
SOURCES += vcl_unistd.cxx
SOURCES += $(wildcard vcl_rtti.cxx)
SOURCES += $(wildcard vcl_debug.cxx)

ifndef USE_NATIVE_STL
SOURCES += emulation/vcl_straits.cxx
SOURCES += emulation/vcl_hashtable.cxx
SOURCES += emulation/vcl_string_instances.cxx
SOURCES += emulation/vcl_alloc.cxx
SOURCES += emulation/vcl_rbtree_instances.cxx
endif

DEFINES += -DBUILDING_VCL_DLL

######################################
include ${IUEROOT}/config/usr_rules.mk
######################################

VXL := 1

################################
include ${IUEROOT}/config/top-params.mk
################################

USES := vxl

LIBRARY := vcl
VERSION := 1.0

ifneq (1, $(NOTEST))
SUBDIRS = tests
endif

ifndef USE_NATIVE_STL
SOURCES += emulation/vcl_straits.cxx
SOURCES += emulation/vcl_hashtable.cxx
SOURCES += emulation/vcl_string_instances.cxx
SOURCES += emulation/vcl_alloc.cxx
SOURCES += emulation/vcl_rbtree_instances.cxx
endif

DEFINES += -DBUILDING_VCL_DLL

################################
include ${IUEROOT}/config/top-rules.mk
################################

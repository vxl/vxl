#   DESCRIPTION:
#
#   This is the vbl (Vision Basics) library.
#
#   DEPENDENCIES:  libvcl

##########################################
include ${IUEROOT}/config/top-params.mk
##########################################

VXL := 1 
USES += vxl
IULIBS := -lvcl

LIBRARY := vbl
VERSION := 1.0

ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

SOURCES += vbl_arg.cxx  # if this is in a .txx file, gcc 2.95 will not generate any code.
SOURCES += vbl_awk.cxx
SOURCES += vbl_base_array_3d.cxx
SOURCES += vbl_clamp.cxx
SOURCES += vbl_file.cxx
SOURCES += vbl_printf.cxx
SOURCES += vbl_ref_count.cxx
SOURCES += vbl_reg_exp.cxx
SOURCES += vbl_sparse_array_2d_base.cxx
SOURCES += vbl_sprintf.cxx
SOURCES += vbl_string.cxx
SOURCES += vbl_timer.cxx
SOURCES += vbl_timestamp.cxx
SOURCES += vbl_types.cxx
SOURCES += vbl_user_info.cxx
SOURCES += vbl_bool_ostream.cxx

DEFINES += -DBUILDING_VBL_DLL

##########################################
include ${IUEROOT}/config/top-rules.mk
##########################################

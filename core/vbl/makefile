#   DESCRIPTION:
#
#   This is the vbl (Vision Basics) library.
#
#   DEPENDENCIES:  libvcl
##########################################
include ${IUEROOT}/config/usr_params.mk
##########################################

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
SOURCES += vbl_file.cxx
SOURCES += vbl_printf.cxx
SOURCES += vbl_ref_count.cxx
SOURCES += vbl_reg_exp.cxx
SOURCES += vbl_sparse_array_2d_base.cxx
SOURCES += vbl_sprintf.cxx
SOURCES += vbl_string.cxx
SOURCES += vbl_timer.cxx
SOURCES += vbl_get_timestamp.cxx
SOURCES += vbl_timestamp.cxx  # ??
SOURCES += vbl_types.cxx
SOURCES += vbl_user_info.cxx
SOURCES += vbl_bool_ostream.cxx
SOURCES += vbl_qsort.cxx
SOURCES += $(wildcard vbl_vector.cxx)
SOURCES += vbl_psfile.cxx
SOURCES += vbl_sequence_filename_map.cxx

# debugging
SOURCES += vbl_trace.cxx
SOURCES += $(wildcard vbl_get_argc_argv.cxx)

DEFINES += -DBUILDING_VBL_DLL

##########################################
include ${IUEROOT}/config/usr_rules.mk
##########################################

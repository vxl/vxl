include ${fmk_ROOT}/fmk_params.mk

fmk_USES += vxl

fmk_LIBRARY := vbl
fmk_VERSION := 1.0

ifndef NOTEST
#fmk_SUBDIRS += tests
#fmk_SUBDIRS += examples
endif

fmk_LIB_SOURCES += vbl_arg.cxx  # if this is in a .txx file, gcc 2.95 will not generate any code.
fmk_LIB_SOURCES += vbl_awk.cxx
fmk_LIB_SOURCES += vbl_base_array_3d.cxx
fmk_LIB_SOURCES += vbl_file.cxx
fmk_LIB_SOURCES += vbl_printf.cxx
fmk_LIB_SOURCES += vbl_ref_count.cxx
fmk_LIB_SOURCES += vbl_reg_exp.cxx
fmk_LIB_SOURCES += vbl_sparse_array_2d_base.cxx
fmk_LIB_SOURCES += vbl_sprintf.cxx
fmk_LIB_SOURCES += vbl_string.cxx
fmk_LIB_SOURCES += vbl_timer.cxx
fmk_LIB_SOURCES += vbl_get_timestamp.cxx
fmk_LIB_SOURCES += vbl_timestamp.cxx  # ??
fmk_LIB_SOURCES += vbl_types.cxx
fmk_LIB_SOURCES += vbl_user_info.cxx
fmk_LIB_SOURCES += vbl_bool_ostream.cxx
fmk_LIB_SOURCES += vbl_qsort.cxx
fmk_LIB_SOURCES += $(wildcard vbl_vector.cxx)
fmk_LIB_SOURCES += vbl_psfile.cxx
fmk_LIB_SOURCES += vbl_sequence_filename_map.cxx

# debugging
fmk_LIB_SOURCES += vbl_trace.cxx
fmk_LIB_SOURCES += $(wildcard vbl_get_argc_argv.cxx)

#FIXME
#DEFINES += -DBUILDING_VBL_DLL

include ${fmk_ROOT}/fmk_rules.mk

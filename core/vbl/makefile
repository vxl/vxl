#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES += vxl
IMMEDIATE_LIBS := -lvcl

LIBRARY := vbl
VERSION := 1.0

SOURCES += vbl_arg.cxx  # if this is in a .txx file, gcc 2.95 will not generate any code.
SOURCES += vbl_awk.cxx
SOURCES += vbl_base_array_3d.cxx
SOURCES += vbl_file.cxx
ifneq ($(HAS_BROKEN_AS)-$(BUILD),1-noshared) # this file generates 4281 bytes assembler line (Rb_tree) which is too much.
SOURCES += vbl_canonical_path.cxx
endif
SOURCES += vbl_printf.cxx
SOURCES += vbl_redirector.cxx
SOURCES += vbl_ref_count.cxx
SOURCES += vbl_reg_exp.cxx
SOURCES += vbl_sparse_array_2d_base.cxx
SOURCES += vbl_sprintf.cxx
SOURCES += vbl_string.cxx
SOURCES += vbl_timer.cxx
SOURCES += vbl_get_timestamp.cxx
SOURCES += vbl_timestamp.cxx  # ??
SOURCES += vbl_user_info.cxx
SOURCES += vbl_bool_ostream.cxx
SOURCES += vbl_qsort.cxx
SOURCES += vbl_psfile.cxx
SOURCES += vbl_sequence_filename_map.cxx

MANPAGE_SOURCES += vbl_array.txx
MANPAGE_SOURCES += vbl_array_2d.txx
MANPAGE_SOURCES += vbl_array_3d.txx
MANPAGE_SOURCES += vbl_basic_relation.txx
MANPAGE_SOURCES += vbl_binary.txx
MANPAGE_SOURCES += vbl_bounding_box.txx
MANPAGE_SOURCES += vbl_br_default.txx
MANPAGE_SOURCES += vbl_br_impl.txx
MANPAGE_SOURCES += vbl_hash_map.txx
MANPAGE_SOURCES += vbl_protection_traits.txx
MANPAGE_SOURCES += vbl_smart_ptr.txx
MANPAGE_SOURCES += vbl_sort.txx
MANPAGE_SOURCES += vbl_sparse_array.txx
MANPAGE_SOURCES += vbl_sparse_array_2d.txx
MANPAGE_SOURCES += vbl_sparse_array_3d.txx

# debugging
SOURCES += vbl_trace.cxx

DEFINES += -DBUILDING_VBL_DLL

ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

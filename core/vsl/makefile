#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

USES := vxl

LIBRARY := vsl
VERSION := 1.0


SOURCES += vsl_indent.cxx
SOURCES += vsl_binary_io.cxx
SOURCES += vsl_binary_loader_base.cxx

# Additional files to document:
MANPAGE_SOURCES += vsl_fwd.h
MANPAGE_SOURCES += vsl_binary_loader.txx
MANPAGE_SOURCES += vsl_clipon_binary_loader.txx
MANPAGE_SOURCES += vsl_binary_explicit_io.h
MANPAGE_SOURCES += vsl_complex_io.txx
MANPAGE_SOURCES += vsl_deque_io.txx
MANPAGE_SOURCES += vsl_list_io.txx
MANPAGE_SOURCES += vsl_map_io.txx
MANPAGE_SOURCES += vsl_pair_io.txx
MANPAGE_SOURCES += vsl_set_io.txx
MANPAGE_SOURCES += vsl_stack_io.txx
MANPAGE_SOURCES += vsl_string_io.txx
MANPAGE_SOURCES += vsl_vector_io.txx
MANPAGE_SOURCES += vsl_quick_file.h

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

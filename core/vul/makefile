USES += core

LIBRARY := vul
VERSION := 1.0

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

MANPAGE_SOURCES += vul_fwd.h

MANPAGE_SOURCES += vul_whereami.h
SOURCES += vul_arg.cxx  # if this is in a .txx file, gcc 2.95 will not generate any code.
SOURCES += vul_awk.cxx
SOURCES += vul_file.cxx
SOURCES += vul_file_iterator.cxx
ifneq ($(HAS_BROKEN_AS),1) # this file generates 4281 bytes assembler line (Rb_tree) which is too much.
SOURCES += vul_expand_path.cxx
endif
SOURCES += vul_printf.cxx
SOURCES += vul_redirector.cxx
SOURCES += vul_reg_exp.cxx
SOURCES += vul_sprintf.cxx
SOURCES += vul_string.cxx
SOURCES += vul_timer.cxx
SOURCES += vul_get_timestamp.cxx
SOURCES += vul_timestamp.cxx  # ??
SOURCES += vul_user_info.cxx
SOURCES += vul_psfile.cxx
SOURCES += vul_sequence_filename_map.cxx
SOURCES += vul_url.cxx
SOURCES += vul_temp_filename.cxx

# debugging
SOURCES += vul_trace.cxx

DEFINES += -DBUILDING_VUL_DLL

# stream I/O
SUBDIRS += io

ifndef NOTEST
SUBDIRS += tests
SUBDIRS += examples
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################

ifeq "$(TJ_GCC30)$(optimize)" "1-O2"
# gcc 3.0 needs too much virtual memory with -O2 or -O1
FLAGS_vul_sequence_filename_map.cxx := -O0
endif

#
# ./vxl/vul/CMakeLists.txt
#
#   ian.m.scott@stud.man.ac.uk
#

# Set up vul_where_vxl.h to have a macro set to $VXL_SOURCE_DIR
CONFIGURE_FILE(
      ${allvxl_SOURCE_DIR}/vxl/vul/vul_where_vxl.h.in
      ${allvxl_SOURCE_DIR}/vxl/vul/vul_where_vxl.h ESCAPE_QUOTES)


ADD_LIBRARY(vul vul_sources)

SOURCE_FILES( vul_sources
vul_arg
vul_reg_exp
vul_awk
vul_sequence_filename_map
vul_file
vul_sprintf
vul_file_iterator
vul_string
vul_get_timestamp
vul_test
vul_timer
vul_printf
vul_timestamp
vul_psfile
vul_trace
vul_user_info
vul_redirector
vul_expand_path
)

AUX_SOURCE_DIRECTORY(Templates vul_sources)

SUBDIRS(io)

IF( BUILD_TESTS)
  SUBDIRS(tests examples)
ENDIF( BUILD_TESTS)

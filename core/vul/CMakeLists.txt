#
# ./vxl/vul/CMakeLists.txt
#
#   ian.m.scott@stud.man.ac.uk
#


ADD_LIBRARY(vul vul_sources)


FIND_FILE(VUL_VCL_WHERE_ROOT_DIR_H vcl_where_root_dir.h 
  ${allvxl_BINARY_DIR}/vcl )


IF (VUL_VCL_WHERE_ROOT_DIR_H)
  ADD_DEFINITIONS(-DVCL_WHERE_ROOT_DIR_H_EXISTS)
ENDIF(VUL_VCL_WHERE_ROOT_DIR_H)



SOURCE_FILES( vul_sources
vul_arg.cxx
vul_reg_exp.cxx
vul_awk.cxx
vul_sequence_filename_map.cxx
vul_file.cxx
vul_sprintf.cxx
vul_file_iterator.cxx
vul_string.cxx
vul_get_timestamp.cxx
vul_test.cxx
vul_timer.cxx
vul_printf.cxx
vul_timestamp.cxx
vul_psfile.cxx
vul_trace.cxx
vul_user_info.cxx
vul_redirector.cxx
vul_expand_path.cxx
vul_root_dir.cxx
vul_url.cxx
)

AUX_SOURCE_DIRECTORY(Templates vul_sources)

SUBDIRS(io)

IF( BUILD_TESTING)
  SUBDIRS(tests examples)
ENDIF( BUILD_TESTING)

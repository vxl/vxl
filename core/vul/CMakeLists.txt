# ./vxl/vul/CMakeLists.txt

ADD_LIBRARY(vul vul_sources)

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
  vul_url.cxx
  vul_temp_filename.cxx
)

AUX_SOURCE_DIRECTORY(Templates vul_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/vxl/vul/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

SUBDIRS(io)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

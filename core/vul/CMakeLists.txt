# vxl/vul/CMakeLists.txt

ADD_LIBRARY(vul vul_sources)

SOURCE_FILES(vul_sources
  vul_fwd.h
  vul_canonical_path.h
  vul_whereami.h

  vul_arg.h                   vul_arg.cxx
  vul_awk.h                   vul_awk.cxx
  vul_expand_path.h           vul_expand_path.cxx
  vul_file.h                  vul_file.cxx
  vul_file_iterator.h         vul_file_iterator.cxx
  vul_get_timestamp.h         vul_get_timestamp.cxx
  vul_printf.h                vul_printf.cxx
  vul_psfile.h                vul_psfile.cxx
  vul_redirector.h            vul_redirector.cxx
  vul_reg_exp.h               vul_reg_exp.cxx
  vul_sequence_filename_map.h vul_sequence_filename_map.cxx
  vul_sprintf.h               vul_sprintf.cxx
  vul_string.h                vul_string.cxx
  vul_temp_filename.h         vul_temp_filename.cxx
  vul_timer.h                 vul_timer.cxx
  vul_timestamp.h             vul_timestamp.cxx
  vul_trace.h                 vul_trace.cxx
  vul_url.h                   vul_url.cxx
  vul_user_info.h             vul_user_info.cxx

#  vul_lru_helper.txx          vul_lru_helper.h
)

AUX_SOURCE_DIRECTORY(Templates vul_sources)

SUBDIRS(io)

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)

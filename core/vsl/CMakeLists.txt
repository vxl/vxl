# vxl/vsl/CMakeLists.txt

SET(vsl_sources
  vsl_fwd.h

  vsl_binary_io.cxx vsl_binary_io.h
  vsl_binary_explicit_io.h
  vsl_binary_loader_base.cxx vsl_binary_loader_base.h
  vsl_indent.cxx vsl_indent.h
  vsl_quick_file.h
  vsl_block_binary.h
  vsl_b_read_block_old.h

  vsl_binary_loader.txx vsl_binary_loader.h
  vsl_clipon_binary_loader.txx vsl_clipon_binary_loader.h
  vsl_complex_io.txx vsl_complex_io.h
  vsl_deque_io.txx vsl_deque_io.h
  vsl_list_io.txx vsl_list_io.h
  vsl_map_io.txx vsl_map_io.h
  vsl_pair_io.txx vsl_pair_io.h
  vsl_set_io.txx vsl_set_io.h
  vsl_stack_io.txx vsl_stack_io.h
  vsl_string_io.txx vsl_string_io.h
  vsl_vector_io.txx vsl_vector_io.h vsl_vector_io_bool.cxx
                                    vsl_vector_io_vector_bool.cxx
)

AUX_SOURCE_DIRECTORY(Templates vsl_sources)

ADD_LIBRARY(vsl ${vsl_sources})
TARGET_LINK_LIBRARIES( vsl vcl )

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)

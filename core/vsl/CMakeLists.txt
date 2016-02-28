# vxl/vsl/CMakeLists.txt

doxygen_add_library(core/vsl
  PACKAGE core
  DESCRIPTION "Streaming Library for Binary I/O"
  )

set(vsl_sources
  vsl_fwd.h
  vsl_export.h
  vsl_binary_io.cxx vsl_binary_io.h
  vsl_binary_explicit_io.h
  vsl_binary_loader_base.cxx vsl_binary_loader_base.h
  vsl_indent.cxx vsl_indent.h
  vsl_quick_file.h
  vsl_block_binary.h  vsl_block_binary.cxx
  vsl_b_read_block_old.h
  vsl_stream.h
  vsl_block_binary_rle.h

  vsl_binary_loader.hxx vsl_binary_loader.h
  vsl_clipon_binary_loader.hxx vsl_clipon_binary_loader.h
  vsl_complex_io.hxx vsl_complex_io.h
  vsl_deque_io.hxx vsl_deque_io.h
  vsl_list_io.hxx vsl_list_io.h
  vsl_map_io.hxx vsl_map_io.h
  vsl_pair_io.hxx vsl_pair_io.h
  vsl_set_io.hxx vsl_set_io.h
  vsl_stack_io.hxx vsl_stack_io.h
  vsl_string_io.hxx vsl_string_io.h
  vsl_vector_io.hxx vsl_vector_io.h vsl_vector_io_bool.cxx
                                    vsl_vector_io_vector_bool.cxx
  vsl_basic_xml_element.cxx vsl_basic_xml_element.h
)

aux_source_directory(Templates vsl_sources)

vxl_add_library(LIBNAME ${VXL_LIB_PREFIX}vsl LIBSRCS ${vsl_sources})
target_link_libraries( ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl )

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

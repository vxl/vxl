# core/vil/io/CMakeLists.txt

set(vil_io_sources
  vil_io_memory_chunk.cxx  vil_io_memory_chunk.h
  vil_io_smart_ptr.txx     vil_io_smart_ptr.h
  vil_io_image_view.h
  vil_io_image_view_base.h vil_io_image_view_base.cxx
)
aux_source_directory(Templates vil_io_sources)

if(CMAKE_COMPILER_IS_GNUCXX)
  set_source_files_properties(Templates/vsl_vector_io+vcl_vector+vil_image_view+float---.cxx
                              PROPERTIES
                              COMPILE_FLAGS -ftemplate-depth-50)
endif()

add_library(vil_io ${vil_io_sources})
target_link_libraries( vil_io vil vsl )

install_targets(/lib vil_io)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/core/vil/io ${vil_io_sources})

if( BUILD_TESTING)
  add_subdirectory(tests)
endif()

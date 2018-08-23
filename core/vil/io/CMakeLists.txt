# core/vil/io/CMakeLists.txt

set(vil_io_sources
  vil_io_memory_chunk.cxx  vil_io_memory_chunk.h
  vil_io_smart_ptr.hxx     vil_io_smart_ptr.h
  vil_io_image_view.h
  vil_io_image_view_base.h vil_io_image_view_base.cxx
)
aux_source_directory(Templates vil_io_sources)

if(CMAKE_COMPILER_IS_GNUCXX)
  set_source_files_properties(Templates/vsl_vector_io+vcl_vector+vil_image_view+float---.cxx
                              PROPERTIES
                              COMPILE_FLAGS -ftemplate-depth-50)
endif()

vxl_add_library(LIBRARY_NAME ${VXL_LIB_PREFIX}vil_io LIBRARY_SOURCES ${vil_io_sources})
target_link_libraries( ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vsl )

if( BUILD_TESTING)
  add_subdirectory(tests)
endif()

if (VXL_BUILD_OBJECT_LIBRARIES)
  add_library(vil_io-obj OBJECT ${vil_io_sources})
endif ()

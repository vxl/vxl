# core/vbl/examples/CMakeLists.txt

set( vbl_example_templates_sources
  # empty
)
aux_source_directory( Templates vbl_example_templates_sources )

vxl_add_library( LIBNAME ${VXL_LIB_PREFIX}vbl_example_templates LIBSRCS ${vbl_example_templates_sources} )
target_link_libraries( ${VXL_LIB_PREFIX}vbl_example_templates ${VXL_LIB_PREFIX}vbl )

add_executable( vbl_smart_ptr_example
  vbl_smart_ptr_example.cxx    vbl_smart_ptr_example.h
)
target_link_libraries( vbl_smart_ptr_example ${VXL_LIB_PREFIX}vbl_example_templates ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vcl )

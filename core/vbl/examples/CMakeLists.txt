# core/vbl/examples/CMakeLists.txt

set( vbl_example_templates_sources
  # empty
)
aux_source_directory( Templates vbl_example_templates_sources )

add_library( vbl_example_templates ${vbl_example_templates_sources} )
target_link_libraries( vbl_example_templates vbl )

add_executable( vbl_smart_ptr_example
  vbl_smart_ptr_example.cxx    vbl_smart_ptr_example.h
)
target_link_libraries( vbl_smart_ptr_example vbl_example_templates vbl vcl )

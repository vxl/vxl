# vxl/vbl/examples/CMakeLists.txt

SET( vbl_example_templates_sources
  # empty
)
AUX_SOURCE_DIRECTORY( Templates vbl_example_templates_sources )

ADD_LIBRARY( vbl_example_templates ${vbl_example_templates_sources} )
TARGET_LINK_LIBRARIES( vbl_example_templates vbl )

ADD_EXECUTABLE( vbl_smart_ptr_example
  vbl_smart_ptr_example.cxx    vbl_smart_ptr_example.h
  Templates/smartptr-instances.cxx
)
TARGET_LINK_LIBRARIES( vbl_smart_ptr_example vbl_example_templates vbl vcl )

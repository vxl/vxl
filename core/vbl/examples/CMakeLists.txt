# vxl/vbl/examples/CMakeLists.txt

ADD_EXECUTABLE( vbl_smart_ptr_example
  vbl_smart_ptr_example.cxx    vbl_smart_ptr_example.h
)
TARGET_LINK_LIBRARIES( vbl_smart_ptr_example vbl vcl )

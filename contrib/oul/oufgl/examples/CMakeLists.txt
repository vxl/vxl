# oul/oufgl/examples/CMakeLists.txt

add_executable( oufgl_example1 example1.cxx )
target_link_libraries( oufgl_example1 oufgl ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vul )

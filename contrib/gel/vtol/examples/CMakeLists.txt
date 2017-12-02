
add_executable( vtol_example2d           example2d.cxx )
target_link_libraries( vtol_example2d    vtol ${VXL_LIB_PREFIX}vcl )

add_executable( vtol_example3d           example3d.cxx )
target_link_libraries( vtol_example3d    vtol ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vcl )

add_executable( vtol_example_vdgl        example_vdgl.cxx )
target_link_libraries( vtol_example_vdgl vtol vsol vdgl ${VXL_LIB_PREFIX}vcl )

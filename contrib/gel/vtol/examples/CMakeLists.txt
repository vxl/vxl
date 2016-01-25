include_directories( ${VXLCORE_INCLUDE_DIR} )

add_executable( vtol_example2d           example2d.cxx )
target_link_libraries( vtol_example2d    vtol vcl )

add_executable( vtol_example3d           example3d.cxx )
target_link_libraries( vtol_example3d    vtol vbl vcl )

add_executable( vtol_example_vdgl        example_vdgl.cxx )
target_link_libraries( vtol_example_vdgl vtol vsol vdgl vcl )
